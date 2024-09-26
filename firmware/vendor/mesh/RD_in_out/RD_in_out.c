#include "RD_in_out.h"
//#include "../driver/8258/gpio_8258.h"

#include "tl_common.h"
#include "proj/mcu/watchdog_i.h"
#include "vendor/common/user_config.h"
#include "proj_lib/rf_drv.h"
#include "proj_lib/pm.h"
#include "proj_lib/ble/blt_config.h"
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/sig_mesh/app_mesh.h"

#define 	NUM_OF_ELEMENT		 2

#define 	DETECT_ZERO_PIN				GPIO_PC2

#define		BT1_PIN						GPIO_PA1 //GPIO_PC3
#define		BT2_PIN						GPIO_PC3 //GPIO_PA1

#define		RELAY1_PIN					GPIO_PC0 //GPIO_PC4
#define		RELAY2_PIN					GPIO_PC4 //GPIO_PC0
//	#define 	RELAY3_PIN					GPIO_PB7

#define 	LED1_PIN					GPIO_PB1 //GPIO_PC1
#define 	LED1_PWM					PWM4_ID //PWM0_ID
#define 	LED2_PIN					GPIO_PC1 //GPIO_PB1
#define 	LED2_PWM					PWM0_ID //PWM4_ID

void init_in_out_pin() {
	gpio_set_func(LED1_PIN, AS_GPIO);
	gpio_set_output_en(LED1_PIN, 1);
	gpio_set_func(LED2_PIN, AS_GPIO);
	gpio_set_output_en(LED2_PIN, 1);
	gpio_write(LED1_PIN,1);
	gpio_write(LED2_PIN,0);
	gpio_set_interrupt_init(BT1_PIN, PM_PIN_UP_DOWN_FLOAT, 1, FLD_IRQ_GPIO_EN);
	gpio_set_interrupt_init(BT2_PIN, PM_PIN_UP_DOWN_FLOAT, 1, FLD_IRQ_GPIO_EN);
}



