/* arch/arm/mach-msm/include/mach/board-victor.h
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#ifndef __ARCH_MSM_BOARD_VICTOR_H
#define __ARCH_MSM_BOARD_VICTOR_H

#include <linux/types.h>
#include <linux/list.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <asm/setup.h>

#include <mach/msm_iomap.h>

#include "pm.h"


#ifdef CONFIG_FB_MSM_TRIPLE_BUFFER
#define MSM_FB_SIZE           		0x780000 
#else 
#define MSM_FB_SIZE           		0x500000 
#endif
#define MSM_PMEM_SF_SIZE      		0x1D00000 //(this can be zero if TARGET_GRALLOC_USES_ASHMEM enabled) 

#define MSM_PMEM_ADSP_SIZE    		0x1D1A000 
#define PMEM_KERNEL_EBI1_SIZE 		0x600000 
#define MSM_PMEM_AUDIO_SIZE   		0x200000 
#define MSM_FLUID_PMEM_ADSP_SIZE	0x2800000


#if 1 	// kh.tak MicroSD Detection
#define SYS_GPIO_SD_DET    42  /* SYS SD Detection GPIO Number 42 */
#define SYS_GPIO_SD_EN_N   56 /* SYS SD Enable GPIO Number 56*/
#endif

#define PMIC_GPIO_INT		27
#define PMIC_VREG_WLAN_LEVEL	2900
#define PMIC_GPIO_SD_DET	36
#define PMIC_GPIO_SDC4_EN	17  /* PMIC GPIO Number 18 */
#define PMIC_GPIO_HDMI_5V_EN_V3 32  /* PMIC GPIO for V3 H/W */
#define PMIC_GPIO_HDMI_5V_EN_V2 39 /* PMIC GPIO for V2 H/W */


#define FPGA_SDCC_STATUS       0x8E0001A8

#define FPGA_OPTNAV_GPIO_ADDR	0x8E000026
#define OPTNAV_I2C_SLAVE_ADDR	(0xB0 >> 1)
#define OPTNAV_IRQ		20
#define OPTNAV_CHIP_SELECT	19

/* Macros assume PMIC GPIOs start at 0 */
#define PM8058_GPIO_PM_TO_SYS(pm_gpio)     (pm_gpio + NR_GPIO_IRQS)
#define PM8058_GPIO_SYS_TO_PM(sys_gpio)    (sys_gpio - NR_GPIO_IRQS)

#define PMIC_GPIO_HAP_ENABLE   16  /* PMIC GPIO Number 17 */

#define HAP_LVL_SHFT_MSM_GPIO 24

#define PMIC_GPIO_QUICKVX_CLK 37 /* PMIC GPIO 38 */

#define	PM_FLIP_MPP 5 /* PMIC MPP 06 */

#if defined(CONFIG_LGE_SU610_NFC)||defined(CONFIG_LGE_KU6100_NFC)
//seokmin for NFC
/* nfc */
#define	NFC_GPIO_VEN			167
#define NFC_GPIO_I2C_SDA		165
#define NFC_GPIO_I2C_SCL		166
//#define NFC_GPIO_CLK_REQ		167
#define NFC_GPIO_SW_UPGRADE		168
#define NFC_GPIO_IRQ			162
#define NFC_I2C_SLAVE_ADDR		0x28
#endif

/* virtual key */
#define ATCMD_VIRTUAL_KEYPAD_ROW	8
#define ATCMD_VIRTUAL_KEYPAD_COL	8

/* key_touch */

#if defined(CONFIG_MACH_MSM8X55_UNIVA_Q)
#define KEY_TOUCH_GPIO_INT      176
#define KEY_TOUCH_GPIO_I2C_SCL  175
#define KEY_TOUCH_GPIO_I2C_SDA  174

#else
#define KEY_TOUCH_GPIO_INT      176
#define KEY_TOUCH_GPIO_I2C_SCL  175
#define KEY_TOUCH_GPIO_I2C_SDA  174
#endif


#define SO340010_I2C_ADDRESS   0x2C

/* touch-screen */

#if defined(CONFIG_MACH_MSM8X55_UNIVA_Q)

#define MAIN_TS_X_MIN				0
#define MAIN_TS_X_MAX				320
#define MAIN_TS_Y_MIN				0
#define MAIN_TS_X_SCRN_MAX			320
#define MAIN_TS_Y_SCRN_MAX			480
#define MAIN_TS_Y_MAX				480
#define MAIN_TS_Y_START				0
#define MAIN_TS_GPIO_I2C_SDA		71
#define MAIN_TS_GPIO_I2C_SCL		70
#define MAIN_TS_GPIO_INT			43
#define MAIN_TS_GPIO_IRQ			MSM_GPIO_TO_INT(MAIN_TS_GPIO_INT)
#define MAIN_TS_GPIO_RESET			121
#else

#define MAIN_TS_X_MIN				0
#define MAIN_TS_X_MAX				480
#define MAIN_TS_Y_MIN				0
#define MAIN_TS_Y_MAX				800
#define MAIN_TS_Y_START				0
#define MAIN_TS_GPIO_I2C_SDA		170
#define MAIN_TS_GPIO_I2C_SCL		169
#define MAIN_TS_GPIO_INT			43
#define MAIN_TS_GPIO_IRQ			MSM_GPIO_TO_INT(MAIN_TS_GPIO_INT)

#endif

#define MXT224_TS_I2C_SLAVE_ADDR	0x4A

/* accelerometer */
#define ACCEL_GPIO_INT	 		171
#define ACCEL_GPIO_I2C_SCL  	178
#define ACCEL_GPIO_I2C_SDA  	177
#define ACCEL_I2C_ADDRESS		0x18

/*Ecompass*/
#define ECOM_GPIO_I2C_SCL		172
#define ECOM_GPIO_I2C_SDA		173
#define ECOM_GPIO_INT			123
#define ECOM_I2C_ADDRESS		0x0E /* slave address 7bit */

/* proximity */

#if defined(LGE_MODEL_C800_REV_EVB)
#define PROXI_GPIO_I2C_SCL		149
#define PROXI_GPIO_I2C_SDA		150
#define PROXI_GPIO_DOUT			50
#define PROXI_I2C_ADDRESS		0x44 /*slave address 7bit*/
#else
#define PROXI_GPIO_I2C_SCL		172
#define PROXI_GPIO_I2C_SDA		173
#define PROXI_GPIO_DOUT			50
#define PROXI_I2C_ADDRESS		0x44 /*slave address 7bit*/
#endif



/* vibrator */

#if !defined(LGE_MODEL_C800_REV_A)
#define VIBRATOR_USES_GP_CLK_PWM
//#define VIBRATOR_USES_GP_MN_PWM
#define GPIO_LIN_MOTOR_PWM		98
#define MOTER_EN				125 
#define VIBE_USES_VREG
#define VIBE_IC_VREG			"gp13"
#define VIBE_IC_VOLTAGE			3300
#else
#define VIBRATOR_USES_PMIC_PWM
#define MOTER_EN				125  
#define VIBE_USES_VREG
//#define VIBE_USES_LDO
#define VIBE_IC_VREG			"gp13"
#define VIBE_IC_VOLTAGE			3300
#endif

#if defined(VIBRATOR_USES_GP_MN_PWM)
#define GP_MN_CLK_MDIV_REG		0x004C
#define GP_MN_CLK_NDIV_REG		0x0050
#define GP_MN_CLK_DUTY_REG		0x0054

/* about 22.93 kHz, should be checked */
#define GPMN_M_DEFAULT			21
#define GPMN_N_DEFAULT			4500
/* default duty cycle = disable motor ic */
#define GPMN_D_DEFAULT			(GPMN_N_DEFAULT >> 1) 
#define PWM_MAX_HALF_DUTY		((GPMN_N_DEFAULT >> 1) - 80) /* minimum operating spec. should be checked */

#define GPMN_M_MASK				0x01FF
#define GPMN_N_MASK				0x1FFF
#define GPMN_D_MASK				0x1FFF

#define REG_WRITEL(value, reg)	writel(value, (MSM_WEB_BASE + reg))
#endif

#if defined(VIBRATOR_USES_GP_CLK_PWM)
// MSM hwio register access set
#define __inpdw(port)							ioread32(port)
#define __outpdw(port, val)						(iowrite32((uint32_t) (val), port))
#define in_dword(addr)							(__inpdw(addr))
#define in_dword_masked(addr, mask)				(__inpdw(addr) & (mask))
#define out_dword(addr, val)					__outpdw(addr, val)
#define out_dword_masked_ns(io, mask, val, current_reg_content) \
	out_dword(io, ((current_reg_content & (u32)(~(mask))) | \
	((u32)((val) & (mask)))))

#define __msmhwio_outm(hwiosym, mask, val)		HWIO_##hwiosym##_OUTM(mask, val)
#define HWIO_OUTM(hwiosym, mask, val)			__msmhwio_outm(hwiosym, mask, val)

// GP_CLK register set

// GP_LCK MD Register
#define HWIO_GP_MD_REG_ADDR						(MSM_CLK_CTL_BASE      + 0x00000058)
#define HWIO_GP_MD_REG_RMSK						0xffffffff
#define HWIO_GP_MD_REG_SHFT						0
#define HWIO_GP_MD_REG_IN						in_dword_masked(HWIO_GP_MD_REG_ADDR, HWIO_GP_MD_REG_RMSK)
#define HWIO_GP_MD_REG_INM(m)					in_dword_masked(HWIO_GP_MD_REG_ADDR, m)
#define HWIO_GP_MD_REG_OUT(v)					out_dword(HWIO_GP_MD_REG_ADDR,v)
#define HWIO_GP_MD_REG_OUTM(m,v)				out_dword_masked_ns(HWIO_GP_MD_REG_ADDR,m,v,HWIO_GP_MD_REG_IN);

#define HWIO_GP_MD_REG_M_VAL_BMSK				0xffff0000
#define HWIO_GP_MD_REG_M_VAL_SHFT				0x10
#define HWIO_GP_MD_REG_D_VAL_BMSK				0xffff
#define HWIO_GP_MD_REG_D_VAL_SHFT				0


// GP_LCK MS Register
#define HWIO_GP_NS_REG_ADDR						(MSM_CLK_CTL_BASE+0x0000005C)
#define HWIO_GP_NS_REG_RMSK						0xffffffff
#define HWIO_GP_NS_REG_SHFT     	            0

#define HWIO_GP_NS_REG_IN						in_dword_masked(HWIO_GP_NS_REG_ADDR, HWIO_GP_NS_REG_RMSK)
#define HWIO_GP_NS_REG_INM(m)					in_dword_masked(HWIO_GP_NS_REG_ADDR, m)
#define HWIO_GP_NS_REG_OUT(v)					out_dword(HWIO_GP_NS_REG_ADDR,v)
#define HWIO_GP_NS_REG_OUTM(m,v)				out_dword_masked_ns(HWIO_GP_NS_REG_ADDR,m,v,HWIO_GP_NS_REG_IN);

#define HWIO_GP_NS_REG_GP_N_VAL_BMSK			0xffff0000
#define HWIO_GP_NS_REG_GP_N_VAL_SHFT			0x10
#define HWIO_GP_NS_REG_GP_ROOT_ENA_BMSK			0x800
#define HWIO_GP_NS_REG_GP_ROOT_ENA_SHFT			0xb
#define HWIO_GP_NS_REG_GP_CLK_INV_BMSK			0x400
#define HWIO_GP_NS_REG_GP_CLK_INV_SHFT			0xa
#define HWIO_GP_NS_REG_GP_CLK_BRANCH_ENA_BMSK	0x200
#define HWIO_GP_NS_REG_GP_CLK_BRANCH_ENA_SHFT	0x9
#define HWIO_GP_NS_REG_MNCNTR_EN_BMSK			0x100
#define HWIO_GP_NS_REG_MNCNTR_EN_SHFT			0x8
#define HWIO_GP_NS_REG_MNCNTR_RST_BMSK			0x80
#define HWIO_GP_NS_REG_MNCNTR_RST_SHFT			0x7
#define HWIO_GP_NS_REG_MNCNTR_MODE_BMSK			0x60
#define HWIO_GP_NS_REG_MNCNTR_MODE_SHFT			0x5
#define HWIO_GP_NS_REG_PRE_DIV_SEL_BMSK			0x18
#define HWIO_GP_NS_REG_PRE_DIV_SEL_SHFT			0x3
#define HWIO_GP_NS_REG_SRC_SEL_BMSK				0x7
#define HWIO_GP_NS_REG_SRC_SEL_SHFT				0

#define GP_MD_M_DEFAULT							7
#define GP_NS_N_DEFAULT							5960
#define GP_MD_D_DEFAULT							(GP_NS_N_DEFAULT >> 1) 

#define	GP_MD_D_MULTIPLIER						5924
#endif

/* Audio Subsystem(AMP) I2C Number */
#define GPIO_AMP_I2C_SDA	89
#define GPIO_AMP_I2C_SCL	88


#endif
