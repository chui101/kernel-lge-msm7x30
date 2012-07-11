/* drivers/video/backlight/lm3530_bl.c
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* CONFIG_MACH_LGE	jihye.ahn	10.07.27
	LCD backlight driver	
*/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/backlight.h>
#include <linux/fb.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <mach/board.h>
//#include <mach/board-victor.h>
#include <mach/vreg.h>
#include <mach/board_lge.h>

#define MODULE_NAME	"lm3530"
#define CONFIG_BACKLIGHT_LEDS_CLASS

#ifdef CONFIG_BACKLIGHT_LEDS_CLASS
#include <linux/leds.h>
#endif

#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif

#if 0
//#if defined(LGE_MODEL_C729_REV_EVB)
#include <mach/pmic.h>

#define KYPD_LED_DEFAULT	1
//#endif
#endif

int saved_backlight_level = 0;

#if 1
atomic_t lcd_event_handled;
atomic_t lcd_bootup_handle;
#else

static int lm3530_write(struct i2c_client *client, u8 reg, u8 val);

atomic_t backlight_event_handle;


struct timer_list timerbl_bl;
struct timer_list timerbl_lcd;
struct i2c_client *bl_i2c_client;

int next_bl = 0;
//int current_bl = 0;

static void lcd_timer(unsigned long arg)
{
	lm3530_write(bl_i2c_client, 0xA0, next_bl);
	atomic_set(&backlight_event_handle,0);
}

static void bl_timer(unsigned long arg)
{
	if(atomic_read(&lcd_event_handled) == 0) 
	{
//		printk("##LMH_TEST	 retimer lcd_event_handled = 0\n") ;
		mod_timer(&timerbl_bl, jiffies + msecs_to_jiffies(30));
	}
	else
	{
//		printk("##LMH_TEST	BL-ON  test_curr=%d / current_bl=%d / next_bl = %d\n", test_curr,current_bl ,next_bl);
		mod_timer(&timerbl_lcd, jiffies + msecs_to_jiffies(40));
	}
}
#endif

/********************************************
 * Definition
 ********************************************/
//#define LCD_LED_MAX			0x7F			/* 20mA */
#define LCD_LED_MAX			0x71			/* 20mA */
#define LCD_LED_NOR			0X7A		/* 15mA */
#define LCD_LED_MIN			0x0			/* 2mA */
#define DEFAULT_BRIGHTNESS	LCD_LED_MAX


//#define MAX_BRIGHTNESS		(0xff)
//#define MIN_BRIGHTNESS		(0)



#ifdef CONFIG_BACKLIGHT_LEDS_CLASS
#define LEDS_BACKLIGHT_NAME "lcd-backlight"
#endif

enum {
	UNINIT_STATE=-1,
	POWERON_STATE,
	NORMAL_STATE,
	SLEEP_STATE,
	POWEROFF_STATE
} LM3530_STATE;

#define dprintk(fmt, args...) \
	do { \
		if(debug) \
			printk("%s:%s: " fmt, MODULE_NAME, __func__, ## args); \
	}while(0);


struct lm3530_driver_data {
	struct i2c_client *client;
	struct backlight_device *bd;
	struct led_classdev *led;
	int gpio;
	int intensity;
	int max_intensity;
	int state;
	int init_on_boot;
#ifdef CONFIG_HAS_EARLYSUSPEND
	struct early_suspend early_suspend;
#endif
};

/********************************************
 * Global variable
 ********************************************/
static unsigned int debug = 0; /* jihye.ahn	   debug message disable : 0 , enable : 1 */
module_param(debug, uint, 0644);
static int lm3530_cal_brightness(struct lm3530_driver_data *drvdata, int level);
#ifdef CONFIG_BACKLIGHT_LEDS_CLASS
static void leds_brightness_get(void);
#endif
/********************************************
 * Functions
 ********************************************/
 #if 0
static int lm3530_read(struct i2c_client *client, u8 reg, u8 *pval)
{
	int ret;
	int status;

	if (client == NULL) { 	/* No global client pointer? */
		dprintk("client is null\n");
		return -1;
	}

	if ((ret = i2c_smbus_read_byte_data(client, reg)) >= 0) {
		*pval = ret;
		status = 0;
	} else {
		status = -EIO;
		dprintk("fail to read(reg=0x%x,val=0x%x)\n", reg,*pval);
	}

	return status;
}
#endif

static int lm3530_write(struct i2c_client *client, u8 reg, u8 val)
{
	int ret;
	int status;

	if (client == NULL) {		/* No global client pointer? */
		dprintk("client is null\n");
		return -1;
	}

	ret = i2c_smbus_write_byte_data(client, reg, val);
	if (ret == 0) {
		status = 0;
	} else {
		status = -EIO;
		dprintk("fail to write(reg=0x%x,val=0x%x)\n", reg, val);
	}
	dprintk("TTT write(reg=0x%x,val=0x%x)\n", reg, val);

	return status;
}


static void lm3530_hw_reset(struct lm3530_driver_data *drvdata)
{
	if(drvdata->client && gpio_is_valid(drvdata->gpio)) {
#if 0		
		gpio_configure(drvdata->gpio, GPIOF_DRIVE_OUTPUT);
#else
		gpio_direction_output(drvdata->gpio, 1);
#endif
		gpio_set_value(drvdata->gpio, 1);
		udelay(5);
		gpio_set_value(drvdata->gpio, 0);
		udelay(5);
		gpio_set_value(drvdata->gpio, 1);
		udelay(5);
	}
}


static void lm3530_go_opmode(struct lm3530_driver_data *drvdata)
{
	
#if defined(LGE_MODEL_C729)
		if(lge_get_hw_rev() <= LGE_REV_D)
			lm3530_write(drvdata->client, 0x10, 0x15);
		else
		{	
			lm3530_write(drvdata->client, 0x10, 0x17);
			lm3530_write(drvdata->client, 0xA0, lm3530_cal_brightness(drvdata , saved_backlight_level));
			lm3530_write(drvdata->client, 0x30, 0x2D);			
		}
#else

#if defined(LGE_MODEL_C800_REV_EVB) || defined(LGE_MODEL_C800_REV_A)	
			lm3530_write(drvdata->client, 0x10, 0x15);
#else
			lm3530_write(drvdata->client, 0x10, 0x17);	//linear type
			lm3530_write(drvdata->client, 0xA0, lm3530_cal_brightness(drvdata , saved_backlight_level));
			lm3530_write(drvdata->client, 0x30, 0x2D);			
#endif

#endif
			drvdata->state = NORMAL_STATE;
}

static void lm3530_init(struct lm3530_driver_data *drvdata)
{
	if(drvdata->init_on_boot || system_state != SYSTEM_BOOTING) {
		lm3530_hw_reset(drvdata);
		lm3530_go_opmode(drvdata);
	}
	drvdata->state = NORMAL_STATE;
}


/* This function provide LM3530 sleep enter routine for power management. */
static void lm3530_sleep(struct lm3530_driver_data *drvdata)
{
	if (!drvdata || drvdata->state == SLEEP_STATE)
		return;

//	dprintk("operation mode is %s\n");
	
			drvdata->state = SLEEP_STATE;
}

#if 0
static void lm3530_wakeup(struct lm3530_driver_data *drvdata)
{
	if (!drvdata || drvdata->state == NORMAL_STATE)
		return;

//	dprintk("operation mode is %s\n");

	if (drvdata->state == POWEROFF_STATE) {
		lm3530_go_opmode(drvdata);
	}
	else if(drvdata->state == SLEEP_STATE) {
		//lm3530_write(drvdata->client, 0x03, drvdata->intensity);
		drvdata->state = NORMAL_STATE;
		}
}


static void lm3530_poweron(struct lm3530_driver_data *drvdata)
{
	if (!drvdata || drvdata->state != POWEROFF_STATE)
		return;
	
	dprintk("POWER ON \n");

	lm3530_init(drvdata);
	//lm3530_write(drvdata->client, 0x03, drvdata->intensity);
}
#endif

static void lm3530_poweroff(struct lm3530_driver_data *drvdata)
{
	if (!drvdata || drvdata->state == POWEROFF_STATE)
		return;

	dprintk("POWER OFF \n");
		
	gpio_tlmm_config(GPIO_CFG(drvdata->gpio, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),GPIO_CFG_ENABLE);
	gpio_direction_output(drvdata->gpio, 1);
	gpio_set_value(drvdata->gpio, 0);
	
	mdelay(6);

	drvdata->state = POWEROFF_STATE;
}

static int lm3530_cal_brightness(struct lm3530_driver_data *drvdata, int level)
{
	int cal_level= 0;
#if defined(LGE_MODEL_C729)
	if(lge_get_hw_rev() == LGE_REV_C || lge_get_hw_rev() == LGE_REV_D)
	{
		if (level == 0)
		{	
			cal_level = 0x00;
		}
		else if (level <= 20)
		{
			cal_level = 0x40;
		}
		else if (level <= 30)
		{
			cal_level = 0x48;
		}
		else if(level > 30 && level <= 102)
		{
			cal_level = (level - 30) * (0x60 - 0x48) / (102 - 30) + 0x48;
		}
		else if(level >102)
		{
			cal_level = (level - 102) * (drvdata->max_intensity - 0x60) / (LED_FULL - 102) + 0x60;
		}
	}
	else
	{
		if (level == 0)
		{
			cal_level = 0x00;
		}
		else if (level <= 20)
		{
			cal_level = 0x05;
		}	
		else if (level <= 30)
		{
			cal_level = 0x09;
		}
		else if(level > 30 && level <= 102)
		{
			cal_level = (level - 30) * (0x2a - 0x09) / (102 - 30) + 0x09;
		}
		else if(level >102)
		{
			cal_level = (level - 102) * (drvdata->max_intensity - 0x2a) / (LED_FULL - 102) + 0x2a;
		}		
	}	
#else

#if defined(LGE_MODEL_C800_REV_EVB) || defined(LGE_MODEL_C800_REV_A)
	if (level == 0)
	{
		cal_level = 0x00;
	}
	else if (level <= 20)
	{
		cal_level = 0x40;
	}
	else if (level <= 30)
	{
		cal_level = 0x48;
	}
	else if(level > 30 && level <= 102)
	{
		cal_level = (level - 30) * (0x60 - 0x48) / (102 - 30) + 0x48;
	}
	else if(level >102)
	{
		cal_level = (level - 102) * (drvdata->max_intensity - 0x60) / (LED_FULL - 102) + 0x60;
	}
#else
	if (level == 0)
	{
		cal_level = 0x00;	
	}
	else if (level <= 20)
	{
		cal_level = 0x05;
	}	
	else if (level <= 30)
	{
		cal_level = 0x09;
	}
	else if(level > 30 && level <= 102)
	{
		cal_level = (level - 30) * (0x2a - 0x09) / (102 - 30) + 0x09;
	}
	else if(level >102)
	{
		cal_level = (level - 102) * (drvdata->max_intensity - 0x2a) / (LED_FULL - 102) + 0x2a;
	}
#endif	
	
#endif
	printk("%s: Brightness level: %d , Cal Value: 0x%x \n",__func__, level, cal_level);
	return cal_level;
}

static int lm3530_send_intensity(struct lm3530_driver_data *drvdata, int next)
{
#if 0
	int ret = -EPERM;

	if(next)
		ret = pmic_set_led_intensity(LED_KEYPAD, KYPD_LED_DEFAULT);
	else
		ret = pmic_set_led_intensity(LED_KEYPAD, 0);
	
	if (ret)
		printk(KERN_WARNING "%s: can't set lcd backlight!\n",
					__func__);
#endif

	if(next > drvdata->max_intensity)
		next = drvdata->max_intensity;
	
	if(next < LCD_LED_MIN)
		next = LCD_LED_MIN;
	dprintk("next current is %d\n", next);

#if 0
	if(drvdata->state == NORMAL_STATE && drvdata->intensity != next)
	{
		if(!atomic_read(&lcd_event_handled) || atomic_read(&backlight_event_handle))
		{
			next_bl = next;
			bl_i2c_client = drvdata->client ;
			atomic_set(&backlight_event_handle,1);	
			mod_timer(&timerbl_bl, jiffies + msecs_to_jiffies(100));		
		}		
		else
		{
//			printk("##LMH_TEST   SET_BL <normal> current_bl : %d / next=%d\n", current_bl,next);		
			lm3530_write(drvdata->client, 0xA0, next);
		}	
	}
#else
	if(drvdata->state == NORMAL_STATE && drvdata->intensity != next)
		lm3530_write(drvdata->client, 0xA0, next);
#endif	
	drvdata->bd->props.brightness =

	drvdata->intensity = next;

	return 0;
}

static int lm3530_get_intensity(struct lm3530_driver_data *drvdata)
{
	return drvdata->intensity;
}


#ifdef CONFIG_PM
#ifdef CONFIG_HAS_EARLYSUSPEND
static void lm3530_early_suspend(struct early_suspend * h)
{	
	struct lm3530_driver_data *drvdata = container_of(h, struct lm3530_driver_data,
						    early_suspend);

	dprintk("start\n");
	lm3530_write(drvdata->client, 0x80, 0x01);  /* HW block disable */
	lm3530_sleep(drvdata);
	lm3530_poweroff(drvdata);
	return;
}

static void lm3530_late_resume(struct early_suspend * h)
{	
	struct lm3530_driver_data *drvdata = container_of(h, struct lm3530_driver_data,
						    early_suspend);

	dprintk("start\n");
#ifdef CONFIG_BACKLIGHT_LEDS_CLASS
	leds_brightness_get();
#endif
	lm3530_init(drvdata);

	return;
}
#else
static int lm3530_suspend(struct i2c_client *i2c_dev, pm_message_t state)
{
	struct lm3530_driver_data *drvdata = i2c_get_clientdata(i2c_dev);
        dprintk("start\n");
	lm3530_write(drvdata->client, 0x80, 0x01);  /* HW block disable */
	lm3530_sleep(drvdata);
	lm3530_poweroff(drvdata);
	return 0;
}

static int lm3530_resume(struct i2c_client *i2c_dev)
{
	struct lm3530_driver_data *drvdata = i2c_get_clientdata(i2c_dev);
         dprintk("start\n");
        lm3530_init(drvdata);

	return 0;
}
#endif	/* CONFIG_HAS_EARLYSUSPEND */
#else
#define lm3530_suspend	NULL
#define lm3530_resume	NULL
#endif	/* CONFIG_PM */


static int lm3530_set_brightness(struct backlight_device *bd)
{
	struct lm3530_driver_data *drvdata = dev_get_drvdata(bd->dev.parent);

#if 0
	int ret = -EPERM;

	if(bd->props.brightness)
		ret = pmic_set_led_intensity(LED_KEYPAD, KYPD_LED_DEFAULT);
	else
		ret = pmic_set_led_intensity(LED_KEYPAD, 0);
	
	if (ret)
		printk(KERN_WARNING "%s: can't set lcd backlight!\n",
					__func__);
#endif
	
	return lm3530_send_intensity(drvdata, bd->props.brightness);
}

static int lm3530_get_brightness(struct backlight_device *bd)
{
	struct lm3530_driver_data *drvdata = dev_get_drvdata(bd->dev.parent);
	return lm3530_get_intensity(drvdata);
}

static struct backlight_ops lm3530_ops = {
	.get_brightness = lm3530_get_brightness,
	.update_status  = lm3530_set_brightness,
};


#ifdef CONFIG_BACKLIGHT_LEDS_CLASS
static void leds_brightness_set(struct led_classdev *led_cdev, enum led_brightness value)
{
	struct lm3530_driver_data *drvdata = dev_get_drvdata(led_cdev->dev->parent);
	int brightness;
	int next = 0;    /*jihye.ahn 10-10-12 WBT fix TD157992 */

	if (!drvdata) {
		dprintk("Error getting drvier data\n");
		return;
	}

	brightness = lm3530_get_intensity(drvdata);

	if (value > 20)
	{
		saved_backlight_level = value;
	}
	next = lm3530_cal_brightness(drvdata, value);
		
	dprintk("android input brightness =%d]\n", value);

	if (brightness != next) {
		dprintk("brightness[current=%d, next=%d]\n", brightness, next);
		lm3530_send_intensity(drvdata, next);
	}
}

static struct led_classdev lm3530_led_dev = {
	.name = LEDS_BACKLIGHT_NAME,
	.brightness_set = leds_brightness_set,
};

static void leds_brightness_get()
{
	saved_backlight_level = lm3530_led_dev.brightness;
	printk("%s: %d\n",__func__, lm3530_led_dev.brightness);
}
#endif

static int lm3530_probe(struct i2c_client *i2c_dev, const struct i2c_device_id *i2c_dev_id)
{


	struct backlight_platform_data *pdata;
	struct lm3530_driver_data *drvdata;
	struct backlight_device *bd;
	struct backlight_properties props;    // temp kideok.kim 

	int err;


	printk("%s start, client addr=0x%x\n", __func__, i2c_dev->addr);

	drvdata = kzalloc(sizeof(struct lm3530_driver_data), GFP_KERNEL);
	if (!drvdata) {
		dev_err(&i2c_dev->dev, "failed to allocate memory\n");
		return -ENOMEM;
	}

	pdata = i2c_dev->dev.platform_data;
	/* jihye.ahn 10-10-12  WBT fix TD157993
	if(pdata && pdata->platform_init)
		pdata->platform_init();
	*/
	drvdata->client = i2c_dev;
	drvdata->gpio = pdata->gpio;
	drvdata->init_on_boot = pdata->init_on_boot;
	drvdata->max_intensity = LCD_LED_MAX;
	if(pdata->max_current > 0)
		drvdata->max_intensity = pdata->max_current;
	drvdata->intensity = LCD_LED_MIN;
	drvdata->state = UNINIT_STATE;

	if(drvdata->gpio && gpio_request(drvdata->gpio, "lm3530 reset") != 0) {
		dprintk("Error while requesting gpio %d\n", drvdata->gpio);
		kfree(drvdata);
		return -ENODEV;
	}

	memset(&props, 0, sizeof(struct backlight_properties));
	props.max_brightness = LCD_LED_MAX;

	bd = backlight_device_register("lm3530-bl", &i2c_dev->dev, NULL, &lm3530_ops, &props);

	if (bd == NULL) {
		dprintk("entering lm3530_bl probe function error \n");
		if(gpio_is_valid(drvdata->gpio))
			gpio_free(drvdata->gpio);
		kfree(drvdata);
		return -1;
	}
	bd->props.power = FB_BLANK_UNBLANK;
	bd->props.brightness = drvdata->intensity;
	bd->props.max_brightness = drvdata->max_intensity;
	drvdata->bd = bd;
/* jihye.ahn 10.08.11
	Register backlight driver to LED class
*/

#if 1
atomic_set(&lcd_event_handled,1);
atomic_set(&lcd_bootup_handle,1);
#else

setup_timer(&timerbl_bl, bl_timer, (unsigned long)i2c_dev);
setup_timer(&timerbl_lcd, lcd_timer, (unsigned long)i2c_dev);
atomic_set(&lcd_event_handled,1);
atomic_set(&backlight_event_handle,0);
atomic_set(&lcd_bootup_handle,1);
#endif

#ifdef CONFIG_BACKLIGHT_LEDS_CLASS
        if(led_classdev_register(&i2c_dev->dev, &lm3530_led_dev) == 0) {
                dprintk("Registering led class dev successfully.\n");
                drvdata->led = &lm3530_led_dev;
        }
#endif

	i2c_set_clientdata(i2c_dev, drvdata);
#if 0
	gpio_set_value(25, 1);
	udelay(5);
	gpio_set_value(25, 0);
	udelay(5);
	gpio_set_value(25, 1);
	udelay(5);
#endif
	lm3530_init(drvdata);
	lm3530_send_intensity(drvdata, DEFAULT_BRIGHTNESS);

#ifdef CONFIG_HAS_EARLYSUSPEND
	drvdata->early_suspend.suspend = lm3530_early_suspend;
	drvdata->early_suspend.resume = lm3530_late_resume;
	drvdata->early_suspend.level = EARLY_SUSPEND_LEVEL_DISABLE_FB - 3;
	register_early_suspend(&drvdata->early_suspend);
#endif

	dprintk("+++++++LM3530 done\n");
	return 0;

// probe_free_exit:
	kfree(drvdata);
	i2c_set_clientdata(i2c_dev, NULL);	
	return err;	
}

static int __devexit lm3530_remove(struct i2c_client *i2c_dev)
{
	struct lm3530_driver_data *drvdata = i2c_get_clientdata(i2c_dev);

	lm3530_send_intensity(drvdata, 0);

	backlight_device_unregister(drvdata->bd);
	led_classdev_unregister(drvdata->led);
	i2c_set_clientdata(i2c_dev, NULL);
	if (gpio_is_valid(drvdata->gpio))
		gpio_free(drvdata->gpio);
	kfree(drvdata);

	return 0;
}

static const struct i2c_device_id lm3530_ids[] = {
	{ MODULE_NAME, 0 },
	{ },		
};
MODULE_DEVICE_TABLE(i2c, lm3530_ids); /*jihye.ahn  10.08.11 fixed typo*/

static struct i2c_driver lm3530_driver = {
	.driver.name	= MODULE_NAME,
	.id_table	= lm3530_ids,		
	.probe 		= lm3530_probe,
	.remove 	= lm3530_remove,
#ifndef CONFIG_HAS_EARLYSUSPEND
	.suspend 	= lm3530_suspend,
	.resume 	= lm3530_resume,
#endif
};

static int __init lm3530_drv_init(void)
{
	int rc = i2c_add_driver(&lm3530_driver);
	pr_notice("%s: i2c_add_driver: rc = %d\n", __func__, rc);
	return rc;
}

static void __exit lm3530_drv_exit(void)
{
	i2c_del_driver(&lm3530_driver);
}
module_init(lm3530_drv_init);
module_exit(lm3530_drv_exit);


MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("LM3530 driver");
MODULE_VERSION("1.0");
MODULE_ALIAS("platform:LM3530");
