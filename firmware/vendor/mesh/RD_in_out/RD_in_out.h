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

/*--------------------------- Gpio init ----------------------------------*/
#define NUM_OF_ELEMENT		 		2
#define NUM_OF_INPUT				4
#define NUM_LED						6

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
#define ARR_LED						{LED_1,LED_2,LED_3,LED_4,LED_5,LED_6}

#define INPUT_READ(i)				gpio_read(Input_Array[i])
#define RD_IN_READ(i)				INPUT_READ(i)
#define BIT_INPUT(i)				(Input_Array[i] && 0xff)
extern uint32_t Input_Array[];

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
	RD_PowUpOff = 0x00, RD_PowUpOn = 0x01, RD_PowUpStore = 0x02, RD_POW_SET_MAX = RD_PowUpStore,
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
	uint8_t Bt_K9B_CountPress[3];
	uint32_t ClockTimeSetHc_ms;
} Sw_Woring_K9B_Str;

typedef enum
{
	UNPAIR = 0,
	PAIRED,
}k9b_handle_state_t;

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

typedef struct
{
	int time_delay_100ms;
	int light;
}out_stt_t;

typedef enum
{
	ON_OFF_BASIS = 0,
	BLINK_BLINK,
}mode_led_t;


#define MAX_STATE_QUEUE_CALLER 22
#define	CYCLE_MAX	8
#define	TIME_DELAY_MAX_100MS	5

#define MAX_STATE_QUEUE_CALLEE 10

typedef struct
{
	int light[MAX_STATE_QUEUE_CALLEE];
	int front;
	int rear;
}queue_write_out_t;

typedef struct
{
	out_stt_t out_status[MAX_STATE_QUEUE_CALLER];
	int front;
	int rear;
}queue_handle_t;


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
/*--------------------------- Variable ----------------------------------*/

extern Button_Stt_Type button1_Stt;
extern Button_Stt_Type button2_Stt;




extern uint8_t Button1_Hold_Flag;
extern uint8_t Button2_Hold_Flag;


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
void RD_mod_io_gw_reset(void);
void RD_SetAndRsp_Switch(int Light_index, u8 OnOff_Set, uint16_t GW_Add_Rsp_G_onoff);
void rd_module_io_handle_input_onoff(void);
void RD_SwitchAC4Ch_UpdateCtr(void);
void RD_SwitchAC4Ch_ScanReset(void);
void RD_Set_UpdateStt(uint8_t Relay_ID);

void rd_toggle_output(uint8_t id_ele, int rsp);
int RD_mod_io_onoff(int light,uint8_t id_ele, int rsp);
int RD_mod_io_blink(uint8_t cyclce, uint8_t time_ms,uint8_t id_ele);
void RD_handle_callee_state_out();
void RD_handle_caller_state_out();


//void RD_K9B_SaveOnOff(uint32_t macDevice, uint8_t key);
//void RD_K9B_CheckScanK9BHc(uint32_t K9BMac_Buff, uint8_t Num_Of_Button, s8 rssi);
//uint8_t RD_K9B_ScanPress2HC(uint32_t macDevice, uint8_t key, uint32_t par_signature);
//int RD_CheckButtonPosK9BHC(uint8_t ButtonID_Aray[MAX_MUM_K9BPRESSTYPE], uint8_t ButtonID_Check);

void RD_K9B_PairHCSet(uint8_t pairStt, uint16_t K9BAdd);
void RD_K9B_TimeOutScanK9BHC(void);
void RD_K9B_ScanPairOnOff(void);


void rd_set_update_in_stt();
void rd_check_update_in_stt();

u16 get_adc_value();
u8 get_status_input(u8 idx);

void reset_detect_input(u8 id);
void reset_all_detect_input(u8 id);

void rd_check_adc();
#endif
