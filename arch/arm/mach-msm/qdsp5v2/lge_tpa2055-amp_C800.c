/* arch/arm/mach-msm/qdsp5v2/lge_tpa2055-amp.c
 *
 * Copyright (C) 2010 LGE, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/device.h>
#include <asm/gpio.h>
#include <asm/system.h>
#include <linux/jiffies.h>
#include <linux/i2c.h>
#include <linux/err.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <asm/ioctls.h>
#include <mach/debug_mm.h>
#include <linux/slab.h>
#include "mach/qdsp5v2/lge_tpa2055-amp.h"

#define MODULE_NAME	"lge_tpa2055"



char amp_cal_data[AMP_CAL_MAX] = {IN1GAIN_0DB, HPL_VOL_M8DB,\
                                                                                    IN2GAIN_0DB, SPK_VOL_M3DB,\
                                                                                    IN1GAIN_0DB, HPL_VOL_0DB,\
                                                                                    IN1GAIN_6DB, HPL_VOL_M3DB,\
                                                                                    IN1GAIN_12DB, HPR_VOL_M21DB, SPK_VOL_0DB,\
                                                                                    IN2GAIN_6DB, SPK_VOL_M4DB};


bool first_boot = 1;

static uint32_t msm_snd_debug = 1;
module_param_named(debug_mask, msm_snd_debug, uint, 0664);


struct amp_data {
	struct i2c_client *client;
};

static struct amp_data *_data;

int ReadI2C(char reg, char *ret)
{

	unsigned int err;
	unsigned char buf = reg;

	struct i2c_msg msg[2] = {
		{ _data->client->addr, 0, 1, &buf },
		{ _data->client->addr, I2C_M_RD, 1, ret}
	};

	err = i2c_transfer(_data->client->adapter, msg, 2);
	if (err < 0)
		dev_err(&_data->client->dev, "i2c read error\n");

	return 0;

}

int WriteI2C(char reg, char val)
{
	int	err;
	unsigned char    buf[2];
	struct i2c_msg	msg = { _data->client->addr, 0, 2, buf };

	buf[0] = reg;
	buf[1] = val;

	err = i2c_transfer(_data->client->adapter, &msg, 1);
	if (err < 0) {
		dev_err(&_data->client->dev, "%s: i2c write error (%d)\n", MODULE_NAME, err);
		// return -EIO;
		return err;
	}
	else {
		return 0;
	}
}


void set_amp_PowerDown(void)
{
	int fail=0;
//	fail |= WriteI2C(SUBSYSTEM_CONTROL, (BYPASS | SWS));

/*
fail |= WriteI2C(0x06, 0x7B);

msleep(5);
fail |= WriteI2C(0x06, 0x7A);

msleep(5);
fail |= WriteI2C(0x06, 0x79);
msleep(5);

fail |= WriteI2C(0x06, 0x78);
msleep(5);

fail |= WriteI2C(0x06, 0x77);
msleep(5);

fail |= WriteI2C(0x06, 0x76);
msleep(5);
fail |= WriteI2C(0x06, 0x75);
msleep(5);


fail |= WriteI2C(0x06, 0x74);
msleep(5);

fail |= WriteI2C(0x06, 0x73);
msleep(5);


fail |= WriteI2C(0x06, 0x72);
msleep(5);

fail |= WriteI2C(0x06, 0x71);
msleep(5);


fail |= WriteI2C(0x06, 0x70);
msleep(5);
fail |= WriteI2C(0x06, 0x6D);
msleep(5);
fail |= WriteI2C(0x06, 0x6A);
msleep(5);
fail |= WriteI2C(0x06, 0x69);
msleep(5);
fail |= WriteI2C(0x06, 0x68);
msleep(5);
fail |= WriteI2C(0x06, 0x67);
msleep(5);
fail |= WriteI2C(0x06, 0x66);
msleep(5);
fail |= WriteI2C(0x06, 0x65);
msleep(5);
fail |= WriteI2C(0x06, 0x64);
msleep(5);
fail |= WriteI2C(0x06, 0x63);
msleep(5);
fail |= WriteI2C(0x06, 0x62);
msleep(5);
fail |= WriteI2C(0x06, 0x61);
fail |= WriteI2C(0x06, 0x60);
*/


//special sequence by Alfredo from TI 

fail |= WriteI2C(0x00, 0x18);

fail |= WriteI2C(0x06, 0x00);
fail |= WriteI2C(0x07, 0x00);
fail |= WriteI2C(0x04, 0x00);


msleep(10);

fail |= WriteI2C(0x03, 0x00);
fail |= WriteI2C(0x05, 0x20);
fail |= WriteI2C(0x03, 0x00);
fail |= WriteI2C(0x05, 0x00);


/*
	fail |= WriteI2C(SPEAKER_VOLUME, SPK_EN);
	fail |= WriteI2C(SPEAKER_OUTPUT, SPKOUT_MUTE);
	fail |= WriteI2C(SPEAKER_VOLUME, ~SPK_EN & SPK_VOL_M60DB);

	fail |= WriteI2C(HP_LEFT_VOLUME, HPL_EN | HP_TRACK | HPL_VOL_M60DB);
	fail |= WriteI2C(HP_LEFT_VOLUME, HPL_VOL_M60DB);
	fail |= WriteI2C(HP_RIGHT_VOLUME, HPR_VOL_M60DB);
	fail |= WriteI2C(HEADPHONE_OUTPUT, HPOUT_MUTE);

	fail |= WriteI2C(SUBSYSTEM_CONTROL, (BYPASS | SWS));
*/

    MM_INFO("1 set_amp_PowerDown() %d\n", fail);
}
EXPORT_SYMBOL(set_amp_PowerDown);
void set_amp_headset_mono_voice(void)
{
	int fail=0;
	fail |= WriteI2C(SUBSYSTEM_CONTROL, (~SWS & ~BYPASS & ~SSM_EN));
	fail |= WriteI2C(INPUT_CONTROL, (amp_cal_data[HEADSET_VOICE_INPUT] | IN1_DIFF)); 				//Modify for desired IN gain 
	fail |= WriteI2C(HP_LEFT_VOLUME, (HPL_VOL_M60DB | HPL_EN | HP_TRACK));
	fail |= WriteI2C(HP_RIGHT_VOLUME, (HPR_VOL_M60DB | HPR_EN));
	fail |= WriteI2C(HEADPHONE_OUTPUT, HPOUT_IN1);
	fail |= WriteI2C(HP_LEFT_VOLUME, (amp_cal_data[HEADSET_VOICE_OUTPUT]  | HPL_EN | HP_TRACK));			//Modify for desired HP gain

	fail |= WriteI2C(SPEAKER_OUTPUT, SPKOUT_MUTE);
    MM_INFO("2 set_amp_headset_mono_voice() %d\n", fail);
}
EXPORT_SYMBOL(set_amp_headset_mono_voice);

void set_amp_speaker_stereo_voice(void)
{
	int fail=0;
	fail |= WriteI2C(SUBSYSTEM_CONTROL, (~SWS & ~BYPASS & ~SSM_EN));
	fail |= WriteI2C(INPUT_CONTROL, (amp_cal_data[SPEAKER_VOICE_INPUT]  | IN2_SE)); 							//Modify for desired IN gain 
	fail |= WriteI2C(SPEAKER_VOLUME, (SPK_EN | amp_cal_data[SPEAKER_VOICE_OUTPUT] ));							//Modify for desired SP gain
	fail |= WriteI2C(SPEAKER_OUTPUT, SPKOUT_IN2);

    MM_INFO("3 set_amp_speaker_stereo_voice() %d\n", fail);
}
EXPORT_SYMBOL(set_amp_speaker_stereo_voice);

void set_amp_tty(void)
{
	int fail=0;
	fail |= WriteI2C(SUBSYSTEM_CONTROL, (~SWS & ~BYPASS & ~SSM_EN));
	fail |= WriteI2C(INPUT_CONTROL, (amp_cal_data[TTY_INPUT]  | IN1_DIFF)); 				//Modify for desired IN gain 
	fail |= WriteI2C(HP_LEFT_VOLUME, (HPL_VOL_M60DB | HPL_EN | HP_TRACK));
	fail |= WriteI2C(HP_RIGHT_VOLUME, (HPR_VOL_M60DB | HPR_EN));
	fail |= WriteI2C(HEADPHONE_OUTPUT, HPOUT_IN1);
	fail |= WriteI2C(HP_LEFT_VOLUME, (amp_cal_data[TTY_OUTPUT]  | HPL_EN | HP_TRACK));			//Modify for desired HP gain

	fail |= WriteI2C(SPEAKER_OUTPUT, SPKOUT_MUTE);

    MM_INFO("4 set_amp_tty() %d\n", fail);
}
EXPORT_SYMBOL(set_amp_tty);

void set_amp_headset_stereo_audio(void)
{
	int fail=0;
/*
	fail |= WriteI2C(0x01, 0x04);
	fail |= WriteI2C(0x06, 0x60);
	fail |= WriteI2C(0x07, 0x20);
	fail |= WriteI2C(0x00, 0xE3);
	fail |= WriteI2C(0x06, 0x7C);
	fail |= WriteI2C(0x04, 0x20);
	
*/


	fail |= WriteI2C(INPUT_CONTROL, (amp_cal_data[HEADSET_AUDIO_INPUT]  | IN1_SE)); 				//Modify for desired IN gain 
	fail |= WriteI2C(HP_LEFT_VOLUME, (HPL_VOL_M60DB | HPL_EN | HP_TRACK));
	fail |= WriteI2C(HP_RIGHT_VOLUME, (HPR_VOL_M60DB | HPR_EN));
	fail |= WriteI2C(SUBSYSTEM_CONTROL, 0xE3);
	fail |= WriteI2C(HP_LEFT_VOLUME, (amp_cal_data[HEADSET_AUDIO_OUTPUT]  | HPL_EN | HP_TRACK));			//Modify for desired HP gain
	fail |= WriteI2C(HEADPHONE_OUTPUT, HPOUT_IN1);



//	fail |= WriteI2C(SPEAKER_OUTPUT, SPKOUT_MUTE);

    MM_INFO("5 set_amp_headset_stereo_audio() %d\n", fail);
}
EXPORT_SYMBOL(set_amp_headset_stereo_audio);

void set_amp_headset_speaker_audio(void)
{
	int fail=0;
	fail |= WriteI2C(INPUT_CONTROL, (amp_cal_data[HEADSET_SPEAKER_INPUT]  | IN1_SE)); 				//Modify for desired IN gain 
	fail |= WriteI2C(HP_LEFT_VOLUME, (HPL_VOL_M60DB | HPL_EN | HP_TRACK));
	fail |= WriteI2C(HP_RIGHT_VOLUME, (HPR_VOL_M60DB | HPR_EN));
	fail |= WriteI2C(SUBSYSTEM_CONTROL, 0xE3);
	fail |= WriteI2C(HP_LEFT_VOLUME, (amp_cal_data[HEADSET_SPEAKER_OUTPUT_HEADSET]  | HPL_EN | HP_TRACK));			//Modify for desired HP gain
	fail |= WriteI2C(HEADPHONE_OUTPUT, HPOUT_IN1);

	fail |= WriteI2C(SPEAKER_VOLUME, (SPK_EN | amp_cal_data[HEADSET_SPEAKER_OUTPUT_SPEAKER]));							//Modify for desired SP gain
	fail |= WriteI2C(SPEAKER_OUTPUT, SPKOUT_IN1);

    MM_INFO(" 6 set_amp_headset_speaker_audio() %d\n", fail);
}
EXPORT_SYMBOL(set_amp_headset_speaker_audio);

void set_amp_speaker_stereo_audio(void)
{
	int fail=0;
	fail |= WriteI2C(SUBSYSTEM_CONTROL, (~SWS & ~BYPASS & ~SSM_EN));
	fail |= WriteI2C(INPUT_CONTROL, (amp_cal_data[SPEAKER_AUDIO_INPUT] | IN2_SE)); 							//Modify for desired IN gain 
	fail |= WriteI2C(SPEAKER_VOLUME, (SPK_EN | amp_cal_data[SPEAKER_AUDIO_OUTPUT]));							//Modify for desired SP gain
	fail |= WriteI2C(SPEAKER_OUTPUT, SPKOUT_IN2);

    MM_INFO("7 set_amp_speaker_stereo_voice() %d\n", fail);
}
EXPORT_SYMBOL(set_amp_speaker_stereo_audio);

static int flip_amp_ctl_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct amp_data *data;
	struct i2c_adapter* adapter = client->adapter;
	int err;
	
	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_WORD_DATA)){
		err = -EOPNOTSUPP;
		return err;
	}

	if (msm_snd_debug & 1)
		printk(KERN_INFO "%s()\n", __FUNCTION__);
	
	data = kzalloc(sizeof (struct amp_data), GFP_KERNEL);
	if (NULL == data) {
			return -ENOMEM;
	}
	_data = data;
	data->client = client;
	i2c_set_clientdata(client, data);
	
	if (msm_snd_debug & 1)
		printk(KERN_INFO "%s chip found\n", client->name);
	
	set_amp_PowerDown();
	return 0;
}

static int flip_amp_ctl_remove(struct i2c_client *client)
{
	struct amp_data *data = i2c_get_clientdata(client);
	kfree (data);
	
	printk(KERN_INFO "%s()\n", __FUNCTION__);
	i2c_set_clientdata(client, NULL);
	return 0;
}


static struct i2c_device_id flip_amp_idtable[] = {
	{ "lge_tpa2055", 1 },
};

static struct i2c_driver flip_amp_ctl_driver = {
	.probe = flip_amp_ctl_probe,
	.remove = flip_amp_ctl_remove,
	.id_table = flip_amp_idtable,
	.driver = {
		.name = MODULE_NAME,
	},
};


static int __init Tpa2055_amp_ctl_init(void)
{
	return i2c_add_driver(&flip_amp_ctl_driver);	
}

static void __exit Tpa2055_amp_ctl_exit(void)
{
	return i2c_del_driver(&flip_amp_ctl_driver);
}

module_init(Tpa2055_amp_ctl_init);
module_exit(Tpa2055_amp_ctl_exit);

MODULE_DESCRIPTION("Bryce Amp Control");
MODULE_AUTHOR("Kim EunHye <ehgrace.kim@lge.com>");
MODULE_LICENSE("GPL");
