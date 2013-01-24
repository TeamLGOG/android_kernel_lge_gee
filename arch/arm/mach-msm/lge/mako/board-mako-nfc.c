#include <linux/nfc/pn544_lge.h>

static struct pn544_i2c_platform_data pn544_i2c_platform_data[] = {
        .irq_gpio = NFC_GPIO_IRQ,
        .ven_gpio = NFC_GPIO_VEN,
        .firm_gpio = 0xFF,
};

#define NFC_I2C_BOARD_INFO						\
	{								\
    		I2C_BOARD_INFO(PN544_DRV_NAME, NFC_I2C_SLAVE_ADDR),	\
	    	.type = PN544_DRV_NAME,					\
		.irq = MSM_GPIO_TO_INT(NFC_GPIO_IRQ),			\
		.platform_data = &pn544_i2c_platform_data,		\
	}
