/* arch/arm/mach-msm/lge/board-univaq-panel.c

 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/fb.h>
#include <linux/mfd/pmic8058.h>
#include <asm/mach-types.h>
#include <mach/mpp.h>
#include <mach/board.h>
#include <mach/vreg.h>
#include <mach/pmic.h>
#include <mach/rpc_pmapp.h>
#include <mach/board_lge.h>
#include "devices.h"
#include "board-univaq.h"

#define MSM_FB_LCDC_VREG_OP(name, op, level)			\
do { \
	vreg = vreg_get(0, name); \
	vreg_set_level(vreg, level); \
	if (vreg_##op(vreg)) \
		printk(KERN_ERR "%s: %s vreg operation failed \n", \
			(vreg_##op == vreg_enable) ? "vreg_enable" \
				: "vreg_disable", name); \
} while (0)

#if 0

static char *msm_fb_vreg[] = {
	"gp10",
	"gp6",
};

static int mddi_power_save_on;


static void msm_fb_mddi_power_save(int on)
{
	struct vreg *vreg;
	int flag_on = !!on;

	if (mddi_power_save_on == flag_on)
		return;

	mddi_power_save_on = flag_on;

	if (on) {
		MSM_FB_LCDC_VREG_OP(msm_fb_vreg[0], enable, 1800);
		MSM_FB_LCDC_VREG_OP(msm_fb_vreg[1], enable, 2800);
	} else{
		MSM_FB_LCDC_VREG_OP(msm_fb_vreg[0], disable, 0);
	}
}
#endif


static int msm_fb_mddi_sel_clk(u32 *clk_rate)
{
	*clk_rate *= 2;
	return 0;
}

static struct mddi_platform_data mddi_pdata = {

//	.mddi_power_save = msm_fb_mddi_power_save,
	.mddi_power_save = NULL,			// LCD power
	.mddi_sel_clk = msm_fb_mddi_sel_clk,		// Camera preview tearing	
};



static struct msm_panel_common_pdata mdp_pdata = {
	.gpio = 30,
	.mdp_core_clk_rate = 122880000,
};




static void __init msm_fb_add_devices(void)
{
	msm_fb_register_device("mdp", &mdp_pdata);
	msm_fb_register_device("pmdh", &mddi_pdata);
}
static int mddi_mainlcd_pmic_backlight(int level)
{
	/* TODO: Backlight control here */
	return 0;
}
static struct msm_panel_lgit_pdata mddi_mainlcd_panel_data = {

#if defined (CONFIG_LGE_MODEL_C800)
#if defined (CONFIG_LGE_MODEL_C800_REV_EVB) || defined (CONFIG_LGE_MODEL_C800_REV_A) || defined (CONFIG_LGE_MODEL_C800_REV_B)
	.gpio = 84,				/* lcd reset_n */
#else
	.gpio = 94,				/* lcd reset_n */
#endif
#else //defined (CONFIG_LGE_MODEL_C800)
	.gpio = 84,				/* lcd reset_n */
#endif //defined (CONFIG_LGE_MODEL_C800)

	.pmic_backlight = mddi_mainlcd_pmic_backlight,
	.initialized = 1,

};
static struct platform_device mddi_mainlcd_panel_device = {
	.name   = "mddi_auo_hvga",
	.id     = 0,
	.dev    = {
		.platform_data = &mddi_mainlcd_panel_data,
	}
};
////////////////////////////////////////////////////////
/* backlight device */

#if defined(CONFIG_LGE_MODEL_C800_REV_EVB)
static struct gpio_i2c_pin bl_i2c_pin[] = {
	[0] = {
		.sda_pin	= 75,
		.scl_pin	= 74,
		.reset_pin	= 25,
		.irq_pin	= 0,     /* need to be checked */
	},
};
#else
static struct gpio_i2c_pin bl_i2c_pin[] = {
	[0] = {
		.sda_pin	= 166,
		.scl_pin	= 165,
		.reset_pin	= 126,
		.irq_pin	= 0,     /* need to be checked */
	},
};
#endif

static struct i2c_gpio_platform_data bl_i2c_pdata = {
	.sda_is_open_drain	= 0,
	.scl_is_open_drain	= 0,
	.udelay				= 2,
};

static struct platform_device bl_i2c_device = {
	.name	= "i2c-gpio",
	.dev.platform_data = &bl_i2c_pdata,
};

static struct i2c_board_info bl_i2c_bdinfo[] = {
	[0] = {
		I2C_BOARD_INFO("lm3530", 0x38),
		.type = "lm3530",
		.platform_data = NULL, //&lm3530bl_data[0],
	},
};

#if defined(CONFIG_LGE_MODEL_C800_REV_EVB)
static struct backlight_platform_data lm3530bl_data[] = {
	[0] = {
		.gpio = 25,
	},
};
#else
static struct backlight_platform_data lm3530bl_data[] = {
	[0] = {
		.gpio = 126,
	},
};
#endif


struct msm_fb_info_st {
        unsigned int width_mm ;
        unsigned int height_mm ;
};

/*write down the mm size of LCD */
static struct msm_fb_info_st msm_fb_info_data = {
        .width_mm = 50,
        .height_mm = 83
};

static int msm_fb_event_notify(struct notifier_block *self,
                              unsigned long action, void *data)
{
        struct fb_event *event = data;
        struct fb_info *info = event->info;
        struct msm_fb_info_st *fb_info_mm = &msm_fb_info_data;
        int ret = 0;

        switch(action) {
        case FB_EVENT_FB_REGISTERED:
                info->var.width = fb_info_mm->width_mm;
                info->var.height = fb_info_mm->height_mm;
                break;
        }
        return ret;
}

static struct notifier_block msm_fb_event_notifier = {
        .notifier_call  = msm_fb_event_notify,
};

#if 0
#define GPIO_BL_I2C_SDA	75
#define GPIO_BL_I2C_SCL	74

static struct gpio_i2c_pin amp_i2c_pin[] = {
	[0] = {
    .sda_pin = GPIO_BL_I2C_SDA,
    .scl_pin = GPIO_BL_I2C_SCL,
	},
};

static struct i2c_gpio_platform_data amp_i2c_pdata = {
	.sda_is_open_drain = 0,
	.scl_is_open_drain = 0,
	.udelay = 2,
};

static struct platform_device amp_i2c_device = {
	.id = 14,
	.name = "i2c-gpio",
	.dev.platform_data = &amp_i2c_pdata,
};

static struct i2c_board_info amp_i2c_bdinfo[] = {
	[0] = {
	I2C_BOARD_INFO("lm3530", 0x38),
	.type = "lm3530",
	.platform_data = &lm3530bl_data[0],
	},


};
#endif

#if 0

int init_gpio_i2c_pin(struct i2c_gpio_platform_data *i2c_adap_pdata,
		struct gpio_i2c_pin gpio_i2c_pin,
		struct i2c_board_info *i2c_board_info_data)
{
	i2c_adap_pdata->sda_pin = gpio_i2c_pin.sda_pin;
	i2c_adap_pdata->scl_pin = gpio_i2c_pin.scl_pin;

	gpio_tlmm_config(GPIO_CFG(gpio_i2c_pin.sda_pin, 0, GPIO_CFG_OUTPUT,
				GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	gpio_tlmm_config(GPIO_CFG(gpio_i2c_pin.scl_pin, 0, GPIO_CFG_OUTPUT,
				GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	gpio_set_value(gpio_i2c_pin.sda_pin, 1);
	gpio_set_value(gpio_i2c_pin.scl_pin, 1);

	if (gpio_i2c_pin.reset_pin) {
		gpio_tlmm_config(GPIO_CFG(gpio_i2c_pin.reset_pin, 0, GPIO_CFG_OUTPUT,
					GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
		gpio_set_value(gpio_i2c_pin.reset_pin, 1);
	}

	if (gpio_i2c_pin.irq_pin) {
		gpio_tlmm_config(GPIO_CFG(gpio_i2c_pin.irq_pin, 0, GPIO_CFG_INPUT,
					GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
		i2c_board_info_data->irq =
			MSM_GPIO_TO_INT(gpio_i2c_pin.irq_pin);
	}

	return 0;
}
#endif


void __init univaq_init_i2c_backlight(int bus_num)
{
	bl_i2c_device.id = bus_num;
	bl_i2c_bdinfo[0].platform_data = &lm3530bl_data[0]; //[lge_bd_rev];
	
	init_gpio_i2c_pin(&bl_i2c_pdata, bl_i2c_pin[0],	&bl_i2c_bdinfo[0]);
	i2c_register_board_info(bus_num, &bl_i2c_bdinfo[0], 1);    
	platform_device_register(&bl_i2c_device);
}

void __init lge_add_lcd_devices(void)
{
	fb_register_client(&msm_fb_event_notifier);
	platform_device_register(&mddi_mainlcd_panel_device);
	lge_add_gpio_i2c_device(univaq_init_i2c_backlight);
	msm_fb_add_devices();
}
