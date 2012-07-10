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
#include <linux/i2c.h>
#include <linux/mfd/pmic8058.h>
#include <linux/input/pmic8058-keypad.h>
#include <linux/pwm.h>
#include <linux/pmic8058-pwm.h>
#include <linux/leds-pmic8058.h>
#include <asm/mach-types.h>
#include "../pm.h"
#include "board-univaq.h"


static int pm8058_gpios_init(void)
{
	int rc;
#ifdef CONFIG_MMC_MSM_CARD_HW_DETECTION
	struct pm8058_gpio sdcc_det = {
		.direction      = PM_GPIO_DIR_IN,
		.pull           = PM_GPIO_PULL_UP_1P5,
		.vin_sel        = 2,
		.function       = PM_GPIO_FUNC_NORMAL,
		.inv_int_pol    = 0,
	};
#endif
	struct pm8058_gpio sdc4_en = {
		.direction      = PM_GPIO_DIR_OUT,
		.pull           = PM_GPIO_PULL_UP_1P5,
		.vin_sel        = 2,
		.function       = PM_GPIO_FUNC_NORMAL,
		.inv_int_pol    = 0,
	};

#ifdef CONFIG_MMC_MSM_CARD_HW_DETECTION
    if (machine_is_msm7x30_fluid())
		sdcc_det.inv_int_pol = 1;

	rc = pm8058_gpio_config(PMIC_GPIO_SD_DET - 1, &sdcc_det);
	if (rc) {
		pr_err("%s PMIC_GPIO_SD_DET config failed\n", __func__);
		return rc;
	}
#endif

	if (machine_is_msm7x30_fluid()) {
		rc = pm8058_gpio_config(PMIC_GPIO_SDC4_EN, &sdc4_en);
		if (rc) {
			pr_err("%s PMIC_GPIO_SDC4_EN config failed\n",
				   __func__);
			return rc;
		}
		gpio_set_value_cansleep(
			PM8058_GPIO_PM_TO_SYS(PMIC_GPIO_SDC4_EN), 0);
	}

	return 0;
}

static int pm8058_pwm_config(struct pwm_device *pwm, int ch, int on)
{
	struct pm8058_gpio pwm_gpio_config = {
		.direction      = PM_GPIO_DIR_OUT,
		.output_buffer  = PM_GPIO_OUT_BUF_CMOS,
		.output_value   = 0,
		.pull           = PM_GPIO_PULL_NO,
		.vin_sel        = PM_GPIO_VIN_S3,
		.out_strength   = PM_GPIO_STRENGTH_HIGH,
		.function       = PM_GPIO_FUNC_2,
	};
	int	rc = -EINVAL;
	int	id, mode, max_mA;

	id = mode = max_mA = 0;
	switch (ch) {
	case 0:
	case 1:
	case 2:
		if (on) {
			id = 24 + ch;
			rc = pm8058_gpio_config(id - 1, &pwm_gpio_config);
			if (rc)
				pr_err("%s: pm8058_gpio_config(%d): rc=%d\n",
				       __func__, id, rc);
		}
		break;

	case 3:
		id = PM_PWM_LED_KPD;
		mode = PM_PWM_CONF_DTEST3;
		max_mA = 200;
		break;

	case 4:
		id = PM_PWM_LED_0;
		mode = PM_PWM_CONF_PWM1;
		max_mA = 40;
		break;

	case 5:
		id = PM_PWM_LED_2;
		mode = PM_PWM_CONF_PWM2;
		max_mA = 40;
		break;

	case 6:
		id = PM_PWM_LED_FLASH;
		mode = PM_PWM_CONF_DTEST3;
		max_mA = 200;
		break;

	default:
		break;
	}

	if (ch >= 3 && ch <= 6) {
		if (!on) {
			mode = PM_PWM_CONF_NONE;
			max_mA = 0;
		}
		rc = pm8058_pwm_config_led(pwm, id, mode, max_mA);
		if (rc)
			pr_err("%s: pm8058_pwm_config_led(ch=%d): rc=%d\n",
			       __func__, ch, rc);
	}

	return rc;
}

static int pm8058_pwm_enable(struct pwm_device *pwm, int ch, int on)
{
	int	rc;

	switch (ch) {
	case 7:
		rc = pm8058_pwm_set_dtest(pwm, on);
		if (rc)
			pr_err("%s: pwm_set_dtest(%d): rc=%d\n",
			       __func__, on, rc);
		break;
	default:
		rc = -EINVAL;
		break;
	}
	return rc;
}

#ifdef CONFIG_MACH_MSM8X55_UNIVA_Q

static const unsigned int univaq_pm_keymap[] = {
#if defined(LGE_MODEL_C800_REV_EVB)
/// KEY(DRV,SNS, keys) on PMIC Keypad.
	KEY(0, 0, KEY_OPTION),
	KEY(0, 1, KEY_Q),
	KEY(0, 2, KEY_W),
	KEY(0, 3, KEY_E),
	KEY(0, 4, KEY_R),
	KEY(0, 5, KEY_T),

	KEY(1, 0, KEY_Y),
	KEY(1, 1, KEY_U),
	KEY(1, 2, KEY_I),
	KEY(1, 3, KEY_O),
	KEY(1, 4, KEY_P),
	KEY(1, 5, KEY_DELETE),

	KEY(2, 0, KEY_LEFTCTRL),
	KEY(2, 1, KEY_A),
	KEY(2, 2, KEY_S),
	KEY(2, 3, KEY_D),
	KEY(2, 4, KEY_F),
	KEY(2, 5, KEY_G),

	KEY(3, 0, KEY_H),
	KEY(3, 1, KEY_J),
	KEY(3, 2, KEY_K),
	KEY(3, 3, KEY_L),
	KEY(3, 4, KEY_ENTER),
	KEY(3, 5, KEY_UNKNOWN),

	KEY(4, 0, KEY_CAPSLOCK),
	KEY(4, 1, KEY_Z),
	KEY(4, 2, KEY_X),
	KEY(4, 3, KEY_C),
	KEY(4, 4, KEY_V),
	KEY(4, 5, KEY_B),

	KEY(5, 0, KEY_N),
	KEY(5, 1, KEY_M),
	KEY(5, 2, KEY_QUESTION),
	KEY(5, 3, KEY_UP),
	KEY(5, 4, KEY_RIGHTSHIFT),
	KEY(5, 5, KEY_UNKNOWN),

	KEY(6, 0, KEY_LEFTALT),
	KEY(6, 1, KEY_COMMA),
	KEY(6, 2, KEY_DOT),
	KEY(6, 3, KEY_COMMA),    // Need to be chaned to SYM key
	KEY(6, 4, KEY_SPACE),
	KEY(6, 5, KEY_LANGUAGE),

	KEY(7, 0, KEY_MAIL),
	KEY(7, 1, KEY_LEFT),
	KEY(7, 2, KEY_DOWN),
	KEY(7, 3, KEY_RIGHT),
	KEY(7, 4, KEY_UNKNOWN),
	KEY(7, 5, KEY_UNKNOWN),
#elif 1 //defined(LGE_MODEL_C800_REV_A)
/// KEY(DRV,SNS, keys) on PMIC Keypad.	// for rev.A

	KEY(0, 0, KEY_MENU), // ok
	KEY(0, 1, KEY_Q), // ok
	KEY(0, 2, KEY_W), // ok
	KEY(0, 3, KEY_E), // ok
	KEY(0, 4, KEY_UNKNOWN),
	KEY(0, 5, KEY_UNKNOWN),

	KEY(1, 0, KEY_R), // ok
	KEY(1, 1, KEY_T), // ok
	KEY(1, 2, KEY_Y), // ok
	KEY(1, 3, KEY_U), // ok 	// Need to be chaned to SYM key
	KEY(1, 4, KEY_UNKNOWN),
	KEY(1, 5, KEY_UNKNOWN),

	KEY(2, 0, KEY_I),  // ok
	KEY(2, 1, KEY_O),  // ok
	KEY(2, 2, KEY_P),  // ok
	KEY(2, 3, KEY_BACKSPACE), // ok
	KEY(2, 4, KEY_MAIL),  // ok
	KEY(2, 5, KEY_A),	// ok

	KEY(3, 0, KEY_S),  // ok
	KEY(3, 1, KEY_D),  // ok
	KEY(3, 2, KEY_F),  // ok
	KEY(3, 3, KEY_G),  // SYM
	KEY(3, 4, KEY_H), // ok
	KEY(3, 5, KEY_J), // ok

	KEY(4, 0, KEY_K), // ok
	KEY(4, 1, KEY_L), // ok
	KEY(4, 2, KEY_ENTER), //ok
	KEY(4, 3, KEY_CAPSLOCK), // nok
	KEY(4, 4, KEY_Z),	// ok
	KEY(4, 5, KEY_X),	// ok

	KEY(5, 0, KEY_C), // ok
	KEY(5, 1, KEY_V), //ok
	KEY(5, 2, KEY_B), // ok
	KEY(5, 3, KEY_N), // ok
	KEY(5, 4, KEY_M), //ok
	KEY(5, 5, KEY_QUESTION), //ok

	KEY(6, 0, KEY_RIGHT),	// ok
	KEY(6, 1, KEY_RIGHTSHIFT), // ok
	KEY(6, 2, KEY_LEFTALT),
	KEY(6, 3, KEY_COMMA),	 // 
	KEY(6, 4, KEY_DOT),  // ok
	KEY(6, 5, KEY_F15),  // SYM

	KEY(7, 0, KEY_SPACE), // ok
	KEY(7, 1, KEY_LANGUAGE),  // ok
	KEY(7, 2, KEY_F14), //ok
	KEY(7, 3, KEY_UP),  // ok
	KEY(7, 4, KEY_LEFT), //ok
	KEY(7, 5, KEY_DOWN), //ok

#else
 /// KEY(DRV,SNS, keys) on PMIC Keypad.  // for rev.A
 
	 KEY(0, 0, KEY_MENU), // ok
	 KEY(0, 1, KEY_Q), // ok
	 KEY(0, 2, KEY_W), // ok
	 KEY(0, 3, KEY_E), // ok
	 KEY(0, 4, KEY_UNKNOWN),
	 KEY(0, 5, KEY_UNKNOWN),
 
	 KEY(1, 0, KEY_R), // ok
	 KEY(1, 1, KEY_T), // ok
	 KEY(1, 2, KEY_Y), // ok
	 KEY(1, 3, KEY_U), // ok	 // Need to be chaned to SYM key
	 KEY(1, 4, KEY_UNKNOWN),
	 KEY(1, 5, KEY_UNKNOWN),
 
	 KEY(2, 0, KEY_I),  // ok
	 KEY(2, 1, KEY_O),  // ok
	 KEY(2, 2, KEY_P),  // ok
	 KEY(2, 3, KEY_SEARCH), // ok
	 KEY(2, 4, KEY_MAIL),  // ok
	 KEY(2, 5, KEY_A),   // ok
 
	 KEY(3, 0, KEY_S),  // ok
	 KEY(3, 1, KEY_D),  // ok
	 KEY(3, 2, KEY_F),  // ok
	 KEY(3, 3, KEY_G),  // SYM
	 KEY(3, 4, KEY_H), // ok
	 KEY(3, 5, KEY_J), // ok
 
	 KEY(4, 0, KEY_K), // ok
	 KEY(4, 1, KEY_L), // ok
	 KEY(4, 2, KEY_BACKSPACE), //ok
	 KEY(4, 3, KEY_LEFTSHIFT), // nok
	 KEY(4, 4, KEY_Z),   // ok
	 KEY(4, 5, KEY_X),   // ok
 
	 KEY(5, 0, KEY_C), // ok
	 KEY(5, 1, KEY_V), //ok
	 KEY(5, 2, KEY_B), // ok
	 KEY(5, 3, KEY_N), // ok
	 KEY(5, 4, KEY_M), //ok
	 KEY(5, 5, KEY_APOSTROPHE), //ok
 
	 KEY(6, 0, KEY_RIGHT),  // ok
	 KEY(6, 1, KEY_ENTER), // ok
	 KEY(6, 2, KEY_LEFTALT),
	 KEY(6, 3, KEY_F13),	  // Need to be chaned to SYM key
	 KEY(6, 4, KEY_WWW),  // ok
	 KEY(6, 5, KEY_COMMA),
 
	 KEY(7, 0, KEY_SPACE), // ok
	 KEY(7, 1, KEY_DOT),  // ok
	 KEY(7, 2, KEY_F14), //ok
	 KEY(7, 3, KEY_UP),  // ok
	 KEY(7, 4, KEY_LEFT), //ok
	 KEY(7, 5, KEY_DOWN), //ok
 
#endif

};


#else
static const unsigned int victor_keymap[] = {
	KEY(1, 1, KEY_VOLUMEUP),
	KEY(1, 2, KEY_VOLUMEDOWN),
};
#endif


static struct resource resources_keypad[] = {
	{
		.start	= PM8058_KEYPAD_IRQ(PMIC8058_IRQ_BASE),
		.end	= PM8058_KEYPAD_IRQ(PMIC8058_IRQ_BASE),
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= PM8058_KEYSTUCK_IRQ(PMIC8058_IRQ_BASE),
		.end	= PM8058_KEYSTUCK_IRQ(PMIC8058_IRQ_BASE),
		.flags	= IORESOURCE_IRQ,
	},
};

static struct matrix_keymap_data univaq_keymap_data = {
	.keymap_size   = ARRAY_SIZE(univaq_pm_keymap),
	.keymap    = univaq_pm_keymap,
};

static struct pmic8058_keypad_data univaq_keypad_data = {
	.input_name		= "univa_q-keypad",
	.input_phys_device	= "univa_q-keypad/input0",
	.num_rows		= 8,
	.num_cols		= 6,
	.rows_gpio_start	= 8,
	.cols_gpio_start	= 0,
	.debounce_ms		= {8, 10},
	.scan_delay_ms		= 32,
	.row_hold_ns		= 91500,
	.wakeup			= 1,
	.keymap_data		= &univaq_keymap_data,
};

static struct pm8058_pwm_pdata pm8058_pwm_data = {
	.config		= pm8058_pwm_config,
	.enable		= pm8058_pwm_enable,
};

/* Put sub devices with fixed location first in sub_devices array */
#define	PM8058_SUBDEV_KPD	0
#define	PM8058_SUBDEV_LED	1

static struct pm8058_gpio_platform_data pm8058_gpio_data = {
	.gpio_base	= PM8058_GPIO_PM_TO_SYS(0),
	.irq_base	= PM8058_GPIO_IRQ(PMIC8058_IRQ_BASE, 0),
	.init		= pm8058_gpios_init,
};

static struct pm8058_gpio_platform_data pm8058_mpp_data = {
	.gpio_base	= PM8058_GPIO_PM_TO_SYS(PM8058_GPIOS),
	.irq_base	= PM8058_MPP_IRQ(PMIC8058_IRQ_BASE, 0),
};


#ifdef CONFIG_MACH_MSM8X55_UNIVA_Q
static struct pmic8058_led pmic8058_univaq_leds[] = {
	[0] = {
		.name		= "keyboard-backlight",   
		.max_brightness = 6,
		.id		= PMIC8058_ID_LED_KB_LIGHT,
	},
	[1] = {
		.name		= "button-backlight",  
		.max_brightness = 6,
		.id		= PMIC8058_ID_LED_0,
	},
};

static struct pmic8058_leds_platform_data pm8058_univaq_leds_data = {
	.num_leds = ARRAY_SIZE(pmic8058_univaq_leds),
	.leds	= pmic8058_univaq_leds,
};
#endif

static struct mfd_cell pm8058_subdevs[] = {
	{	.name = "pm8058-keypad",
		.id		= -1,
		.num_resources	= ARRAY_SIZE(resources_keypad),
		.resources	= resources_keypad,
	},

#ifdef CONFIG_MACH_MSM8X55_UNIVA_Q
	{	.name = "pm8058-led",
		.id		= -1,
	},
#endif

	{	.name = "pm8058-gpio",
		.id		= -1,
		.platform_data	= &pm8058_gpio_data,
		.data_size	= sizeof(pm8058_gpio_data),
	},
	{	.name = "pm8058-mpp",
		.id		= -1,
		.platform_data	= &pm8058_mpp_data,
		.data_size	= sizeof(pm8058_mpp_data),
	},
	{	.name = "pm8058-pwm",
		.id		= -1,
		.platform_data	= &pm8058_pwm_data,
		.data_size	= sizeof(pm8058_pwm_data),
	},
};

static struct pm8058_platform_data pm8058_7x30_data = {
	.irq_base = PMIC8058_IRQ_BASE,

	.num_subdevs = ARRAY_SIZE(pm8058_subdevs),
	.sub_devices = pm8058_subdevs,
	.irq_trigger_flags = IRQF_TRIGGER_LOW,
};

static struct i2c_board_info pm8058_boardinfo[] __initdata = {
	{
		I2C_BOARD_INFO("pm8058-core", 0x55),
		.irq = MSM_GPIO_TO_INT(PMIC_GPIO_INT),
		.platform_data = &pm8058_7x30_data,
	},
};

int __init pmic8058_buses_init(void)
{
	if (gpio_tlmm_config(GPIO_CFG(PMIC_GPIO_INT, 1, GPIO_CFG_INPUT,
				  GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE))
		pr_err("%s: gpio_tlmm_config (gpio=%d) failed\n",
		       __func__, PMIC_GPIO_INT);

	pm8058_7x30_data.sub_devices[PM8058_SUBDEV_KPD].platform_data
		= &univaq_keypad_data;
	pm8058_7x30_data.sub_devices[PM8058_SUBDEV_KPD].data_size
		= sizeof(univaq_keypad_data);

	i2c_register_board_info(6 /* I2C_SSBI ID */, pm8058_boardinfo,
				ARRAY_SIZE(pm8058_boardinfo));

	return 0;
}

static struct msm_pm_platform_data msm_pm_data[MSM_PM_SLEEP_MODE_NR] = {
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE].supported = 1,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE].suspend_enabled = 1,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE].idle_enabled = 1,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE].latency = 8594,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE].residency = 23740,

	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN].supported = 1,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN].suspend_enabled = 1,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN].idle_enabled = 1,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN].latency = 4594,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN].residency = 23740,

	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE].supported = 1,
#ifdef CONFIG_MSM_STANDALONE_POWER_COLLAPSE
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE].suspend_enabled = 0,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE].idle_enabled = 1,
#else /*CONFIG_MSM_STANDALONE_POWER_COLLAPSE*/
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE].suspend_enabled = 0,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE].idle_enabled = 0,
#endif /*CONFIG_MSM_STANDALONE_POWER_COLLAPSE*/
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE].latency = 500,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE].residency = 6000,

	[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].supported = 1,
	[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].suspend_enabled
		= 1,
	[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].idle_enabled = 0,
	[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].latency = 443,
	[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].residency = 1098,

	[MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT].supported = 1,
	[MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT].suspend_enabled = 1,
	[MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT].idle_enabled = 1,
	[MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT].latency = 2,
	[MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT].residency = 0,
};

void __init lge_pm_set_platform_data(void)
{
	msm_pm_set_platform_data(msm_pm_data, ARRAY_SIZE(msm_pm_data));
}

#ifdef CONFIG_MACH_MSM8X55_UNIVA_Q
void __init pmic8058_leds_init(void)
{
		pm8058_7x30_data.sub_devices[PM8058_SUBDEV_LED].platform_data
			= &pm8058_univaq_leds_data;
		pm8058_7x30_data.sub_devices[PM8058_SUBDEV_LED].data_size
			= sizeof(pm8058_univaq_leds_data);
}
#endif

