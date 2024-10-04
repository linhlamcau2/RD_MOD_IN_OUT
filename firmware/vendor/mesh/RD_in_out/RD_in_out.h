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

#define TIME_TRAIN_FACTORY			1200
#define TIME_RESET_ALL				60000	//ms
#define TRAIN_MAX_TIME				1
#define DELAY_TRAIN_TIME			90		//s
#define GW_ADD_DEFAULT				0x0001
#define TIME_CYCLE_RST_TOUCH_S		3600   	// 3600s 1hour
/*--------------------------- Time wait detect relay----------------------------------*/
#define TIME_DETECT_ON				6000	//us
#define TIME_DETECT_OFF				4800	//us
/*-------------------------------------end detect time---------------------------------*/
#define TOUCH_ACTIVE_POW			0 // 1 is origin ver
#define TOUCH_NON_ACTIVE_POW		1

#define LED_DIM_OFF_DF 				20
#define	LED_DIM_ON_DF				90

/*--------------------------- Gpio init ----------------------------------*/
#define NUM_OF_ELEMENT		 		2
#define NUM_OF_INPUT				4

#define	BT1_PIN						GPIO_PA1
#define	BT2_PIN						GPIO_PC3

#define INPUT_1						BT1_PIN
#define INPUT_2						BT1_PIN
#define INPUT_3						BT1_PIN
#define INPUT_4						BT1_PIN

#define OUTPUT_1					GPIO_PB1
#define OUTPUT_2					GPIO_PC1

#define RESET_TOUCH_PIN				GPIO_PA1

#define ARR_INPUT_PIN				{INPUT_1,INPUT_1,INPUT_1,INPUT_1}
#define ARR_OUTPUT_PIN 				{OUTPUT_1,OUTPUT_2}

#define INPUT_READ(i)				gpio_read(Input_Array[i])

//#define OUTPUT_EXPAND(i)            OUTPUT_EXPAND_INDIRECT1(i)
//#define OUTPUT_EXPAND_INDIRECT1(i)            OUTPUT_EXPAND_INDIRECT(i)
//#define OUTPUT_EXPAND_INDIRECT(i)   OUTPUT_##i
//#define OUTPUT_WRITE(i,t)           OUTPUT_WRITE_SUB(OUTPUT_EXPAND(i),t)
//#define OUTPUT_WRITE_SUB(i,t)       gpio_write(i,t)
#define OUTPUT_WRITE(i,t)            gpio_write(Output_Array[i], t)
/*--------------------------- type product ----------------------------------*/
#define TYPE_MAINDEVICE 	 		0x02
#define	TYPE_SWITCH			 		0x02

#define	TYPE_TOUCH_SWITCH	 		0x02

/*--------------------------- config ----------------------------------*/
#define 	UART_ON 					1
#define 	CLOCK_TIME_OUT_PRESS_K9B_MS	500
#define		TIME_OUT_RESET				3000					// 4000 loop break pressResetCount
#define		COUNT_DETECT_POWER_OFF		7						// 2ms (timer1 interrupt) * 6 = 12ms zero
#define		NUM_CHECK_DETECH_MAX		150
#define 	DETECT_ZERO_OFF				0
#define		DETECT_ZERO_BY_ADC			1
#define 	DETECT_ZERO_BY_FAILING		2

#define 	METHOD_DETECT_ZERO			DETECT_ZERO_BY_FAILING //DETECT_ZERO_BY_FAILING
#define 	TIME_HOLD_BT				(500)

#define 	TSS_DEFAULT					0x0005

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
	RD_PowUpOff = 0x00, RD_PowUpOn = 0x01, RD_PowUpStore = 0x02
} RD_PowUp_Type;

typedef enum {
	Button_None = 0x00U, Button_Press = 0x01U, Button_Keeping = 0x02U
} Button_Stt_Type;

typedef struct {
	uint32_t countLastOfMacID[MAX_NUM_K9ONOFF];
} K9bOnOff_countLast_st;

typedef struct {
	uint32_t countLastOfMacID[MAX_NUM_K9BHC];
} K9bHc_countLast_st;
typedef struct {
	uint8_t OTA_Flag; // check in OTA mode. Note

} Sw_Working_Stt_Str;

typedef enum {
	NONE_FUN = 0x00, TOGGLE_FUN = 0x01, OFF_FUN = 0x02, ON_FUN = 0x03
} K9B_Function_Button_t;

typedef struct {
	uint8_t Pair_K9BOnOff_Flag;
	uint8_t Button_K9BOnOff_Pair;
	uint8_t Pair_K9BHc_Flag;
	uint16_t Add_K9B_HCSet; // HC set add for K9B remote
	uint32_t ClockTimeSetHc_ms;
	uint32_t Clock_BtK9BStartPress_ms[3];
	uint8_t Bt_K9B_CountPress[3];
} Sw_Woring_K9B_Str;

typedef struct {
	uint32_t MacOld;
	uint32_t MacNow;
	uint8_t Button_ID;
	uint8_t ButtonPressCounter;
	uint32_t ClockTimePress_ms;
} Sw_press_K9BHC_Str;
typedef struct {
	uint32_t Time_Last_Update_ms;
	uint8_t Update_Stt_Flag[5];
} Sw_Update_Stt_Str;

typedef struct {
	uint16_t CycleBlink;
	uint32_t Count_Toggle;
	uint32_t Last_ClockTime_Toggle;
} Sw_SingleBlink_Str;

typedef struct {
	uint32_t Time_Start_CountDown_s;
	uint32_t Time_CountDown_s;
	uint8_t Taget;
} CountDown_Str;

/*--------------------------- Variable ----------------------------------*/

extern Button_Stt_Type button1_Stt;
extern Button_Stt_Type button2_Stt;
extern Button_Stt_Type button3_Stt;
extern Button_Stt_Type button4_Stt;
extern Button_Stt_Type button5_Stt;

extern uint8_t Button1_Hold_Flag;
extern uint8_t Button2_Hold_Flag;
extern uint8_t Button3_Hold_Flag;
extern uint8_t Button4_Hold_Flag;
extern uint8_t Button5_Hold_Flag;

extern uint8_t Kick_all_Flag;
extern Sw_Working_Stt_Str Sw_Working_Stt_Val;
extern Relay_Stt_Type relay_Stt[5];
extern CountDown_Str CountDown_Val[5];

extern uint8_t countPower;
extern uint8_t powerSaved;
extern uint8_t Traing_Loop;

#define SETMAX(Val_SET, Num_Max) 	Val_SET = (Val_SET > Num_Max) ? Num_Max:Val_SET
#define SETMIN(Val_SET, Num_Min) 	Val_SET = (Val_SET < Num_Min) ? Num_Min : Val_SET

/*--------------------------- Function ----------------------------------*/
//void init_in_out_pin();
void RD_mod_in_out_init(void);
void RD_mod_in_out_loop(void);
void RD_mod_in_out_factory_reset();

#endif
