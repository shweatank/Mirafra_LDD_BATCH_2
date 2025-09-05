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
#define RPI_I2C_S		0x4
#define RPI_I2C_DLEN	        0x8
#define RPI_I2C_A		0xc
#define RPI_I2C_FIFO    	0x10
#define RPI_I2C_DIV		0x14
#define RPI_I2C_DEL		0x18

#define RPI_I2C_CLKT	        0x1c


#define RPI_I2C_FEDL_SHIFT	16
#define RPI_I2C_REDL_SHIFT	0

#define I2C_CDIV_MIN	        0x0002
#define I2C_CDIV_MAX	        0xFFFE



struct i2cadapt_dev {
	void __iomem *reg;   /* Virtual address for register access */
	struct device *dev;
	int irq;
	struct clk *bus_clk;
	struct i2c_adapter i2c_adapter;
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

static irqreturn_t rpi_i2c_isr(int this_irq, void *data)
{
      pr_info("i'm in isr function\n");
      return IRQ_HANDLED;

}
static int rpi_i2c_xfer(struct i2c_adapter *adap, struct i2c_msg msgs[],int num)
{
        pr_info(" hello i'm xfer function \n");
	return 0;
}
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

       ret = request_irq(i2c_dev->irq, rpi_i2c_isr, IRQF_SHARED,
                  dev_name(&pdev->dev), i2c_dev);
       if (ret) {
             dev_err(&pdev->dev, "Could not request IRQ %d, error %d\n", i2c_dev->irq, ret);
            goto err_disable_unprepare_clk;
      } else {
            dev_info(&pdev->dev, "IRQ %d successfully requested\n", i2c_dev->irq);
      }
      

        adap = &i2c_dev->i2c_adapter;
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
    
    i2c_del_adapter(&i2c_dev->i2c_adapter);
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
MODULE_AUTHOR("GANGA K");
MODULE_DESCRIPTION("writting test code for i2c adapter v2");

