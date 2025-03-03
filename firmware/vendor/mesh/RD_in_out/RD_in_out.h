#ifndef RD_IN_OUT_H
#define RD_IN_OUT_H


#include "proj/tl_common.h"
#include "proj/mcu/watchdog_i.h"
#include "vendor/common/user_config.h"
#include "proj_lib/rf_drv.h"
#include "proj_lib/pm.h"
#include "proj_lib/ble/blt_config.h"
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "../../common/light.h"

#include "../RD_meshdata/rd_meshdata.h"
#include "../RD_flash/rd_flash.h"
#include "../RD_hardware/rd_hardware.h"
#include "../RD_secure/rd_secure.h"
#include "../RD_log/RD_log.h"

#include "../../common/mi_api/libs/mesh_auth/mible_mesh_device.h"

#define TIME_TRAIN_FACTORY				1200
#define TIME_RESET_ALL					60000	//ms
#define TRAIN_MAX_TIME					1
#define DELAY_TRAIN_TIME				30		//s
#define GW_ADD_DEFAULT					0x0001
#define TIME_TRAINING_TOGGLE_STATE_S	2			//s
/*--------------------------- Gpio init ----------------------------------*/
#define NUM_OF_ELEMENT		 		2
#define NUM_OF_INPUT				4
#define NUM_LED						6
#define NUM_OF_RELAY				NUM_OF_ELEMENT

#define DETECT_ZERO_PIN				GPIO_PA2

#define INPUT_1						GPIO_PA1
#define INPUT_2						GPIO_PB7
#define INPUT_3						GPIO_PC0
#define INPUT_4						GPIO_PC1

#define ADC_PIN						GPIO_PB6

#define OUTPUT_1					GPIO_PC2
#define OUTPUT_2					GPIO_PC3


#define BUTTON_RESET 				GPIO_PD2

#define LED_1						GPIO_PB1
#define LED_2						GPIO_PB5
#define LED_3						GPIO_PC4
#define LED_4						GPIO_PB4
#define LED_5						GPIO_PD4
#define LED_6						GPIO_PD3


#define BUZZER						GPIO_PA7
#define RESET_TOUCH_PIN				GPIO_PA1

#define ARR_INPUT_PIN				{INPUT_1,INPUT_2,INPUT_3,INPUT_4}
#define ARR_OUTPUT_PIN 				{OUTPUT_1,OUTPUT_2}

#define ARR_RELAY					ARR_OUTPUT_PIN

#define ARR_LED						{LED_1,LED_2,LED_3,LED_4,LED_5,LED_6}

#define INPUT_READ(i)				gpio_read(Input_Array[i])
#define RD_IN_READ(i)				INPUT_READ(i)
#define BIT_INPUT(i)				(Input_Array[i] && 0xff)


#define OUTPUT_WRITE(i,t)           gpio_write(Output_Array[i], t)
#define LED_WRITE(i,t)				gpio_write(arr_led[i],t)
/*--------------------------- type product ----------------------------------*/
#define TYPE_MAINDEVICE 	 		0x02
#define	TYPE_SWITCH			 		0x02

#define	TYPE_TOUCH_SWITCH	 		0x02

/*--------------------------- config ----------------------------------*/
#define 	UART_ON 					1
#define 	CLOCK_TIME_OUT_PRESS_K9B_MS	500
#define		TIME_OUT_RESET				3000					// 4000 loop break pressResetCount
#define 	TSS_DEFAULT					0x0005
#define 	DELTA_ADC					200


//#define LED_NUM_BLINK_SETTING_INPUT			3
//#define LED_NUM_BLINK_SET_LINKED			3
//#define LED_NUM_BLINK_UNPROV				5
//#define LED_NUM_BLINK_POW_UP_OUTPUT			3

enum
{
	LED_NUM_BLINK_SETTING_INPUT = 2,
	LED_NUM_BLINK_SET_LINKED = 2,
	LED_NUM_SET_SENCE = 2,
	LED_NUM_BLINK_UNPROV = 5,
	LED_NUM_BLINK_PROVISON_SUC = 3,
	LED_NUM_BLINK_POW_UP_OUTPUT =2,
	LED_NUM_RAISING_INPUT	= 1,
	LED_NUM_ADC_CALIB_SUCCESS = 3,
	LED_NUM_START_CALIB_ADC = 3,
};

enum
{
	TIME_100MS	= 1,
	TIME_200MS	= 2,
	TIME_400MS	= 4,
	TIME_500MS	= 5,
};
//#define		COUNT_CHECK_ADC				10
/*--------------------------- enum ----------------------------------*/
typedef enum {
	Relay_On = 0x01U, Relay_Off = 0x00U
} Relay_Stt_Type;

typedef enum {
	RD_EN = 0x01, RD_DIS = 0x00
} RD_En_type;

typedef enum {
	RD_Success = 0x01, RD_Failure = 0x00
} RD_Result_Type;

typedef enum {
	RD_PowUpOff = 0x00, RD_PowUpOn = 0x01, RD_PowUpStore = 0x02, RD_POW_SET_MAX = RD_PowUpStore,
} RD_PowUp_Type;

typedef enum {
	Button_None = 0x00U, Button_Press = 0x01U, Button_Keeping = 0x02U
} Button_Stt_Type;

typedef struct {
	uint8_t OTA_Flag; // check in OTA mode. Note
} Sw_Working_Stt_Str;

enum
{
	DEFAULT_STATE = 0,
	SYNC_PRESS_STATE = DEFAULT_STATE,    // dong bo trang thai giu		// dao trang thai giu
	PRESS_RELEASE_STATE,		//
	INPUT_STATE_MAX = PRESS_RELEASE_STATE,
};

enum
{
	MODE_ON = 0,
	MODE_OFF,
	MODE_PULSING,
	MODE_MAX = MODE_PULSING,
};

enum
{
	TYPE_IN = 1,
	TYPE_ADC_GREATER,
	TYPE_ADC_LOWER
};

enum
{
	LED_IN_1 = 0,
	LED_IN_2,
	LED_IN_3,
	LED_IN_4,
	LED_OUT_1,
	LED_OUT_2,
	LED_OUT = LED_OUT_1,
	LED_RELAY_1 = LED_OUT_1,
	LED_RELAY_2 = LED_OUT_2,
};

enum
{
	MODE_NO_TRAIN =0,												// mode normal: no training, unprov, proved
	CATCH_UP_MESS_TRAIN_FAC,
	PRE_TRAIN_FAC = CATCH_UP_MESS_TRAIN_FAC,
	DOING_TRAIN_FAC,
	TRAIN_MODE_MAX = DOING_TRAIN_FAC,
};

enum
{
	MODE_NORMAL = 0,
	MODE_CALIB
};
/*--------------------------- Variable ----------------------------------*/

extern uint8_t Kick_all_Flag;
extern Sw_Working_Stt_Str Sw_Working_Stt_Val;
extern u32 arr_led[];
extern uint32_t Input_Array[];
extern u8 Train_Factory;

/*--------------------------- Function ----------------------------------*/

void RD_mod_in_out_init(void);
void RD_mod_in_out_loop(void);
void rd_init_input();

void RD_mod_in_out_factory_reset();
void RD_mod_io_gw_reset(void);
void RD_ScanKickAll(void);
int rd_rsp_stt_relay(int Light_index, u8 OnOff_Set, uint16_t GW_Add_Rsp_G_onoff);
void rd_module_io_handle_input_onoff(void);

u16 get_adc_value();
u8 get_status_input(u8 idx);

void reset_detect_input(u8 id);
void reset_all_detect_input(u8 id);

void rd_check_adc();
void rd_init_adc();

void rd_init_queue_led();
void rd_blink_led(u8 idx_led, u8 num,u8 time_delay_100ms);
void rd_on_off_led(u8 idx_led, u8 status);


void rd_init_queue_relay();
int rd_init_onoff_relay(u8 stt,u8 id_relay);
int rd_onoff_relay(u8 stt,u8 id_relay, int rsp,u8 src);    //0: local, 1: app
void rd_toggle_relay(uint8_t id_ele, int rsp);

#endif
