/*
 *  Created on: Sep 26, 2024
 *      Author: Linh.NT
 */
#include "rd_in_out.h"

extern void mesh_g_onoff_st_rsp_par_fill(mesh_cmd_g_onoff_st_t *rsp, u8 idx);
extern void set_on_power_up_onoff(int idx, int st_trans_type, u8 onoff);
extern int mesh_tx_cmd_g_onoff_st(u8 idx, u16 ele_adr, u16 dst_adr, u8 *uuid,
		model_common_t *pub_md, u16 op_rsp);

#include "tl_common.h"
#include "proj/mcu/watchdog_i.h"
#include "vendor/common/user_config.h"
#include "proj_lib/rf_drv.h"
#include "proj_lib/pm.h"
#include "proj_lib/ble/blt_config.h"
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/sig_mesh/app_mesh.h"

uint8_t CycleBlink = 0;
uint8_t check = 0;

uint16_t CountToggleBlink = 0;
uint8_t Button1_Keep_Flag = 0;
uint8_t Button2_Keep_Flag = 0;
uint8_t Button3_Keep_Flag = 0;
uint8_t Button4_Keep_Flag = 0;
uint8_t Button5_Keep_Flag = 0;

uint8_t Button1_Hold_Flag = 0;
uint8_t Button2_Hold_Flag = 0;
uint8_t Button3_Hold_Flag = 0;
uint8_t Button4_Hold_Flag = 0;
uint8_t Button5_Hold_Flag = 0;

uint8_t Sw1_PairK9B_OnOff_Flag = 0;
uint8_t Register_Config[128] = { 0 };

Button_Stt_Type button1_Stt = Button_None;
Button_Stt_Type button2_Stt = Button_None;
Button_Stt_Type button3_Stt = Button_None;
Button_Stt_Type button4_Stt = Button_None;
Button_Stt_Type button5_Stt = Button_None;
static uint8_t Reset_Count_Buff = 0;

Relay_Stt_Type relay_Stt[5] = { 0 };

CountDown_Str CountDown_Val[5] = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0,
		0, 0 }, { 0, 0, 0 } };

Sw_SingleBlink_Str Sw_SingleBlink_Val[NUM_OF_ELEMENT];

uint8_t pressResetCount = 0;
uint8_t countPower = 0;
uint8_t powerSaved = 0;
uint8_t Traing_Loop = 0;

uint8_t Fist_Socket_Relay = 0;
uint8_t Kick_all_Flag;
Sw_Working_Stt_Str Sw_Working_Stt_Val;
Sw_press_K9BHC_Str Sw_Press_K9BHC_Val;
Sw_Update_Stt_Str Sw_Update_Stt_Val;
u8 element = 2;

uint32_t Input_Array[NUM_OF_INPUT] = ARR_INPUT_PIN;
uint32_t Output_Array[NUM_OF_ELEMENT] = ARR_OUTPUT_PIN;

Sw_Woring_K9B_Str Sw_Woring_K9B_Val = { 0 };
static uint16_t count_k9B_press[NUM_OF_ELEMENT] = { 0 };

static void RD_GPIO_Init(void) {
	/*--------------------------- init output led ----------------------------------*/
	for (int i = 0; i < NUM_OF_ELEMENT; i++) {
		gpio_set_func(Output_Array[i], AS_GPIO);
		gpio_set_output_en(Output_Array[i], 1);
		OUTPUT_WRITE(i,1);
	}
	for (int i = 0; i < NUM_OF_INPUT; i++) {
		gpio_setup_up_down_resistor(Input_Array[i], PM_PIN_PULLDOWN_100K);
		gpio_set_func(Input_Array[i], AS_GPIO);
		gpio_set_input_en(Input_Array[i], 1);
	}
}

static void RD_Switch_Intro(void) {
	//	char UART_TempSend[64];
	//	sprintf(UART_TempSend,"RD Switch V3 x2 Version: %d.%d \n",Firm_Ver_H,Firm_Ver_L);
	uart_CSend("Module In Out Version: 0.1.0\n");
}

static void RD_in_out_check_provision(void) {
	if (get_provision_state() == STATE_DEV_UNPROV) {
		OUTPUT_WRITE(0,0);
	}
}

void RD_mod_in_out_init(void) {
	RD_GPIO_Init();

	RD_Flash_Init();

	RD_in_out_check_provision();

	RD_Secure_CheckInit();
	//	Send_Relay_Stt_Message_RALL_PowerUp();
}

void RD_SwitchAc4Ch_BlinkSet(uint8_t cycle, uint16_t countToggle) {
	CycleBlink = cycle;
	CountToggleBlink = countToggle;
}

void RD_mod_in_out_factory_reset() {
	uart_CSend("Factory reset \n");
	for (int j = 0; j < 5; j++) {
		for (int i = 0; i < NUM_OF_ELEMENT; i++) {
			OUTPUT_WRITE(i,0);
		}
		sleep_ms(200);
		for (int i = 0; i < NUM_OF_ELEMENT; i++) {
			OUTPUT_WRITE(i,1);
		}
		sleep_ms(200);
		wd_clear(); //clear watch dog
	}
	if ((get_provision_state() == STATE_DEV_UNPROV)) {
		reset_Mess_Flag = 0;
	}
	RD_Flash_Save_DataDefault();
	sleep_ms(200);
	factory_reset();
	start_reboot();
}

void RD_mod_in_out_loop(void) {
	static uint16_t timeLoop = 0;
	static uint64_t clockTick_ReadBt_ms = 0;
	timeLoop++;
	if (timeLoop >= 200) {

		timeLoop = 0;

#if 0
		char UART_TempSend[128];
		sprintf(UART_TempSend,"Toggle%x \n",relay_stt);
		uart_CSend(UART_TempSend);
#endif
	}
	RD_Secure_CheckLoop();
}
