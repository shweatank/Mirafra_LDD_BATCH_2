#include<linux/module.h>
#include<linux/of.h>
#include<linux/platform_device.h>
#include<linux/kernel.h>
#include<linux/i2c.h>
#include <linux/clk.h>
#include <linux/clkdev.h>
#include <linux/clk-provider.h> 
#include <linux/of_device.h>



#define RPI_I2C_C		0x0
#define RPI_I2C_S       0x4
#define RPI_I2C_DLEN	0x8
#define RPI_I2C_A		0xc
#define RPI_I2C_FIFO    0x10
#define RPI_I2C_DIV		0x14
#define RPI_I2C_DEL		0x18

#define RPI_I2C_CLKT	0x1c



#define RPI_I2C_C_READ	BIT(0)
#define RPI_I2C_C_CLEAR	BIT(4) /* bits 4 and 5 both clear */
#define RPI_I2C_C_ST	BIT(7)
#define RPI_I2C_C_INTD	BIT(8)
#define RPI_I2C_C_INTT	BIT(9)
#define RPI_I2C_C_INTR	BIT(10)
#define RPI_I2C_C_I2CEN	BIT(15)   /* I2C Enable */

#define RPI_I2C_S_TA	BIT(0)
#define RPI_I2C_S_DONE	BIT(1)
#define RPI_I2C_S_TXW	BIT(2)
#define RPI_I2C_S_RXR	BIT(3)
#define RPI_I2C_S_TXD	BIT(4)
#define RPI_I2C_S_RXD	BIT(5)
#define RPI_I2C_S_TXE	BIT(6)
#define RPI_I2C_S_RXF	BIT(7)
#define RPI_I2C_S_ERR	BIT(8)
#define RPI_I2C_S_CLKT	BIT(9)
#define RPI_I2C_S_LEN	BIT(10)


#define RPI_I2C_FEDL_SHIFT	16
#define RPI_I2C_REDL_SHIFT	0

#define I2C_CDIV_MIN	    0x0002
#define I2C_CDIV_MAX	    0xFFFE




struct i2cadapt_dev {
	void __iomem *reg;   /* Virtual address for register access */
	struct device *dev;
	int irq;
	struct i2c_adapter adapter;
	struct completion completion;
	struct i2c_msg *curr_msg;
	struct clk *bus_clk;
	int num_msgs;
	u32 msg_err;
	u8 *msg_buf;
	size_t msg_buf_remaining;
};

#define to_clk_i2c(_hw) container_of(_hw, struct clk_bcm2835_i2c, hw)
struct clk_bcm2835_i2c {
	struct clk_hw hw;
	struct i2cadapt_dev *i2c_dev;
};


static inline void rpi_i2c_writel(struct i2cadapt_dev *i2c_dev,u32 reg, u32 val)
{
	writel(val, i2c_dev->reg + reg);
}

static inline u32 rpi_i2c_readl(struct i2cadapt_dev *i2c_dev, u32 reg)
{
	return readl(i2c_dev->reg + reg);
}

static int clk_i2c_calc_divider(unsigned long rate,unsigned long parent_rate)
{
	u32 divider = DIV_ROUND_UP(parent_rate, rate);

	/*
	 * Per the datasheet, the register is always interpreted as an even
	 * number, by rounding down. In other words, the LSB is ignored. So,
	 * if the LSB is set, increment the divider to avoid any issue.
	 */
	if (divider & 1)
		divider++;
	if ((divider < I2C_CDIV_MIN) ||
			(divider > I2C_CDIV_MAX))
		return -EINVAL;

	return divider;
}

static int clk_i2c_set_rate(struct clk_hw *hw, unsigned long rate,unsigned long parent_rate)
{

	u32 redl, fedl;
	u32 divider;
	struct clk_bcm2835_i2c *div;
	div = to_clk_i2c(hw);

	pr_info("i'm in clc i2c set function");
	divider = clk_i2c_calc_divider(rate, parent_rate);
	if (divider == -EINVAL)
		return -EINVAL;
	rpi_i2c_writel(div->i2c_dev, RPI_I2C_DIV, divider);

	fedl = max(divider / 16, 1u);
	/*
	 * Number of core clocks to wait after rising edge before
	 * 	 * sampling the next incoming data bit.
	 * 	 	 */

	redl = max(divider / 4, 1u);

	rpi_i2c_writel(div->i2c_dev, RPI_I2C_DEL,
			(fedl << RPI_I2C_FEDL_SHIFT) |
			(redl << RPI_I2C_REDL_SHIFT));

	return 0;
}

static long clk_i2c_round_rate(struct clk_hw *hw, unsigned long rate,unsigned long *parent_rate)
{   
	u32 divider;	 
	pr_info("i2c_round_rate\n");
	divider = clk_i2c_calc_divider(rate, *parent_rate);

	return DIV_ROUND_UP(*parent_rate, divider);
}

static unsigned long clk_i2c_recalc_rate(struct clk_hw *hw,unsigned long parent_rate)
{
	u32 divider;
	struct clk_bcm2835_i2c *div;
	pr_info("i2c recal rate \n");
	div = to_clk_i2c(hw);
	divider = rpi_i2c_readl(div->i2c_dev, RPI_I2C_DIV);

	return DIV_ROUND_UP(parent_rate, divider);
}


static const struct clk_ops clk_i2c_ops = {
	.set_rate = clk_i2c_set_rate,
	.round_rate = clk_i2c_round_rate,
	.recalc_rate = clk_i2c_recalc_rate,
};


static struct clk *bcm2835_i2c_register_div(struct device *dev,struct clk *mclk,struct i2cadapt_dev *i2c_dev)
{
	struct clk_init_data init;
	struct clk_bcm2835_i2c *priv;
	char name[32];
	const char *mclk_name;

	pr_info("clk i2c register devicer function \n ");
	snprintf(name, sizeof(name), "%s_div", dev_name(dev));

	mclk_name = __clk_get_name(mclk);

	init.ops = &clk_i2c_ops;
	init.name = name;
	init.parent_names = (const char* []) { mclk_name };
	init.num_parents = 1;
	init.flags = 0;

	priv = devm_kzalloc(dev, sizeof(struct clk_bcm2835_i2c), GFP_KERNEL);
	if (priv == NULL)
		return ERR_PTR(-ENOMEM);

	priv->hw.init = &init;
	priv->i2c_dev = i2c_dev;

	clk_hw_register_clkdev(&priv->hw, "div", dev_name(dev));
	return devm_clk_register(dev, &priv->hw);
}


static void rpi_fill_txfifo(struct i2cadapt_dev *i2c_dev)
{
	u32 val;
	dev_info(i2c_dev->dev,"I'M in txfifo fucntion \n");

	while (i2c_dev->msg_buf_remaining) {
		val = rpi_i2c_readl(i2c_dev, RPI_I2C_S);
		if (!(val & RPI_I2C_S_TXD))
			break;
		dev_info(i2c_dev->dev, "TX FIFO ready, writing byte: 0x%02x\n", *i2c_dev->msg_buf);
		rpi_i2c_writel(i2c_dev, RPI_I2C_FIFO,
				*i2c_dev->msg_buf);
		i2c_dev->msg_buf++;
		i2c_dev->msg_buf_remaining--;
	}
}

static void rpi_drain_rxfifo(struct i2cadapt_dev *i2c_dev)
{
	u32 val;

	dev_info(i2c_dev->dev,"im in rxfifo function \n");

	while (i2c_dev->msg_buf_remaining) {
		val = rpi_i2c_readl(i2c_dev,RPI_I2C_S);
		if (!(val & RPI_I2C_S_RXD))
			break;
		*i2c_dev->msg_buf = rpi_i2c_readl(i2c_dev,RPI_I2C_FIFO);
		dev_info(i2c_dev->dev, "RX FIFO data read: 0x%02x\n", *i2c_dev->msg_buf);
		i2c_dev->msg_buf++;
		i2c_dev->msg_buf_remaining--;
	}


}

static void rpi_i2c_start_transfer(struct i2cadapt_dev *dev);
#if 1
static irqreturn_t rpi_i2c_isr(int this_irq, void *data)
{


#if  1
	struct i2cadapt_dev *i2c_dev = data;
	u32 val, err;

	dev_info(i2c_dev->dev,"i'm in isr function\n");

	val = rpi_i2c_readl(i2c_dev, RPI_I2C_S);
	pr_info("the status register value:%d\n",val);

	err = val & (RPI_I2C_S_CLKT | RPI_I2C_S_ERR);

	pr_info("the clkt and err value in the status register:%x\n",err);

	if (err) {
		pr_info("the error bit the status register is enabled\n");
		i2c_dev->msg_err = err;
		goto complete;
	}
	if (val & RPI_I2C_S_DONE) {
		pr_info("the transfer is done(RPI_I2C_S_DONE)\n");
		if (!i2c_dev->curr_msg) {
			dev_err(i2c_dev->dev, "Got unexpected interrupt (from firmware?)\n");
		} else if (i2c_dev->curr_msg->flags & I2C_M_RD) {
			rpi_drain_rxfifo(i2c_dev);
			val = rpi_i2c_readl(i2c_dev, RPI_I2C_S);
		}

		if ((val & RPI_I2C_S_RXD) || i2c_dev->msg_buf_remaining)
			i2c_dev->msg_err = RPI_I2C_S_LEN;
		else
			i2c_dev->msg_err = 0;
		goto complete;

	}

	if (val & RPI_I2C_S_TXW) {
		pr_info("the transfer going on (RPI_I2C_S_TXW)\n");
		if (!i2c_dev->msg_buf_remaining) {
			i2c_dev->msg_err = val | RPI_I2C_S_LEN;
			goto complete;
		}

		rpi_fill_txfifo(i2c_dev);

		if (i2c_dev->num_msgs && !i2c_dev->msg_buf_remaining) {
			i2c_dev->curr_msg++;
			rpi_i2c_start_transfer(i2c_dev);
		}

		return IRQ_HANDLED;
	}


	if (val & RPI_I2C_S_RXR) {
		pr_info("the reciving going on (RPI_I2C_S_RXR)\n");
		if (!i2c_dev->msg_buf_remaining) {
			i2c_dev->msg_err = val | RPI_I2C_S_LEN;
			goto complete;
		}

		rpi_drain_rxfifo(i2c_dev);
		return IRQ_HANDLED;
	}

	return IRQ_NONE;

complete:
	rpi_i2c_writel(i2c_dev, RPI_I2C_C, RPI_I2C_C_CLEAR);
	rpi_i2c_writel(i2c_dev, RPI_I2C_S, RPI_I2C_S_CLKT |
			RPI_I2C_S_ERR | RPI_I2C_S_DONE);
	complete(&i2c_dev->completion);

#endif
	return IRQ_HANDLED;

}

#else

static irqreturn_t rpi_i2c_isr(int irq, void *dev_id){


	struct i2cadapt_dev *dev = dev_id;
	u32 status, err, val;

	status =  rpi_i2c_readl(dev,RPI_I2C_S);
	err   =  status & (RPI_I2C_S_CLKT | RPI_I2C_S_ERR);

	if(err){
		dev_err("the clkt and err value in the status register:%x and enabled \n",err);
		dev->msg_err = err;
		goto complete;
	}


	/* TX Transmit FIFO needs writing*/

	if( status & RPI_I2C_S_TXW ){

		if (!dev->msg_buf_remaining) {  /* error condition */
			dev->msg_err = status | RPI_I2C_S_LEN;
			goto complete;
		}

		dev_info(dev->dev,"send no of bytes  = %d\n",dev->msg_buf_remaining);
		while (dev->msg_buf_remaining) {
			val = rpi_i2c_readl(dev, RPI_I2C_S);
			if (!(val & RPI_I2C_S_TXD))   /* need to handle */
				break;
			dev_info(dev->dev, "TX FIFO ready, writing byte: 0x%02x\n", *dev->msg_buf);
			rpi_i2c_writel(dev, RPI_I2C_FIFO, *dev->msg_buf);
			dev->msg_buf++;
			dev->msg_buf_remaining--;
		}

		if (dev->num_msgs && !dev->msg_buf_remaining) {
			dev->curr_msg++; // now points to msgs[1] if the nums_msg is 2
			dev_info(dev->dev," again start condition \n");
			rpi_i2c_start_transfer(dev);
		}

		return IRQ_HANDLED;
	}

	if(status & RPI_I2C_S_RXR){

		dev_info(dev->dev,"the reciving going on RXR)\n");

		if (!dev->msg_buf_remaining) {
			dev->msg_err = val | RPI_I2C_S_LEN;
			goto complete;
		}

                rpi_drain_rxfifo(dev);
		return IRQ_HANDLED;
	}

	/* Transfer done */
	if (status & RPI_I2C_S_DONE) {
		dev_info(dev->dev,"the transfer is done(RPI_I2C_S_DONE)\n");
		dev->msg_err = 0;
		if (!i2c_dev->curr_msg) {
			dev_err(dev->dev, "Got unexpected interrupt\n");
		} else if (i2c_dev->curr_msg->flags & I2C_M_RD) {
			rpi_drain_rxfifo(i2c_dev);
			val = rpi_i2c_readl(i2c_dev, RPI_I2C_S);
		}

		if ((val & RPI_I2C_S_RXD) || i2c_dev->msg_buf_remaining)
			i2c_dev->msg_err = RPI_I2C_S_LEN;
		else
			i2c_dev->msg_err = 0;
		goto complete;
	}


	dev_warn(dev->dev, "unexpected I2C status: 0x%x\n", status);
	return IRQ_NONE;

complete:
	rpi_i2c_writel(dev, RPI_I2C_C, RPI_I2C_C_CLEAR);
	rpi_i2c_writel(dev, RPI_I2C_S, RPI_I2C_S_CLKT | RPI_I2C_S_ERR | RPI_I2C_S_DONE);
	complete(&i2c_dev->completion); //Notify transfer complete

	return IRQ_HANDLED;
}


#endif

static void rpi_i2c_start_transfer(struct i2cadapt_dev *dev)
{
	u32 control_var = RPI_I2C_C_ST | RPI_I2C_C_I2CEN;

	struct i2c_msg *msg = dev->curr_msg;

	bool last_msg = (dev->num_msgs == 1);

	if(!dev->num_msgs)
		return;

	dev->num_msgs--;
	dev->msg_buf = msg->buf;
	dev->msg_buf_remaining = msg->len;



	if (msg->flags & I2C_M_RD)
		control_var |= RPI_I2C_C_READ | RPI_I2C_C_INTR;
	else
		control_var |= RPI_I2C_C_INTT;

	if (last_msg)
		control_var |= RPI_I2C_C_INTD;

	dev_info(dev->dev," started writting address and msg in register 1\n ");

	rpi_i2c_writel(dev, RPI_I2C_A, msg->addr);
	rpi_i2c_writel(dev, RPI_I2C_DLEN, msg->len);
	rpi_i2c_writel(dev, RPI_I2C_C, control_var);	

	dev_info(dev->dev,"write into mamory register completed \n ");
	return;
}


static void rpi_i2c_finish_transfer(struct i2cadapt_dev *dev)
{
	dev->curr_msg = NULL;
	dev->num_msgs = 0;

	dev->msg_buf = NULL;
	dev->msg_buf_remaining = 0;
}


static int rpi_i2c_xfer(struct i2c_adapter *adap, struct i2c_msg msgs[],int num)
{


	struct i2cadapt_dev *i2c_dev;
	unsigned long time_left;
	int i;


	i2c_dev  = i2c_get_adapdata(adap);
	dev_info(i2c_dev->dev," HELLO  I'M IN XFER FUN \n");

	dev_info(i2c_dev->dev,"num = %d\n", num);

#if 1 // check logs purpose 

	for (i = 0; i < num; i++) {
		const char *rw = (msgs[i].flags & I2C_M_RD) ? "READ" : "WRITE";
		dev_info(i2c_dev->dev, "msgs[%d] = %s, addr=0x%02x, len=%d\n",i, rw, msgs[i].addr, msgs[i].len);
		// Safely print first up to N bytes of buf as hex
		for (int b = 0; b < msgs[i].len && b < 8; b++)
			dev_info(i2c_dev->dev, "buf[%d]=0x%02x", b, msgs[i].buf[b]);
        }

#endif
	for (i = 0; i < (num - 1); i++){
            
		/* if first msg is read msg then  it will generate error and exit */
 		if (msgs[i].flags & I2C_M_RD) {
			dev_warn_once(i2c_dev->dev, "only one read message supported, has to be last\n");
			return -EOPNOTSUPP;
		}
	}


	i2c_dev->curr_msg = msgs;  //start with first message
	i2c_dev->num_msgs = num;  // total messages to process
	reinit_completion(&i2c_dev->completion);

	rpi_i2c_start_transfer(i2c_dev);
	dev_info(i2c_dev->dev,"adapt timeout =  %d \n", adap->timeout);
	time_left = wait_for_completion_timeout(&i2c_dev->completion,adap->timeout);

	rpi_i2c_finish_transfer(i2c_dev);
	if(!time_left) {
		dev_err(i2c_dev->dev,"time left is zero \n");
		rpi_i2c_writel(i2c_dev, RPI_I2C_C, RPI_I2C_C_CLEAR);
		dev_err(i2c_dev->dev,"i'm in error condition write and rpi _i2c cleared \n");
		return -ETIMEDOUT;
	}

	if (!i2c_dev->msg_err){
		dev_err(i2c_dev->dev,"msg  is ok fine \n");
		return num;
	}

	dev_dbg(i2c_dev->dev, "i2c transfer failed: %x\n", i2c_dev->msg_err);

	if (i2c_dev->msg_err & RPI_I2C_S_ERR)
		return -EREMOTEIO;


	return -EIO;

}


/* The BCM2835 was reported to have problems with clock stretching */
static const struct i2c_adapter_quirks rpi_i2c_quirks = {
	.flags = I2C_AQ_NO_CLK_STRETCH,
};

static u32 rpi_i2c_func(struct i2c_adapter *adap)
{
	return I2C_FUNC_I2C | I2C_FUNC_SMBUS_EMUL | I2C_FUNC_PROTOCOL_MANGLING;
}

static const struct i2c_algorithm rpi_i2c_algo = {
	.master_xfer = rpi_i2c_xfer,
	.functionality = rpi_i2c_func,
};


static int i2cadapt_probe(struct platform_device *pdev)
{
	struct i2cadapt_dev *i2c_dev;
	int ret;
	struct i2c_adapter *adap;
	struct clk *mclk;
	u32 bus_clk_rate;
	unsigned long rate;
	//  struct resource *res;

	dev_info(&pdev->dev, "hello I'm in i2cadapt probe function v2\n");

	/* allocate driver private structure */
	i2c_dev = devm_kzalloc(&pdev->dev, sizeof(*i2c_dev), GFP_KERNEL);
	if (!i2c_dev)
		return -ENOMEM;

	init_completion(&i2c_dev->completion);

	i2c_dev->dev = &pdev->dev;
	platform_set_drvdata(pdev, i2c_dev);

	i2c_dev->reg = devm_platform_get_and_ioremap_resource(pdev,0, NULL);
	if(IS_ERR(i2c_dev->reg)){

		dev_err(&pdev->dev, "failed read reg address ");
		return PTR_ERR(i2c_dev->reg);
	}

	dev_info(&pdev->dev, "I2C PHYSICAL ADDRESS %pa \n", &pdev->resource[0].start);
	/* print the mapped virtual address */
	dev_info(&pdev->dev, "I2C virtual base address: %p\n", i2c_dev->reg);

	mclk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(mclk)) {
		return dev_err_probe(&pdev->dev, PTR_ERR(mclk),"Could not get clock\n");
	}

	dev_info(&pdev->dev,"clock handle acquired successfuly \n");

	rate = clk_get_rate(mclk);
	dev_info(&pdev->dev, "I2C bus parent clock rate: %lu Hz\n", rate);

	i2c_dev->bus_clk = bcm2835_i2c_register_div(&pdev->dev, mclk, i2c_dev);

	if (IS_ERR(i2c_dev->bus_clk))
		return dev_err_probe(&pdev->dev, PTR_ERR(i2c_dev->bus_clk),
				"Could not register clock\n");


	ret = of_property_read_u32(pdev->dev.of_node, "clock-frequency",
			&bus_clk_rate);
	if (ret < 0) {
		dev_warn(&pdev->dev,
				"Could not read clock-frequency property\n");
		bus_clk_rate = I2C_MAX_STANDARD_MODE_FREQ;
	}

	dev_info(&pdev->dev, "I2C bus clock frequency from DT = %u Hz\n",bus_clk_rate);

	ret = clk_set_rate_exclusive(i2c_dev->bus_clk, bus_clk_rate);
	if (ret < 0)
		return dev_err_probe(&pdev->dev, ret,
				"Could not set clock frequency\n");

	rate = clk_get_rate(i2c_dev->bus_clk);
	dev_info(&pdev->dev, "Requested bus clock = %u Hz, actual clock = %lu Hz\n",
			bus_clk_rate, rate);

	ret = clk_prepare_enable(i2c_dev->bus_clk);
	if (ret) {
		dev_err(&pdev->dev, "Couldn't prepare clock, error %d\n", ret);
		goto err_put_exclusive_rate;
	} else {
		dev_info(&pdev->dev, "Clock prepared and enabled successfully\n");
	}

	i2c_dev->irq = platform_get_irq(pdev, 0);
	if (i2c_dev->irq < 0) {
		dev_err(&pdev->dev, "Failed to get IRQ: %d\n", i2c_dev->irq);
		ret = i2c_dev->irq;
		goto err_disable_unprepare_clk;
	}

#if 1
	ret = request_irq(i2c_dev->irq, rpi_i2c_isr, IRQF_SHARED,
			dev_name(&pdev->dev), i2c_dev);
	if (ret) {
		dev_err(&pdev->dev, "Could not request IRQ %d, error %d\n", i2c_dev->irq, ret);
		goto err_disable_unprepare_clk;
	} else {
		dev_info(&pdev->dev, "IRQ %d successfully requested\n", i2c_dev->irq);
	}

#endif
	adap = &i2c_dev->adapter;
	dev_info(&pdev->dev, "Setting i2c adapter data\n");
	i2c_set_adapdata(adap, i2c_dev);
	adap->owner = THIS_MODULE;
	adap->class = I2C_CLASS_DEPRECATED;
	snprintf(adap->name, sizeof(adap->name), "rpi2835 (%s)",of_node_full_name(pdev->dev.of_node));
	dev_info(&pdev->dev, "Adapter name: %s\n", adap->name);
	adap->algo = &rpi_i2c_algo;
	adap->dev.parent = &pdev->dev;
	adap->dev.of_node = pdev->dev.of_node;
	dev_info(&pdev->dev, "Getting quirks from device tree match data\n");
	adap->quirks = of_device_get_match_data(&pdev->dev);

	if (adap->quirks) {

		dev_info(&pdev->dev, "Quirks set successfully\n");
	} else {

		dev_warn(&pdev->dev, "No quirks found for this device\n");
	}
	/*
	 * Disable the hardware clock stretching timeout. SMBUS
	 * specifies a limit for how long the device can stretch the
	 * clock, but core I2C doesn't.
	 */
	rpi_i2c_writel(i2c_dev, RPI_I2C_CLKT, 0);
	rpi_i2c_writel(i2c_dev, RPI_I2C_C, 0);

	dev_info(&pdev->dev, "Hardware registers initialized\n");
	ret = i2c_add_adapter(adap);
	if (ret) {

		dev_err(&pdev->dev, "Failed to add i2c adapter: %d\n", ret);
		goto err_free_irq;
	}

	dev_info(&pdev->dev, "I2C adapter added successfully\n");
	return 0;


err_free_irq:
	free_irq(i2c_dev->irq, i2c_dev);
err_disable_unprepare_clk:
	clk_disable_unprepare(i2c_dev->bus_clk);
err_put_exclusive_rate:
	clk_rate_exclusive_put(i2c_dev->bus_clk);



	return ret;
}

static int  i2cadapt_remove(struct platform_device *pdev){

	struct i2cadapt_dev *i2c_dev;
	i2c_dev  = platform_get_drvdata(pdev);

	if (!i2c_dev) {
		dev_warn(&pdev->dev, "No driver data found during remove\n");
		return 0;
	}

	dev_info(&pdev->dev, "i2c adapter driver removed\n");

	i2c_del_adapter(&i2c_dev->adapter);
	free_irq(i2c_dev->irq, i2c_dev);
	clk_disable_unprepare(i2c_dev->bus_clk);
	clk_rate_exclusive_put(i2c_dev->bus_clk);

	return 0;
}


static const struct of_device_id mydriver_i2c_of_match[] ={
	{ .compatible = "gki2c,bcm-i2c" },
	{},
};

MODULE_DEVICE_TABLE(of,mydriver_i2c_of_match);

static struct platform_driver mydriver_i2c_adapter = {
	.probe    = i2cadapt_probe,
	.remove   = i2cadapt_remove,
	.driver   = {
		.name = "myi2c_adapter",
		.of_match_table = mydriver_i2c_of_match, 
	},
};


module_platform_driver(mydriver_i2c_adapter);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("kavya");
MODULE_DESCRIPTION("writting test code for i2c adapter added isr V4");
