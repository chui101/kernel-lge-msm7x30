/* Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */

#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <mach/vreg.h>
#include <mach/rpc_server_handset.h>
#include <mach/board.h>
#include <mach/board_lge.h>

#ifdef CONFIG_MACH_MSM8X55_UNIVA_Q
#include "board-univaq.h"
#include <linux/gpio_keys.h>
#include <linux/gpio_event.h>

#endif

#ifdef CONFIG_MACH_MSM8X55_UNIVA_Q
#include <mach/vreg.h>
#include <mach/board_lge.h>
#endif



/* atcmd virtual device : AT%FKPD */
static unsigned int atcmd_virtual_keycode[ATCMD_VIRTUAL_KEYPAD_ROW][ATCMD_VIRTUAL_KEYPAD_COL] = {
	{KEY_1,   KEY_9,           KEY_A,   KEY_I,   KEY_Q,   KEY_Y,         KEY_UP,          KEY_F15},
	{KEY_2,   KEY_0,           KEY_B,   KEY_J,   KEY_R,   KEY_Z,         KEY_QUESTION,    KEY_F16},
	{KEY_3,   KEY_BACK,        KEY_C,   KEY_K,   KEY_S,   KEY_MAIL,      KEY_RIGHT,       KEY_SPACE},
	{KEY_4,   KEY_SEARCH,      KEY_D,   KEY_L,   KEY_T,   KEY_SEND,      KEY_LANGUAGE,    KEY_F23},
	{KEY_5,   KEY_HOME,        KEY_E,   KEY_M,   KEY_U,   KEY_END,       KEY_LEFTALT,     KEY_DOT},
	{KEY_6,   KEY_MENU,        KEY_F,   KEY_N,   KEY_V,   KEY_ENTER,     KEY_RIGHTALT,    KEY_NUMERIC_STAR},
	{KEY_7,   KEY_VOLUMEUP,    KEY_G,   KEY_O,   KEY_W,   KEY_F14,       KEY_LEFTSHIFT,   KEY_NUMERIC_POUND},
	{KEY_8,   KEY_VOLUMEDOWN,  KEY_H,   KEY_P,   KEY_X,   KEY_BACKSPACE, KEY_RIGHTSHIFT,  KEY_COMMA},
};

static struct atcmd_virtual_platform_data atcmd_virtual_pdata = {
	.keypad_row = ATCMD_VIRTUAL_KEYPAD_ROW,
	.keypad_col = ATCMD_VIRTUAL_KEYPAD_COL,
	.keycode = (unsigned int *)atcmd_virtual_keycode,
};

static struct platform_device atcmd_virtual_device = {
	.name = "atcmd_virtual_kbd",
	.id = -1,
	.dev = {
		.platform_data = &atcmd_virtual_pdata,
	},
};

/* head set device */
static struct msm_handset_platform_data hs_platform_data = {
	.hs_name = "7k_handset",
	.pwr_key_delay_ms = 500, /* 0 will disable end key */
};

static struct platform_device hs_device = {
	.name   = "msm-handset",
	.id     = -1,
	.dev    = {
		.platform_data = &hs_platform_data,
	},
};

static int main_ts_set_vreg(unsigned char onoff)
{
	struct vreg *vreg_touch_avdd;	// analog vdd
	struct vreg *vreg_touch_vdd;	// digital vdd
	int rc;

	printk("[MainTouch] %s() onoff:%d\n",__FUNCTION__, onoff);

	vreg_touch_avdd = vreg_get(0, "wlan");
	vreg_touch_vdd = vreg_get(0, "xo_out");

	if(IS_ERR(vreg_touch_avdd) || IS_ERR(vreg_touch_vdd)) {
		printk("[MainTouch] vreg_get fail : touch\n");
		return -1;
	}

	if (onoff) {
		rc = vreg_set_level(vreg_touch_avdd, 3000);
		if (rc != 0) {
			printk("[MainTouch] avdd vreg_set_level failed\n");
			return -1;
		}

		rc = vreg_enable(vreg_touch_avdd);
		if (rc != 0) {
			printk("[MainTouch] avdd vreg_enable failed\n");
		}


		rc = vreg_set_level(vreg_touch_vdd, 2850);
		if (rc != 0) {
			printk("[MainTouch] vdd vreg_set_level failed\n");
			return -1;
		}

		rc = vreg_enable(vreg_touch_vdd);

		if (rc != 0) {
			printk("[MainTouch] vdd vreg_enable failed\n");
		}

	}
	else {
	
		rc = vreg_disable(vreg_touch_avdd);
		if (rc != 0) {
			printk("[MainTouch] avdd vreg_disable failed\n");
		}

		rc = vreg_disable(vreg_touch_vdd);
		if (rc != 0) {
			printk("[MainTouch] vdd vreg_disable failed\n");
		}
	}

	return 0;
}

#ifdef CONFIG_TOUCHSCREEN_QT602240
static struct qt602240_platform_data main_ts_pdata = {
	.x_line		= 19,
	.y_line		= 11,
	.x_size		= 800,
	.y_size		= 480,
	.blen		= 32,
	.threshold	= 40,
	.voltage	= 2700000,
	.orient		= 7,
	.power		= main_ts_set_vreg,
	.gpio_int	= MAIN_TS_GPIO_INT,
	.irq		= MAIN_TS_GPIO_IRQ,
	.scl		= MAIN_TS_GPIO_I2C_SCL,
	.sda		= MAIN_TS_GPIO_I2C_SDA,
};
#else   // UNIVA_Q uses here
static struct touch_platform_data main_ts_pdata = {
	.ts_x_min   = MAIN_TS_X_MIN,
	.ts_x_max   = MAIN_TS_X_MAX,
	.ts_x_scrn_max = MAIN_TS_X_SCRN_MAX,
	.ts_y_min   = MAIN_TS_Y_MIN,
	.ts_y_max   = MAIN_TS_Y_MAX,
	.ts_y_start = MAIN_TS_Y_START,
	.ts_y_scrn_max = MAIN_TS_Y_SCRN_MAX,
	.power      = main_ts_set_vreg,
	.gpio_int   = MAIN_TS_GPIO_INT,
	.irq        = MAIN_TS_GPIO_IRQ,
	.scl        = MAIN_TS_GPIO_I2C_SCL,
	.sda        = MAIN_TS_GPIO_I2C_SDA,
 	.hw_i2c     = 1,
 	.ce	    = MAIN_TS_GPIO_RESET,
 	.reset	    = MAIN_TS_GPIO_RESET,
};
#endif

#ifndef CONFIG_MACH_MSM8X55_UNIVA_Q

static struct gpio_i2c_pin touch_panel_i2c_pin[] = {
	[0] = {
		.sda_pin    = MAIN_TS_GPIO_I2C_SDA,
		.scl_pin    = MAIN_TS_GPIO_I2C_SCL,
		.reset_pin  = MAIN_TS_GPIO_RESET,
		.irq_pin    = MAIN_TS_GPIO_INT,
	},
};


static struct i2c_gpio_platform_data touch_panel_i2c_pdata = {
	.sda_is_open_drain = 0,
	.scl_is_open_drain = 0,
	.udelay = 2,
};



static struct platform_device touch_panel_i2c_device = {
	.name = "i2c-gpio",
	.dev.platform_data = &touch_panel_i2c_pdata,
};
#endif


static struct i2c_board_info touch_panel_i2c_bdinfo[] = {
	[0] = {
		I2C_BOARD_INFO("qt602240_ts", MXT224_TS_I2C_SLAVE_ADDR),
		.type = "qt602240_ts",
		.platform_data = &main_ts_pdata,
		.irq = MAIN_TS_GPIO_IRQ,
	},
};




#ifdef CONFIG_MACH_MSM8X55_UNIVA_Q

/* Hall IC input */

static struct gpio_event_direct_entry flip_slide_switch_map[] = {
	{ 18,		   SW_LID		   }, 
};

static int flip_gpio_slide_power(
		const struct gpio_event_platform_data *pdata, bool on)
{


#ifdef CONFIG_MACH_MSM8X55_UNIVA_Q

	if(lge_get_hw_rev() <= LGE_REV_C)
	{
		struct vreg *xo_out_vreg =	vreg_get(0, "xo_out");; 		

		if(on)
		{
//			vreg_set_level(xo_out_vreg, 2850/*2600*/);
			vreg_enable(xo_out_vreg);
			printk("[gpio_slide_power] %s(), HW REV : %d xo_out onoff:%d\n",__FUNCTION__,lge_get_hw_rev(), on);

		}
		else
		{
			vreg_disable(xo_out_vreg);
			printk("[gpio_slide_power] %s(), HW REV : %d xo_out onoff:%d\n",__FUNCTION__,lge_get_hw_rev(), on);
		}
	}
	else
	{

		printk("[gpio_slide_power] %s(), HW REV : %d Do nothing . \n",__FUNCTION__,lge_get_hw_rev());

	}
	return 1;

#else	
	return 0;
#endif

}

#if 1
#if 0
static int flip_gpio_slide_input_func(struct input_dev *input_dev,
		struct gpio_event_info *info, void **data, int func)
{

	if (func == GPIO_EVENT_FUNC_INIT)
		gpio_tlmm_config(GPIO_CFG(18, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP,
					GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	return gpio_event_input_func(input_dev, info, data, func);
}
#else
static int flip_gpio_slide_input_func(struct gpio_event_input_devs *input_devs,
		struct gpio_event_info *info, void **data, int func)
{

	if (func == GPIO_EVENT_FUNC_INIT)
		gpio_tlmm_config(GPIO_CFG(18, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP,
					GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	return gpio_event_input_func(input_devs, info, data, func);
}

#endif
static struct gpio_event_input_info flip_slide_switch_info = {
	.info.func = flip_gpio_slide_input_func,
	.debounce_time.tv64 = (80 * NSEC_PER_MSEC),
	.flags = GPIOEDF_ACTIVE_HIGH,
	.type = EV_SW,
	.keymap = flip_slide_switch_map,
	.keymap_size = ARRAY_SIZE(flip_slide_switch_map)
};

#else
static struct gpio_event_input_info flip_slide_switch_info = {
	.info.func = gpio_event_input_func,
	.debounce_time.tv64 = 0,
	.flags = 0,
	.type = EV_SW,
	.keymap = flip_slide_switch_map,
	.keymap_size = ARRAY_SIZE(flip_slide_switch_map)
};

#endif

static struct gpio_event_info *flip_gpio_slide_info[] = {
	&flip_slide_switch_info.info,
};

static struct gpio_event_platform_data flip_gpio_slide_data = {
	.name = "gpio-slide-detect",
	.info = flip_gpio_slide_info,
	.info_count = ARRAY_SIZE(flip_gpio_slide_info),
	.power = flip_gpio_slide_power,
};

static struct platform_device flip_gpio_slide_device = {
	.name = "gpio-event",
	.id = 0,
	.dev        = {
		.platform_data  = &flip_gpio_slide_data,
	},
};




/* Direct GPIO Side Keys */

#if 1 //defined(CONFIG_LGE_MODEL_C800)
static struct gpio_keys_button univaq_sidekey_buttons[] = {
	{	
		.code		= KEY_VOLUMEUP,
		.gpio		= 129,
		.active_low	= 1,
		.desc		= "volume_up_sidekey",
		.wakeup		= 1,
		.type       = EV_KEY,
	},
	{	
		.code		= KEY_VOLUMEDOWN,
		.gpio		= 128,
		.active_low	= 1,
		.desc		= "volume_down_sidekey",
		.wakeup		= 1,
		.type       = EV_KEY,
	},
};
#else
static struct gpio_keys_button univaq_sidekey_buttons[] = {
	{	
		.code		= KEY_VOLUMEUP,
		.gpio		= 180,
		.active_low	= 1,
		.desc		= "volume_up_sidekey",
		.wakeup		= 1,
		.type       = EV_KEY,
	},
	{	
		.code		= KEY_VOLUMEDOWN,
		.gpio		= 147,
		.active_low	= 1,
		.desc		= "volume_down_sidekey",
		.wakeup		= 1,
		.type       = EV_KEY,
	},
};
#endif

static struct gpio_keys_platform_data univaq_sidekey_button_data = {
	.buttons	= univaq_sidekey_buttons,
	.nbuttons	= ARRAY_SIZE(univaq_sidekey_buttons),
};


static struct platform_device univaq_gpio_direct_sidekey_device = {
	.name = "gpio-sidekey-device",
	.id = -1,
	.dev		= {
		.platform_data =  &univaq_sidekey_button_data,
	},
};


#endif


/* accelerometer */
static int accel_power(unsigned char onoff)
{
	int ret = 0;
	struct vreg *xo_out_vreg = vreg_get(0, "xo_out");
	struct vreg *gp7_vreg = vreg_get(0, "gp7");

	if (onoff) {
		vreg_set_level(gp7_vreg, 1800);
		vreg_enable(gp7_vreg);
		vreg_set_level(xo_out_vreg, 2850/*2600*/);
		vreg_enable(xo_out_vreg);
	} else {
		vreg_disable(gp7_vreg);
		vreg_disable(xo_out_vreg);
	}

	return ret;
}

struct acceleration_platform_data bma250 = {
	.irq_num	= ACCEL_GPIO_INT,
	.power = accel_power,
};

static struct gpio_i2c_pin accel_i2c_pin[] = {
	[0] = {
		.sda_pin	= ACCEL_GPIO_I2C_SDA,
		.scl_pin	= ACCEL_GPIO_I2C_SCL,
		.reset_pin	= 0,
		.irq_pin	= ACCEL_GPIO_INT,
	},
};

static struct i2c_gpio_platform_data accel_i2c_pdata = {
	.sda_is_open_drain = 0,
	.scl_is_open_drain = 0,
	.udelay = 2,
};

static struct platform_device accel_i2c_device = {
	.name = "i2c-gpio",
	.dev.platform_data = &accel_i2c_pdata,
};

static struct i2c_board_info accel_i2c_bdinfo[] = {
	[0] = {
		I2C_BOARD_INFO("bma250", ACCEL_I2C_ADDRESS),
		.type = "bma250",
		.platform_data = &bma250,
	},
};

static void __init victor_init_i2c_acceleration(int bus_num)
{
	accel_i2c_device.id = 11;//bus_num;

	init_gpio_i2c_pin(&accel_i2c_pdata, accel_i2c_pin[0], &accel_i2c_bdinfo[0]);

	i2c_register_board_info(11/*bus_num*/, &accel_i2c_bdinfo[0], 1);

	platform_device_register(&accel_i2c_device);
}

/* ecompass */
static int ecom_power_set(unsigned char onoff)
{
	int ret = 0;
	struct vreg *xo_out_vreg = vreg_get(0, "xo_out");
	struct vreg *gp7_vreg = vreg_get(0, "gp7");

	if (onoff) {
		vreg_set_level(gp7_vreg, 1800);
		vreg_enable(gp7_vreg);
		vreg_set_level(xo_out_vreg, 2850/*2600*/);
		vreg_enable(xo_out_vreg);
	} else {
		vreg_disable(gp7_vreg);
		vreg_disable(xo_out_vreg);
	}

	return ret;
}

static struct ecom_platform_data ecom_pdata = {
	.pin_int        = ECOM_GPIO_INT,
	.pin_rst		= 0,
	.power          = ecom_power_set,
};

/* proximity */
static int prox_power_set(unsigned char onoff)
{
	int ret = 0;
	struct vreg *wlan2_vreg = vreg_get(0, "wlan2");
	struct vreg *gp7_vreg = vreg_get(0, "gp7");

	if (onoff) {
		vreg_set_level(gp7_vreg, 1800);
		vreg_enable(gp7_vreg);
		vreg_set_level(wlan2_vreg, 2600);
		vreg_enable(wlan2_vreg);
	} else {
		vreg_disable(gp7_vreg);
		vreg_disable(wlan2_vreg);
	}

	return ret;
}

static struct proximity_platform_data proxi_pdata = {
	.irq_num	= PROXI_GPIO_DOUT,
	.power		= prox_power_set,
	.methods		= 1,
	.operation_mode		= 1,
	.debounce	 = 0,
	.cycle = 5,
};

static struct i2c_board_info prox_ecom_i2c_bdinfo[] = {
	[0] = {
		I2C_BOARD_INFO("proximity_gp2ap", PROXI_I2C_ADDRESS),
		.type = "proximity_gp2ap",
		.platform_data = &proxi_pdata,
	},
	[1] = {
		I2C_BOARD_INFO("ami304_sensor", ECOM_I2C_ADDRESS),
		.type = "ami304_sensor",
		.platform_data = &ecom_pdata,
	},
};

static struct gpio_i2c_pin proxi_ecom_i2c_pin[] = {
	[0] = {
		.sda_pin	= PROXI_GPIO_I2C_SDA,
		.scl_pin	= PROXI_GPIO_I2C_SCL,
		.reset_pin	= 0,
		.irq_pin	= PROXI_GPIO_DOUT,
	},
	[1] = {
		.sda_pin	= ECOM_GPIO_I2C_SDA,
		.scl_pin	= ECOM_GPIO_I2C_SCL,
		.reset_pin	= 0,
		.irq_pin	= ECOM_GPIO_INT,
	},
};

static struct i2c_gpio_platform_data proxi_ecom_i2c_pdata = {
	.sda_is_open_drain = 0,
	.scl_is_open_drain = 0,
	.udelay = 2,
};

static struct platform_device proxi_ecom_i2c_device = {
        .name = "i2c-gpio",
        .dev.platform_data = &proxi_ecom_i2c_pdata,
};

static void __init victor_init_i2c_prox_ecom(int bus_num)
{
	proxi_ecom_i2c_device.id = 12;//bus_num;

	init_gpio_i2c_pin(&proxi_ecom_i2c_pdata, proxi_ecom_i2c_pin[0], &prox_ecom_i2c_bdinfo[0]);
	init_gpio_i2c_pin(&proxi_ecom_i2c_pdata, proxi_ecom_i2c_pin[1], &prox_ecom_i2c_bdinfo[1]);

	i2c_register_board_info(12/*bus_num*/, &prox_ecom_i2c_bdinfo[0], 2);
	platform_device_register(&proxi_ecom_i2c_device);
}

static void __init touch_panel(int bus_num)
{

	main_ts_set_vreg(1);
	i2c_register_board_info(0, &touch_panel_i2c_bdinfo[0], 1);

}

/* input platform device */
static struct platform_device *victor_input_devices[] __initdata = {
	&hs_device,

#ifdef CONFIG_MACH_MSM8X55_UNIVA_Q
	&flip_gpio_slide_device,
	&univaq_gpio_direct_sidekey_device,
#endif

	&atcmd_virtual_device,
};

/* common function */
void __init lge_add_input_devices(void)
{
    platform_add_devices(victor_input_devices, ARRAY_SIZE(victor_input_devices));


	lge_add_gpio_i2c_device(touch_panel);


	lge_add_gpio_i2c_device(victor_init_i2c_acceleration);

    lge_add_gpio_i2c_device(victor_init_i2c_prox_ecom);

#if defined(CONFIG_LGE_SU610_NFC)||defined(CONFIG_LGE_KU6100_NFC)
	lge_add_gpio_i2c_device(victor_init_i2c_nfc);	//seokmin for NFC
//	gpio_tlmm_config(GPIO_CFG(NFC_GPIO_SW_UPGRADE, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), GPIO_CFG_ENABLE);	//seokmin for NFC
#endif

}
