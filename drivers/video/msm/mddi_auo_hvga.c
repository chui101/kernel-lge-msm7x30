/* drivers/video/msm/src/panel/mddi/mddi_auo_hvga.c
 *
 * Copyright (C) 2008 QUALCOMM Incorporated.
 * Copyright (c) 2008 QUALCOMM USA, INC.
 * 
 * All source code in this file is licensed under the following license
 * except where indicated.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you can find it at http://www.fsf.org
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>



#include "msm_fb.h"
#include "mddihost.h"
#include "mddihosti.h"
//#include "mddi_lgit.h"
#include <linux/gpio.h>
#include <mach/gpio.h>
#include "msm_fb_def.h"
#include <mach/vreg.h>
#include <mach/board.h>

//#include <mach/board-victor.h>

//#include "msm_fb.h"
//#include "mddihost.h"
//#include "mddihosti.h"
//#include <asm/gpio.h>
//#include <mach/vreg.h>

//#include <mach/board_lge.h>


#define PANEL_DEBUG 0

//#define USE_TENTATIVE_COMMAND	1

#define LCD_CONTROL_BLOCK_BASE	0x110000
#define INTFLG		LCD_CONTROL_BLOCK_BASE|(0x18)
#define INTMSK		LCD_CONTROL_BLOCK_BASE|(0x1c)
#define VPOS		LCD_CONTROL_BLOCK_BASE|(0xc0)

//static uint32 mddi_auo_curr_vpos;
//static boolean mddi_auo_monitor_refresh_value = FALSE;
//static boolean mddi_auo_report_refresh_measurements = FALSE;
static boolean is_lcd_on = -1;

/* The comment from AMSS codes:
 * Dot clock (10MHz) / pixels per row (320) = rows_per_second
 * Rows Per second, this number arrived upon empirically 
 * after observing the timing of Vsync pulses
 * XXX: TODO: change this values for INNOTEK PANEL */
static uint32 mddi_auo_rows_per_second = 31250;
static uint32 mddi_auo_rows_per_refresh = 480;
//static uint32 mddi_auo_usecs_per_refresh = 15360; /* rows_per_refresh / rows_per_second */
extern boolean mddi_vsync_detect_enabled;

static msm_fb_vsync_handler_type mddi_auo_vsync_handler = NULL;
static void *mddi_auo_vsync_handler_arg;
static uint16 mddi_auo_vsync_attempts;

//static struct msm_panel_common_pdata *mddi_auo_pdata;
//static struct msm_panel_auo_pdata *mddi_auo_pdata;

static struct msm_panel_lgit_pdata *mddi_auo_pdata;




static int mddi_auo_lcd_on(struct platform_device *pdev);
static int mddi_auo_lcd_off(struct platform_device *pdev);

static int mddi_auo_lcd_init(void);
static void mddi_auo_lcd_panel_poweron(void);
// static void mddi_auo_lcd_panel_poweroff(void);

#define DEBUG 1
#if DEBUG
#define EPRINTK(fmt, args...) printk(fmt, ##args)
#else
#define EPRINTK(fmt, args...) do { } while (0)
#endif

struct display_table {
    unsigned reg;
    unsigned char count;
    unsigned val_list[256];
};

#define REGFLAG_DELAY             0XFFFE
#define REGFLAG_END_OF_TABLE      0xFFFF   // END OF REGISTERS MARKER



static struct display_table mddi_auo_display_off[] = {
	// Display off sequence
	{0x28, 4, {0x00, 0x00, 0x00, 0x00}},
	//{REGFLAG_DELAY, 40, {0}},

	// Sleep Mode In
	{0x10, 4, {0x00, 0x00, 0x00, 0x00}},
	{REGFLAG_DELAY, 120, {0}},
        
	{REGFLAG_END_OF_TABLE, 0x00, {0}}
};


static struct display_table mddi_auo_init_on[] = {

#if defined (LGE_MODEL_C800)
#if 1			//LH350H05-SD02 (2011.04.06)
	{REGFLAG_DELAY, 10, {0}},

	// Sleep Out
	{0x11, 1, {0}},
	{REGFLAG_DELAY, 120, {0}},

	// MCAP
	{0xb0, 1, {0x04}},

	// Set Tear On
	{0x35, 1, {0}},

	// Set Address Mode
	{0x36, 1, {0x08}},

	// Set Pixel Format
	{0x3a, 1, {0x55}},    //0x55 : 16bit, 0x66 : 18bit, 0x77 : 24bit

	// Set Column Address
	{0x2a, 4, {0x00, 0x00, 0x01, 0x3f}},

	// Set Page Address
	{0x2b, 4, {0x00, 0x00, 0x01, 0xdf}},

	// Frame Memory Access and Interface Setting
	{0xb3, 4, {0x02, 0x00, 0x00, 0x00}},

	// Panel Driving Setting
	{0xc0, 8, {0x01, 0xdf, 0x40, 0x10, 0x00, 0x01, 0x00, 0x33}},
	
    	// Display Timing Setting for Normal Mode
	{0xc1, 5, {0x07, 0x27, 0x08, 0x08, 0x10}},

	// Source/Gate Driving Timing Setting
	{0xc4, 4, {0x77, 0x00, 0x03, 0x01}},
	
	// DPI Polarity Control
	{0xc6, 1, {0x00}},
	
	// Gamma Setting A Set
	{0xc8, 24, {0x00, 0x04, 0x10, 0x20, 0x2d, 0x46, 0x39, 0x24,
                0x16, 0x0b, 0x05, 0x00, 0x00, 0x04, 0x10, 0x20,
                0x2d, 0x46, 0x39, 0x24, 0x16, 0x0b, 0x05, 0x00}},
	
	// Gamma Setting B Set
	{0xc9, 24, {0x00, 0x04, 0x10, 0x20, 0x2d, 0x46, 0x39, 0x24,
                0x16, 0x0b, 0x05, 0x00, 0x00, 0x04, 0x10, 0x20,
                0x2d, 0x46, 0x39, 0x24, 0x16, 0x0b, 0x05, 0x00}},
	
	// Gamma Setting C Set
	{0xca, 24, {0x00, 0x04, 0x10, 0x20, 0x2d, 0x46, 0x39, 0x24,
                0x16, 0x0b, 0x05, 0x00, 0x00, 0x04, 0x10, 0x20,
                0x2d, 0x46, 0x39, 0x24, 0x16, 0x0b, 0x05, 0x00}},
	
	// Power Setting (Charge Pump Setting)
	{0xd0, 16, {0x95, 0x06, 0x08, 0x20, 0x31, 0x04, 0x01, 0x00,
                0x08, 0x01, 0x00, 0x06, 0x01, 0x00, 0x00, 0x20}},

	// VCOM Setting
	{0xd1, 4, {0x02, 0x22, 0x22, 0x33}},

	// NVM Access Control
	{0xe0, 4, {0x00, 0x00, 0x00, 0x00}},

	// Set DDB Write Control
	{0xe1, 6, {0x00, 0x00, 0x00, 0x00, 0x00, 0x70}},

	// NVM Load Control
	{0xe2, 1, {0x80}},

	// Write Memory Start
	{0x2C, 1, {0}},

	// Display ON
	{0x29, 1, {0}},
	{REGFLAG_DELAY, 40, {0}},

	{REGFLAG_END_OF_TABLE, 0x00, {0}}
#else
	// MCAP
	{0xb0, 4, {0x04, 0x00, 0x00, 0x00}},

	// Set Tear On
	{0x35, 4, {0x00, 0x00, 0x00, 0x00}},

	// Set Address Mode
	{0x36, 4, {0x08, 0x00, 0x00, 0x00}},

	// Set Pixel Format
	{0x3a, 4, {0x55, 0x00, 0x00, 0x00}},    //0x55 : 16bit, 0x66 : 18bit, 0x77 : 24bit

	// Set Column Address
	{0x2a, 4, {0x00, 0x00, 0x01, 0x3f}},

	// Set Page Address
	{0x2b, 4, {0x00, 0x00, 0x01, 0xdf}},

	// Frame Memory Access and Interface Setting
	{0xb3, 4, {0x02, 0x00, 0x00, 0x00}},

	// Panel Driving Setting
//	{0xc0, 8, {0x01, 0xdf, 0x40, 0x13, 0x00, 0x01, 0x00, 0x33}},
	{0xc0, 8, {0x01, 0xdf, 0x40, 0x13, 0xF0, 0x01, 0x00, 0x33}},
	
/*
    // Display Timing Setting for Normal Mode
	{0xc1, 8, {0x07, 0x27, 0x08, 0x08, 0x50, 0x00, 0x00, 0x00}},
*/

	// Source/Gate Driving Timing Setting
	{0xc4, 4, {0x44, 0x00, 0x03, 0x03}},
	
	// DPI Polarity Control
	{0xc6, 4, {0x00, 0x00, 0x00, 0x00}},
	
	// Gamma Setting A Set
	{0xc8, 24, {0x00, 0x04, 0x10, 0x20, 0x2d, 0x46, 0x39, 0x24,
                0x16, 0x0b, 0x05, 0x00, 0x00, 0x04, 0x10, 0x20,
                0x2d, 0x46, 0x39, 0x24, 0x16, 0x0b, 0x05, 0x00}},
	
	// Gamma Setting B Set
	{0xc9, 24, {0x00, 0x04, 0x10, 0x20, 0x2d, 0x46, 0x39, 0x24,
                0x16, 0x0b, 0x05, 0x00, 0x00, 0x04, 0x10, 0x20,
                0x2d, 0x46, 0x39, 0x24, 0x16, 0x0b, 0x05, 0x00}},
	
	// Gamma Setting C Set
	{0xca, 24, {0x00, 0x04, 0x10, 0x20, 0x2d, 0x46, 0x39, 0x24,
                0x16, 0x0b, 0x05, 0x00, 0x00, 0x04, 0x10, 0x20,
                0x2d, 0x46, 0x39, 0x24, 0x16, 0x0b, 0x05, 0x00}},
	
/* 그라데이션이 이상해짐
	// Power Setting (Charge Pump Setting)
	{0xd0, 16, {0x95, 0x06, 0x08, 0x20, 0x31, 0x04, 0x01, 0x00,
                0x08, 0x01, 0x00, 0x06, 0x01, 0x00, 0x00, 0x20}},
*/

	// VCOM Setting
	{0xd1, 4, {0x02, 0x22, 0x22, 0x33}},

	// NVM Access Control
	{0xe0, 4, {0x00, 0x00, 0x00, 0x00}},

	// Set DDB Write Control
	{0xe1, 8, {0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00}},

	// NVM Load Control
	{0xe2, 4, {0x80, 0x00, 0x00, 0x00}},

	// Write Memory Start
	{0x2C, 4, {0x00, 0x00, 0x00, 0x00}},

	    // Sleep Out
		{0x11, 4, {0x00, 0x00, 0x00, 0x00}},
	    {REGFLAG_DELAY, 120, {0}},
	    // Display ON
		{0x29, 4, {0x00, 0x00, 0x00, 0x00}},
	    {REGFLAG_DELAY, 40, {0}},

	{REGFLAG_END_OF_TABLE, 0x00, {0}}
#endif
#else		//flip main lcd auo (2011.09.09)
               {REGFLAG_DELAY, 10, {0}},
               // Sleep Out
                     {0x11, 1, {0}},
               {REGFLAG_DELAY, 120, {0}},
// MCAP
           {0xb0, 1, {0x04}},
           // Frame Memory Access and Interface Setting
           {0xb3, 4, {0x02, 0x00, 0x00, 0x00}},
           // Display Mode
           {0xb4, 1, {0x00}},
           // Panel Driving Setting
           {0xc0, 8, {0x03, 0xdf, 0x40, 0x10, 0x00, 0x01, 0x00, 0x54}},
    // Display Timing Setting for Normal Mode
           {0xc1, 5, {0x07, 0x27, 0x08, 0x08, 0x00}},
           // Source/Gate Driving Timing Setting
           {0xc4, 4, {0x37, 0x00, 0x03, 0x03}},
           // Gamma Setting A Set
           {0xc8, 24, {0x26, 0x28, 0x2a, 0x2e, 0x34, 0x42, 0x4b, 0x42,
                0x35, 0x2e, 0x26, 0x10, 0x26, 0x28, 0x2a, 0x2e,
                0x34, 0x42, 0x4b, 0x42, 0x35, 0x2e, 0x26, 0x10}},
           
           // Gamma Setting B Set
           {0xc9, 24, {0x26, 0x28, 0x2a, 0x2e, 0x34, 0x43, 0x4b, 0x41,
                0x35, 0x2e, 0x26, 0x10, 0x26, 0x28, 0x2a, 0x2e,
                0x34, 0x43, 0x4a, 0x41, 0x35, 0x2e, 0x26, 0x10}},
           
           // Gamma Setting C Set
           {0xca, 24, {0x26, 0x28, 0x2a, 0x2e, 0x34, 0x45, 0x4a, 0x41,
                0x35, 0x2e, 0x26, 0x10, 0x26, 0x28, 0x2a, 0x2e,
                0x34, 0x45, 0x49, 0x41, 0x35, 0x2e, 0x26, 0x10}},
           // Power Setting (Charge Pump Setting)
           {0xd0, 16, {0x99, 0x0a, 0x08, 0x20, 0x39, 0x04, 0x01, 0x00,
                0x08, 0x01, 0x00, 0x06, 0x01, 0x00, 0x00, 0x20}},
           // VCOM Setting
           {0xd1, 4, {0x02, 0x20, 0x14, 0x2f}},
           // Set Pixel Format
           {0x3a, 1, {0x55}},    //0x55 : 16bit, 0x66 : 18bit, 0x77 : 24bit
           // Set Address Mode
           {0x36, 1, {0x00}},
           // Set Tear On
           {0x35, 1, {0x00}},
           // Set Column Address
           {0x2a, 4, {0x00, 0x00, 0x01, 0x3f}},
           // Set Page Address
           {0x2b, 4, {0x00, 0x00, 0x01, 0xdf}},


               // Display ON
                     {0x29, 1, {0}},
             //  {REGFLAG_DELAY, 40, {0}},

	{REGFLAG_END_OF_TABLE, 0x00, {0}}

#endif
	
};


void process_display_table(struct display_table *table, unsigned int count)
{
	unsigned int i;

    for(i = 0; i < count; i++) {
		
        unsigned reg;
        reg = table[i].reg;
		
        switch (reg) {
			
            case REGFLAG_DELAY :
                mdelay(table[i].count);
				EPRINTK("%s() : delay %d msec\n", __func__, table[i].count);
                break;
				
            case REGFLAG_END_OF_TABLE :
                break;
				
            default:
                mddi_host_register_cmds_write32(reg, table[i].count, table[i].val_list, 1, 0, 0);
                //EPRINTK("%s: reg : 0x%04X, val : 0x%04X\n", __func__, reg, table[i].val_list[0]);
       	}
    }
	
}


static void mddi_auo_vsync_set_handler(msm_fb_vsync_handler_type handler,	/* ISR to be executed */
					 void *arg)
{
	boolean error = FALSE;
	unsigned long flags;

	printk("%s : handler = %x\n", 
			__func__, (unsigned int)handler);

	/* Disable interrupts */
	spin_lock_irqsave(&mddi_host_spin_lock, flags);
	/* INTLOCK(); */

	if (mddi_auo_vsync_handler != NULL) {
		error = TRUE;
	} else {
		/* Register the handler for this particular GROUP interrupt source */
		mddi_auo_vsync_handler = handler;
		mddi_auo_vsync_handler_arg = arg;
	}
	
	/* Restore interrupts */
	spin_unlock_irqrestore(&mddi_host_spin_lock, flags);
	/* MDDI_INTFREE(); */
	if (error) {
		printk("MDDI: Previous Vsync handler never called\n");
	} else {
		/* Enable the vsync wakeup */
		/* mddi_queue_register_write(INTMSK, 0x0000, FALSE, 0); */
		mddi_auo_vsync_attempts = 1;
		mddi_vsync_detect_enabled = TRUE;
	}
}

static void mddi_auo_lcd_vsync_detected(boolean detected)
{
	/* static timetick_type start_time = 0; */
//	static struct timeval start_time;
//	static boolean first_time = TRUE;
	/* unit32 mdp_cnt_val = 0; */
	/* timetick_type elapsed_us; */
//	struct timeval now;
//	uint32 elapsed_us;
//	uint32 num_vsyncs;

	mddi_vsync_detect_enabled = TRUE;;

#if 0 /* Block temporaly till vsync implement */
	mddi_queue_register_write_int(0x2C00, 0);

	if ((detected) || (mddi_auo_vsync_attempts > 5)) {
		if ((detected) || (mddi_auo_monitor_refresh_value)) {
			/* if (start_time != 0) */
			if (!first_time) {
				jiffies_to_timeval(jiffies, &now);
				elapsed_us =
					(now.tv_sec - start_time.tv_sec) * 1000000 +
					now.tv_usec - start_time.tv_usec;
				/*
				 * LCD is configured for a refresh every usecs,
				 *  so to determine the number of vsyncs that
				 *  have occurred since the last measurement
				 *  add half that to the time difference and
				 *  divide by the refresh rate.
				 */
				num_vsyncs = (elapsed_us +
						(mddi_auo_rows_per_refresh >>
						 1))/
					mddi_auo_rows_per_refresh;
				/*
				 * LCD is configured for * hsyncs (rows) per
				 * refresh cycle. Calculate new rows_per_second
				 * value based upon these new measuerments.
				 * MDP can update with this new value.
				 */
				mddi_auo_rows_per_second =
					(mddi_auo_rows_per_refresh * 1000 *
					 num_vsyncs) / (elapsed_us / 1000);
			}
			/* start_time = timetick_get();*/
			first_time = FALSE;
			jiffies_to_timeval(jiffies, &start_time);
			if (mddi_auo_report_refresh_measurements) {
				(void)mddi_queue_register_read_int(VPOS,
									&mddi_auo_curr_vpos);
				/* mdp_cnt_val = MDP_LINE_COUNT; */
			}
		}
		/* if detected = TRUE, client initiated wakeup was detected */
		if (mddi_auo_vsync_handler != NULL) {
			(*mddi_auo_vsync_handler)
				(mddi_auo_vsync_handler_arg);
			mddi_auo_vsync_handler = NULL;
		}
		mddi_vsync_detect_enabled = FALSE;
		mddi_auo_vsync_attempts = 0;
		/* need to disable the interrupt wakeup */
		if (!mddi_queue_register_write_int(INTMSK, 0x0001))
			printk("Vsync interrupt disable failed!\n");
		if (!detected) {
			/* give up after 5 failed attempts but show error */
			printk("Vsync detection failed!\n");
		} else if ((mddi_auo_monitor_refresh_value) &&
				(mddi_auo_report_refresh_measurements)) {
			printk("  Last Line Counter=%d!\n",
					mddi_auo_curr_vpos);
			/* MDDI_MSG_NOTICE("  MDP Line Counter=%d!\n",mdp_cnt_val); */
			printk("  Lines Per Second=%d!\n",
					mddi_auo_rows_per_second);
		}
		/* clear the interrupt */
		if (!mddi_queue_register_write_int(INTFLG, 0x0001))
			printk("Vsync interrupt clear failed!\n");
	} else {
		/* if detected = FALSE, we woke up from hibernation, but did not
		 * detect client initiated wakeup.
		 */
		mddi_auo_vsync_attempts++;
	}
#endif
}


static int mddi_auo_lcd_on(struct platform_device *pdev)
{
	EPRINTK("%s: started.\n", __func__);

#ifndef BOGUS
	if(is_lcd_on == -1) {
		is_lcd_on = TRUE;
		return 0;
	}
#endif//BOGUS

	if(is_lcd_on != TRUE )
	{
#ifdef BOGUS//useless
		if (system_state == SYSTEM_BOOTING && mddi_auo_pdata->initialized) {
			is_lcd_on = TRUE;
		}
#endif/*BOGUS*/

		// LCD HW Reset
		mddi_auo_lcd_panel_poweron();
		mddi_host_client_cnt_reset();
		process_display_table(mddi_auo_init_on,
			   	sizeof(mddi_auo_init_on)/sizeof(struct display_table));

		is_lcd_on = TRUE;
	}
	
//	printk ("GPIO_OUT_3: %x\n", *(uint32*)(0xF0003000+0x8));
//	printk ("GPIO_OUT_EN3: %x\n", *(uint32*)(0xF0003000+0x18));
//	printk ("GPIO_OWN_3: %x\n", *(uint32*)(0xF0003000+0x10C));
	return 0;
}

static int mddi_auo_lcd_off(struct platform_device *pdev)
{
	if(is_lcd_on != FALSE)
	{
		mddi_host_client_cnt_reset();
		process_display_table(mddi_auo_display_off, 
				sizeof(mddi_auo_display_off)/sizeof(struct display_table));

#ifdef BOGUS
		mddi_auo_lcd_panel_poweroff();
#endif/*BOGUS*/

		is_lcd_on = FALSE;
	}

	return 0;
}

#if 0  // hot_temp
static ssize_t mddi_auo_lcd_show_onoff(struct platform_device *pdev)
{
	EPRINTK("%s : strat\n", __func__);
	return 0;
}

static ssize_t mddi_auo_lcd_store_onoff(struct platform_device *pdev, struct device_attribute *attr, const char *buf, size_t count)
{
	int onoff; // = simple_strtol(buf, NULL, count);
	sscanf(buf, "%d", &onoff);

	EPRINTK("%s: onoff : %d\n", __func__, onoff);
	
	if(onoff) {
#if 0
		is_lcd_on = TRUE;
		process_display_table(mddi_auo_init_on, sizeof(mddi_auo_init_on) / sizeof(struct display_table));
#else
		if(is_lcd_on != TRUE )
		{
			mddi_auo_lcd_panel_poweron();
	
			process_display_table(mddi_auo_init_on,
				   	sizeof(mddi_auo_init_on)/sizeof(struct display_table));
	
			is_lcd_on = TRUE;
		}
#endif
	}
	else {
#if 0
		is_lcd_on = FALSE;
		process_display_table(mddi_auo_display_off, sizeof(mddi_auo_display_off) / sizeof(struct display_table));
#else
		if(is_lcd_on != FALSE)
		{
			process_display_table(mddi_auo_display_off, 
					sizeof(mddi_auo_display_off)/sizeof(struct display_table));
	
			mddi_auo_lcd_panel_poweroff();
	
			is_lcd_on = FALSE;
		}
#endif
	}

	return 0;
}


DEVICE_ATTR(lcd_onoff, 0666, mddi_auo_lcd_show_onoff, mddi_auo_lcd_store_onoff);    // hot_temp

#endif

struct msm_fb_panel_data auo_panel_data0 = {
	.on = mddi_auo_lcd_on,
	.off = mddi_auo_lcd_off,
	.set_backlight = NULL,
	.set_vsync_notifier = mddi_auo_vsync_set_handler,
};

static struct platform_device this_device_0 = {
	.name   = "mddi_auo_hvga",
	.id	= MDDI_LCD_AUO,
	.dev	= {
		.platform_data = &auo_panel_data0,
	}
};

static int mddi_auo_lcd_probe(struct platform_device *pdev)
{
//	int ret;    // hot_temp
	EPRINTK("%s: started.\n", __func__);

	if (pdev->id == 0) {
		mddi_auo_pdata = pdev->dev.platform_data;
		return 0;
	}

	msm_fb_add_device(pdev);

//	ret = device_create_file(&pdev->dev, &dev_attr_lcd_onoff);     // hot_temp

	return 0;
}

static struct platform_driver this_driver = {
	.probe  = mddi_auo_lcd_probe,
	.driver = {
		.name   = "mddi_auo_hvga",
	},
};

static int mddi_auo_lcd_init(void)
{
	int ret;
	struct msm_panel_info *pinfo;

#ifdef CONFIG_FB_MSM_MDDI_AUTO_DETECT
	u32 id;
	id = mddi_get_client_id();

	/* TODO: Check client id */

#endif
	ret = platform_driver_register(&this_driver);
	if (!ret) {
		pinfo = &auo_panel_data0.panel_info;
		EPRINTK("%s: setting up panel info.\n", __func__);
		pinfo->xres = 320;
		pinfo->yres = 480;
		pinfo->type = MDDI_PANEL;
		pinfo->pdest = DISPLAY_1;
		pinfo->mddi.vdopkt = MDDI_DEFAULT_PRIM_PIX_ATTR;
		pinfo->wait_cycle = 0;
		pinfo->bpp = 16;
	
		// vsync config
		pinfo->lcd.vsync_enable = TRUE;
		pinfo->lcd.refx100 = (mddi_auo_rows_per_second * 100) /
                        		mddi_auo_rows_per_refresh;

		pinfo->lcd.v_back_porch = 200;
		pinfo->lcd.v_front_porch = 200;
		pinfo->lcd.v_pulse_width = 30;

		pinfo->lcd.hw_vsync_mode = TRUE;
		pinfo->lcd.vsync_notifier_period = (1 * HZ);

		pinfo->bl_max = 4;
		pinfo->bl_min = 1;

		pinfo->clk_rate = 122880000;
		pinfo->clk_min =   120000000;
		pinfo->clk_max =   130000000;
#ifdef CONFIG_FB_MSM_DPSV
		pinfo->fb_num = 1;	//Temp. 1 Until support for Panning in DPSV is implemented.
#else
		pinfo->fb_num = 2;
#endif

		ret = platform_device_register(&this_device_0);
		if (ret) {
			EPRINTK("%s: this_device_0 register success\n", __func__);
			platform_driver_unregister(&this_driver);
		}
	}

	if(!ret) {
		mddi_lcd.vsync_detected = mddi_auo_lcd_vsync_detected;
	}

	process_display_table(mddi_auo_init_on,
		   	sizeof(mddi_auo_init_on)/sizeof(struct display_table));
	return ret;
}

unsigned fb_width;
unsigned fb_height;

static void mddi_auo_lcd_panel_poweron(void)
{
	//struct msm_panel_common_pdata *pdata = mddi_auo_pdata;
	struct msm_panel_lgit_pdata *pdata = mddi_auo_pdata;

	EPRINTK("%s: started.\n", __func__);

	fb_width = 320;
	fb_height = 480;

	if(pdata && pdata->gpio) {
#if 0
		gpio_direction_output(pdata->gpio, 0);
		mdelay(10);
		gpio_direction_output(pdata->gpio, 1);
		mdelay(20);
#else
		gpio_tlmm_config(GPIO_CFG(pdata->gpio, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE); // LCD_RESET_N
		
		gpio_set_value(pdata->gpio, 0);
		mdelay(10);
		gpio_set_value(pdata->gpio, 1);
		mdelay(20);
#endif
	}
}

#if 0  
static void mddi_auo_lcd_panel_poweroff(void)
{
	struct msm_panel_lgit_pdata *pdata = mddi_auo_pdata;

	EPRINTK("%s: started.\n", __func__);

	fb_width = 320;
	fb_height = 480;

	if(pdata && pdata->gpio) {
#if 0
		gpio_direction_output(pdata->gpio, 0);
		mdelay(10);
#else
		gpio_set_value(pdata->gpio, 0);
		mdelay(10);
#endif
	}
}
#endif

module_init(mddi_auo_lcd_init);
