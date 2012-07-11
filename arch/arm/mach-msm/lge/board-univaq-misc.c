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
#include <linux/power_supply.h>

#include <mach/msm_battery.h>
#include <mach/board.h>
#include <mach/board_lge.h>
// === motor_temp ===
#include <mach/vreg.h>
#include <mach/pmic.h>
#include <linux/pwm.h>
#include <linux/pmic8058-pwm.h>
#include <mach/msm_iomap.h>
#include <asm/io.h>
#include <linux/clk.h>
// === motor_temp ===
#include <mach/board_lge.h>
#include "board-univaq.h"

#ifdef CONFIG_LGE_FUEL_GAUGE
static u32 victor_battery_capacity(u32 current_soc)
{
	if(current_soc > 100)
		current_soc = 100;

	return current_soc;
}
#endif

static struct msm_psy_batt_pdata msm_psy_batt_data = {
	.voltage_min_design 	= 3200,
	.voltage_max_design	= 4200,
	.avail_chg_sources   	= AC_CHG | USB_CHG ,
	.batt_technology        = POWER_SUPPLY_TECHNOLOGY_LION,
#ifdef CONFIG_LGE_FUEL_GAUGE
	.calculate_capacity		= victor_battery_capacity,
#endif
};

static struct platform_device msm_batt_device = {
	.name 		    = "msm-battery",
	.id		    = -1,
	.dev.platform_data  = &msm_psy_batt_data,
};

int lge_vibrator_power_set(int enable)
{
#if defined(VIBE_USES_VREG)
	struct vreg *vibe_vreg;
	static int is_enabled = 0;

    //printk("[vibrator] %s (%d)\n", __FUNCTION__, enable);

	vibe_vreg = vreg_get(NULL, VIBE_IC_VREG);

	if (IS_ERR(vibe_vreg)) {
		printk(KERN_ERR "%s: vreg_get failed\n", __FUNCTION__);
		return PTR_ERR(vibe_vreg);
	}

	if (enable) {
		if (is_enabled) {
			//printk(KERN_INFO "vibrator power was enabled, already\n");
			return 0;
		}
		
		/* 3300 mV for Motor IC */
		if (vreg_set_level(vibe_vreg, VIBE_IC_VOLTAGE) <0) {		
			printk(KERN_ERR "%s: vreg_set_level failed\n", __FUNCTION__);
			return -EIO;
		}
		
		if (vreg_enable(vibe_vreg) < 0 ) {
			printk(KERN_ERR "%s: vreg_enable failed\n", __FUNCTION__);
			return -EIO;
		}
		is_enabled = 1;
	} else {
		if (!is_enabled) {
			//printk(KERN_INFO "vibrator power was disabled, already\n");
			return 0;
		}
		
		if (vreg_disable(vibe_vreg) < 0) {
			printk(KERN_ERR "%s: vreg_disable failed\n", __FUNCTION__);
			return -EIO;
		}
		is_enabled = 0;
	}
#endif
	return 0;
}


// === motor_temp ===
#if defined(VIBRATOR_USES_PMIC_PWM)
static struct pwm_device *vib_pwm = 0;
#elif defined(VIBRATOR_USES_GP_CLK_PWM)
static struct clk *gp_clk;
#endif
// === motor_temp ===


int lge_vibrator_pwm_set(int enable, int amp)
{

// === motor_temp ===
/*========== test_sch for vibrator test using amp value.=============
	int pwm_freq_hz = 25000;
	int rc =0;

	int pwm_period_usec;
	int pwm_level;
	int pwm_duty_level;

	pwm_freq_hz = amp;
	pwm_period_usec = (USEC_PER_SEC / pwm_freq_hz);
	pwm_level = 128;
	pwm_duty_level = (pwm_period_usec / pwm_level);
======================================================================*/
// === motor_temp ===
#if defined(VIBRATOR_USES_PMIC_PWM)

	int pwm_freq_hz = 24000;
	int rc =0;

	int pwm_period_usec;
	int pwm_level;
	int pwm_duty_level;

	pwm_period_usec = (USEC_PER_SEC / pwm_freq_hz);
	pwm_level = 128;
	pwm_duty_level = (pwm_period_usec / pwm_level);

		if(vib_pwm == 0)
			vib_pwm = pwm_request(0 , "vibrator_pwm_test");

		rc = pwm_config(vib_pwm, ((pwm_period_usec*amp)/pwm_level), pwm_period_usec);

		if (rc >= 0)
		{
			if(enable)
				pwm_enable(vib_pwm);
			else
				pwm_disable(vib_pwm);
		}

#elif defined(VIBRATOR_USES_GP_MN_PWM)

	int gain = ((PWM_MAX_HALF_DUTY * amp) >> 7) + GPMN_D_DEFAULT;

	REG_WRITEL((GPMN_M_DEFAULT & GPMN_M_MASK), GP_MN_CLK_MDIV_REG);
	REG_WRITEL((~( GPMN_N_DEFAULT - GPMN_M_DEFAULT )&GPMN_N_MASK), GP_MN_CLK_NDIV_REG);

	if (enable) {
		REG_WRITEL((gain & GPMN_D_MASK), GP_MN_CLK_DUTY_REG);
		gpio_tlmm_config(GPIO_CFG(GPIO_LIN_MOTOR_PWM, 2, GPIO_CFG_OUTPUT,
			GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
		gpio_direction_output(GPIO_LIN_MOTOR_PWM, 1);
	} else {
		REG_WRITEL(GPMN_D_DEFAULT, GP_MN_CLK_DUTY_REG);
		gpio_tlmm_config(GPIO_CFG(GPIO_LIN_MOTOR_PWM, 2, GPIO_CFG_OUTPUT,
			GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
		gpio_direction_output(GPIO_LIN_MOTOR_PWM, 0);
	}

#elif defined(VIBRATOR_USES_GP_CLK_PWM)

	int err = 0;
	int gain = 0;
//    static int old_gain;
//    static int same_count;
 
	static int gp_clk_enable = 0;

	//if(amp > 127) amp = 127;
	//else if(amp < -127) amp = -127;

	gain = ((amp * GP_MD_D_MULTIPLIER) >> 8) + GP_MD_D_DEFAULT;
/*
	if(old_gain != gain)
	{
		printk("[vibrator] %s GP_CLK is called %d,%d,%d,(%d)\n", __func__,enable,amp,gain,same_count);
		old_gain = gain;
		same_count = 0;
	}
	else
	{
        same_count++;
	}
*/
	/* Error detection code */
	if(gp_clk == NULL)
	{
		gp_clk = clk_get(NULL, "gp_clk");
		if (IS_ERR(gp_clk)) {
			printk(KERN_ERR "flip_vibrator_pwm_set: could not get gp clk\n");
			gp_clk = NULL;
		}
		else
		{	
			err = clk_set_rate(gp_clk, 19200000);  // TCXO
			if (err)
				printk(KERN_ERR "flip_vibrator_pwm_set: set clock rate failed\n");
		}
	}

	if (enable)
	{
		if(gp_clk != NULL && gp_clk_enable == 0)
		{
			clk_enable(gp_clk); 
			gp_clk_enable = 1;
		}

		/* gpio config to gp_clk_x */
		gpio_tlmm_config(GPIO_CFG(GPIO_LIN_MOTOR_PWM, 2, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_4MA), GPIO_CFG_ENABLE);

		/* disable gp_clk branch */
	    HWIO_OUTM(GP_NS_REG, HWIO_GP_NS_REG_GP_CLK_BRANCH_ENA_BMSK, 0 << HWIO_GP_NS_REG_GP_CLK_BRANCH_ENA_SHFT);

		/* disable gp_clk_src */
	    HWIO_OUTM(GP_NS_REG, HWIO_GP_NS_REG_GP_ROOT_ENA_BMSK, 0 << HWIO_GP_NS_REG_GP_ROOT_ENA_SHFT);

	    /* disable M/N counter */	
	    HWIO_OUTM(GP_NS_REG, HWIO_GP_NS_REG_MNCNTR_EN_BMSK, 0 << HWIO_GP_NS_REG_MNCNTR_EN_SHFT);   

        /* Set MD & NS register */

	    /* inactive M/N counter reset */
	    HWIO_OUTM(GP_NS_REG, HWIO_GP_NS_REG_MNCNTR_RST_BMSK, 0 << HWIO_GP_NS_REG_MNCNTR_RST_SHFT);

	    /* CLK_SRC_SEL : 000 => SYS_CLK TCXO(19.2MHz), 110 => SKEEO_CLK(32kHz) */
	    HWIO_OUTM(GP_NS_REG, HWIO_GP_NS_REG_SRC_SEL_BMSK, 0 << HWIO_GP_NS_REG_SRC_SEL_SHFT);

	    /* PRE_DIV_SEL: 00 => bypass, 01 => div-by-2, 10 => div-by-3, 11 => div-by-4 */
	    HWIO_OUTM(GP_NS_REG, HWIO_GP_NS_REG_PRE_DIV_SEL_BMSK, 0 << HWIO_GP_NS_REG_PRE_DIV_SEL_SHFT);
	  
	    /* configure M value */
	    HWIO_OUTM(GP_MD_REG, HWIO_GP_MD_REG_M_VAL_BMSK, GP_MD_M_DEFAULT << HWIO_GP_MD_REG_M_VAL_SHFT);
	  
	    /* configure D value */ /* Duty 50% : force 0 without Inverter */
	    //HWIO_OUTM(GP_MD_REG, HWIO_GP_MD_REG_D_VAL_BMSK, ~(GP_MD_D_DEFAULT<<1) << HWIO_GP_MD_REG_D_VAL_SHFT);

	    /* configure D value */ /* Duty gain% : force 0 without Inverter */
	    HWIO_OUTM(GP_MD_REG, HWIO_GP_MD_REG_D_VAL_BMSK, (~( gain << 1)) << HWIO_GP_MD_REG_D_VAL_SHFT);
	  
	    /* configure N value */
	    HWIO_OUTM(GP_NS_REG, HWIO_GP_NS_REG_GP_N_VAL_BMSK, (~(GP_NS_N_DEFAULT - GP_MD_M_DEFAULT)) << HWIO_GP_NS_REG_GP_N_VAL_SHFT);

	    /* configure not inverted gp_clk */
	    HWIO_OUTM(GP_NS_REG, HWIO_GP_NS_REG_GP_CLK_INV_BMSK, 0 << HWIO_GP_NS_REG_GP_CLK_INV_SHFT);

	    /* configure M/N counter mode : 0 => bypass, 1 => clock swallow, 2 => dual-edge mode, 3 => single-edge mode*/
	    HWIO_OUTM(GP_NS_REG, HWIO_GP_NS_REG_MNCNTR_MODE_BMSK, 3 << HWIO_GP_NS_REG_MNCNTR_MODE_SHFT);
	  
	    /* enable M/N counter */
	    HWIO_OUTM(GP_NS_REG, HWIO_GP_NS_REG_MNCNTR_EN_BMSK, 1 << HWIO_GP_NS_REG_MNCNTR_EN_SHFT);

		/* enable gp_clk_src */
	    HWIO_OUTM(GP_NS_REG, HWIO_GP_NS_REG_GP_ROOT_ENA_BMSK, 1 << HWIO_GP_NS_REG_GP_ROOT_ENA_SHFT);

		/* enable gp_clk branch */
	    HWIO_OUTM(GP_NS_REG, HWIO_GP_NS_REG_GP_CLK_BRANCH_ENA_BMSK, 1 << HWIO_GP_NS_REG_GP_CLK_BRANCH_ENA_SHFT);

	}
	else 
	{
		/* disable gp_clk branch */
	    HWIO_OUTM(GP_NS_REG, HWIO_GP_NS_REG_GP_CLK_BRANCH_ENA_BMSK, 0 << HWIO_GP_NS_REG_GP_CLK_BRANCH_ENA_SHFT);

		/* disable gp_clk_src */
	    HWIO_OUTM(GP_NS_REG, HWIO_GP_NS_REG_GP_ROOT_ENA_BMSK, 0 << HWIO_GP_NS_REG_GP_ROOT_ENA_SHFT);

	    /* disable M/N counter */	
	    HWIO_OUTM(GP_NS_REG, HWIO_GP_NS_REG_MNCNTR_EN_BMSK, 0 << HWIO_GP_NS_REG_MNCNTR_EN_SHFT);   

		if(gp_clk != NULL && gp_clk_enable == 1)
		{
			clk_disable(gp_clk);
			gp_clk_enable = 0;
		}
	}
#endif


	return 0;
}

int lge_vibrator_ic_enable_set(int enable)
{
	//printk("[vibrator] %s (%d)\n", __func__,enable);

	if (enable) {
		gpio_tlmm_config(GPIO_CFG(MOTER_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
		gpio_direction_output(MOTER_EN, 1);
		}
	else{
		gpio_tlmm_config(GPIO_CFG(MOTER_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
		gpio_direction_output(MOTER_EN, 0);
		}

	return 0;
}

static struct lge_vibrator_platform_data univa_q_vibrator_data = {
	.enable_status = 0,
	.power_set = lge_vibrator_power_set,
	.pwm_set = lge_vibrator_pwm_set,
	.ic_enable_set = lge_vibrator_ic_enable_set,
//	.amp_value = 105,
	.amp_value = 127,
};

#if defined(CONFIG_ANDROID_VIBRATOR)
 static struct platform_device android_vibrator_device = {
 	.name   = "android-vibrator",
 	.id = -1,
 	.dev = {
 		.platform_data = &univa_q_vibrator_data,
 	},
 };
#elif defined(CONFIG_IMMERSION_VIBRATOR)
static struct platform_device tspdrv_vibrator_device = {
	.name   = "tspdrv",
	.id = -1,
	.dev = {
		.platform_data = &univa_q_vibrator_data,
	},
};
#else
#error "Vibrator solution is not defined"
#endif

/* misc platform devices */
static struct platform_device *misc_devices[] __initdata = {
	&msm_batt_device,
#if defined(CONFIG_ANDROID_VIBRATOR)
	&android_vibrator_device,
#elif defined(CONFIG_IMMERSION_VIBRATOR)
	&tspdrv_vibrator_device,
#endif
};

/* main interface */
void __init lge_add_misc_devices(void)
{
	gpio_request(MOTER_EN, "motor enable");
#if defined(VIBRATOR_USES_GP_CLK_PWM)
	gpio_request(GPIO_LIN_MOTOR_PWM, "motor pwm");
#endif
	platform_add_devices(misc_devices, ARRAY_SIZE(misc_devices));
}
