/* Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
 * Copyright (c) 2012, LGE Inc.
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
#include <linux/gpio_event.h>

#include <mach/vreg.h>
#include <mach/rpc_server_handset.h>
#include <mach/board.h>

/* keypad */
#include <linux/regulator/gpio-regulator.h>
#include <linux/mfd/pm8xxx/pm8921.h>

/* i2c */
#include <linux/regulator/consumer.h>
#include <linux/i2c.h>
#include <linux/earlysuspend.h>
#include <linux/input/touch_synaptics_rmi4_i2c.h>
#include <linux/input/lge_touch_core.h>
#include <mach/board_lge.h>
#include "board-mako.h"

/* TOUCH GPIOS */
#define SYNAPTICS_TS_I2C_SDA                 	8
#define SYNAPTICS_TS_I2C_SCL                 	9
#define SYNAPTICS_TS_I2C_INT_GPIO            	6
#define TOUCH_RESET                             33

#define TOUCH_FW_VERSION                        1

/* touch screen device */
#define APQ8064_GSBI3_QUP_I2C_BUS_ID            3

int synaptics_t1320_power_on(int on)
{
	int rc = -EINVAL;
	static struct regulator *vreg_l15 = NULL;
	static struct regulator *vreg_l22 = NULL;

	/* 3.3V_TOUCH_VDD, VREG_L15: 2.75 ~ 3.3 */
	if (!vreg_l15) {
		vreg_l15 = regulator_get(NULL, "touch_vdd");
		if (IS_ERR(vreg_l15)) {
			pr_err("%s: regulator get of 8921_l15 failed (%ld)\n",
					__func__,
			       PTR_ERR(vreg_l15));
			rc = PTR_ERR(vreg_l15);
			vreg_l15 = NULL;
			return rc;
		}
	}
	/* 1.8V_TOUCH_IO, VREG_L22: 1.7 ~ 2.85 */
	if (!vreg_l22) {
		vreg_l22 = regulator_get(NULL, "touch_io");
		if (IS_ERR(vreg_l22)) {
			pr_err("%s: regulator get of 8921_l22 failed (%ld)\n",
					__func__,
			       PTR_ERR(vreg_l22));
			rc = PTR_ERR(vreg_l22);
			vreg_l22 = NULL;
			return rc;
		}
	}

	rc = regulator_set_voltage(vreg_l15, 3300000, 3300000);
	rc |= regulator_set_voltage(vreg_l22, 1800000, 1800000);
	if (rc < 0) {
		printk(KERN_INFO "[Touch D] %s: cannot control regulator\n",
		       __func__);
		return rc;
	}

	if (on) {
		printk("[Touch D]touch enable\n");
		regulator_enable(vreg_l15);
		regulator_enable(vreg_l22);
	} else {
		printk("[Touch D]touch disable\n");
		regulator_disable(vreg_l15);
		regulator_disable(vreg_l22);
	}

	return rc;
}

static struct touch_power_module touch_pwr = {
	.use_regulator = 0,
	.vdd = "8921_l15",
	.vdd_voltage = 3300000,
	.vio = "8921_l22",
	.vio_voltage = 1800000,
	.power = synaptics_t1320_power_on,
};

static struct touch_device_caps touch_caps = {
	.button_support = 1,
	.y_button_boundary = 0,
	.number_of_button = 3,
	.button_name = {KEY_BACK,KEY_HOMEPAGE,KEY_MENU},
	.button_margin	= 0,
	.is_width_supported = 1,
	.is_pressure_supported = 1,
	.is_id_supported = 1,
	.max_width = 15,
	.max_pressure = 0xFF,
	.max_id = 10,
	.lcd_x = 768,
	.lcd_y = 1280,
	.x_max = 1536-1,
	.y_max = 2560-1,
};

static struct touch_operation_role touch_role = {
	.operation_mode = INTERRUPT_MODE,
	.key_type = TOUCH_HARD_KEY,
	.report_mode = REDUCED_REPORT_MODE,
	.delta_pos_threshold = 1,
	.orientation = 0,
	.report_period	= 10000000,
	.booting_delay = 400,
	.reset_delay = 5,
	.suspend_pwr = POWER_OFF,
	.resume_pwr = POWER_ON,
	.jitter_filter_enable = 0,
	.jitter_curr_ratio = 30,
	.accuracy_filter_enable = 1,
	.irqflags = IRQF_TRIGGER_FALLING,
	.show_touches = 0,
	.pointer_location = 0,
};

static struct touch_platform_data mako_ts_data = {
	.int_pin = SYNAPTICS_TS_I2C_INT_GPIO,
	.reset_pin = TOUCH_RESET,
	.maker = "Synaptics",
	.caps = &touch_caps,
	.role = &touch_role,
	.pwr = &touch_pwr,
};

static struct i2c_board_info synaptics_ts_info[] = {
	[0] = {
		I2C_BOARD_INFO(LGE_TOUCH_NAME, 0x20),
		.platform_data = &mako_ts_data,
		.irq = MSM_GPIO_TO_INT(SYNAPTICS_TS_I2C_INT_GPIO),
	},
};





       rc = gpio_request(TOUCH_RESET, "rmi4_reset_pin");
       if (rc) {
       	pr_err("%s: Failed to get rmi4_reset_pin %d. Code: %d.",
       						__func__, TOUCH_RESET, rc);
       	return rc;
       }            
       rc = gpio_direction_output(TOUCH_RESET, 1);



	//Touch IC Power On
	vreg_l15 = regulator_get(NULL, "8921_l15");   //3.3V_TOUCH_VDD, VREG_L15: 2.75 ~ 3.3
	if (IS_ERR(vreg_l15)) {
		pr_err("%s: regulator get of 8921_l15 failed (%ld)\n", __func__, PTR_ERR(vreg_l15));
		rc = PTR_ERR(vreg_l15);
		return rc;
	}
	
	pr_err("%s: [Touch D] S2 \n",__func__);
	vreg_l22 = regulator_get(NULL, "8921_l22");   //1.8V_TOUCH_IO, VREG_L22: 1.7 ~ 2.85
	if (IS_ERR(vreg_l22)) {
		pr_err("%s: regulator get of 8921_l22 failed (%ld)\n", __func__, PTR_ERR(vreg_l22));
		rc = PTR_ERR(vreg_l22);
		return rc;
	}
	pr_err("%s: [Touch D] S3 \n",__func__);
	rc = regulator_set_voltage(vreg_l15, 3300000, 3300000);
	rc = regulator_set_voltage(vreg_l22, 1800000, 1800000);


	regulator_enable(vreg_l15);
	regulator_enable(vreg_l22);


	if (configure) {
		rc = gpio_request(data->gpio_number, "rmi4_attn");
		if (rc) {
			pr_err("%s: Failed to get attn gpio %d. Code: %d.",
								__func__, data->gpio_number, rc);
			return rc;
		}

		gpio_tlmm_config(GPIO_CFG(data->gpio_number, 0, GPIO_CFG_INPUT,
								GPIO_CFG_PULL_UP, GPIO_CFG_6MA),GPIO_CFG_ENABLE);

		rc = gpio_direction_input(data->gpio_number);
		if (rc) {
				pr_err("%s: Failed to setup attn gpio %d. Code: %d.",
				__func__, data->gpio_number, rc);
				gpio_free(data->gpio_number);
		}
	} else {
			pr_warn("%s: No way to deconfigure gpio %d.",
			__func__, data->gpio_number);
	}

	if(rc < 0){
			printk(KERN_INFO "[Touch D] %s: cannot request GPIO\n", __func__);
			return rc;
	}
	
	printk("[Touch D]synaptics_touchpad_gpio_setup -- \n");

	return rc;
}

#define AXIS_ALIGNMENT { \
	.swap_axes = false, \
	.flip_x = false, \
	.flip_y = false, \
	.clip_X_low  = 0, \
	.clip_Y_low  = 0, \
	.offset_X = 0,  \
	.offset_Y = 0, \
}

#define TM2144_ADDR 0x20
#define TM2144_ATTN 6

static unsigned char tm2144_button_codes[] = {KEY_BACK,KEY_HOMEPAGE,KEY_MENU}; 
static struct rmi_f1a_button_map tm2144_button_map = {
	.nbuttons = ARRAY_SIZE(tm2144_button_codes),
	.map = tm2144_button_codes,
};

static struct syna_gpio_data tm2144_gpiodata = {
	.gpio_number = TM2144_ATTN,
	.gpio_name = "ts_int.gpio_6",
};

static struct rmi_device_platform_data tm2144_platformdata = {
	.driver_name = "rmi_generic",
	.sensor_name = "TM2144",
	.attn_gpio = TM2144_ATTN,
	.attn_polarity = RMI_ATTN_ACTIVE_LOW,
	.gpio_data = &tm2144_gpiodata,
	.gpio_config = synaptics_touchpad_gpio_setup,
	.axis_align = AXIS_ALIGNMENT,
	.f1a_button_map = &tm2144_button_map,
	.reset_delay_ms = 100,
};


static struct i2c_board_info synaptics_ds4_rmi_info[] = {
     [0] = {
         I2C_BOARD_INFO("rmi_i2c", TM2144_ADDR),
        .platform_data = &tm2144_platformdata,
     },
};


void __init apq8064_init_input(void)
{

	printk(KERN_INFO "[Touch D] %s: NOT DCM KDDI, reg synaptics driver \n", __func__);
	i2c_register_board_info(APQ8064_GSBI3_QUP_I2C_BUS_ID,
		&synaptics_ts_info[0], 1);

}

