#include<linux/kernel.h>
#include<linux/gpio.h>
#include<linux/timer.h>
#define LED_PIN 529
static int led=0;
static struct timer_list led_timer;

static void led_blink(struct timer_list *tm){
led=!led;
gpio_set_value(LED_PIN, led);
mod_timer(&led_timer,jiffies+msecs_to_jiffies(1000));
}

static int __init basic_init(void){
	int ret;
	if(gpio_is_valid(LED_PIN)==false){
	printk(KERN_INFO "the gpio pin is not valid\n");
	return -1;
	}
	ret=gpio_request(LED_PIN,"LED");
	if(ret<0){
	printk(KERN_INFO "the gpio request fail\n");
	return ret;
	}
	gpio_direction_output(LED_PIN, 0);

	timer_setup(&led_timer, led_blink,0);
	mod_timer(&led_timer, jiffies+msecs_to_jiffies(1000));
printk(KERN_INFO "init module loaded\n");
return 0;
}
static void __exit basic_exit(void){
gpio_free(LED_PIN);
	printk(KERN_INFO "module was unloaded\n");

}


module_init(basic_init);
module_exit(basic_exit);

MODULE_LICENSE("GPL");
