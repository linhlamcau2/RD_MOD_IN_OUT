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
#include "vendor/common/light.h"
#include "proj_lib/rf_drv.h"
#include "proj_lib/pm.h"
#include "proj_lib/ble/blt_config.h"
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/sig_mesh/app_mesh.h"




uint8_t Button1_Hold_Flag = 0;
uint8_t Button2_Hold_Flag = 0;



uint8_t Register_Config[128] = { 0 };

Button_Stt_Type button1_Stt = Button_None;
Button_Stt_Type button2_Stt = Button_None;




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

uint32_t Input_Array[] = ARR_INPUT_PIN;
uint32_t Output_Array[NUM_OF_ELEMENT] = ARR_OUTPUT_PIN;
uint32_t led_arr[] = ARR_LED;

extern void init_handler_callee_state_output();
extern void init_handler_caller_state_output();
static void RD_init_handle_output()
{
	init_handler_callee_state_output();
	init_handler_caller_state_output();
}

static void RD_GPIO_Init(void) {
	/*--------------------------- init output led ----------------------------------*/
	for (int i = 0; i < NUM_OF_ELEMENT; i++) {
		gpio_set_func(Output_Array[i], AS_GPIO);
		gpio_set_output_en(Output_Array[i], 1);
	}
	for (int i = 0; i < NUM_OF_INPUT; i++) {
		gpio_setup_up_down_resistor(Input_Array[i], PM_PIN_PULLDOWN_100K);
		gpio_set_func(Input_Array[i], AS_GPIO);
		gpio_set_input_en(Input_Array[i], 1);
	}

	for(u8 i =0; i<sizeof(led_arr)/ sizeof(led_arr[0]); i++)
	{
		gpio_set_func(led_arr[i], AS_GPIO);
		gpio_set_output_en(led_arr[i], 1);
		gpio_write(led_arr[i], i%2);
	}

//	gpio_set_func(BUZZER, AS_GPIO);
//	gpio_set_output_en(BUZZER, 1);
//	gpio_write(BUZZER, 0);

	gpio_setup_up_down_resistor(BUTTON_RESET, PM_PIN_PULLDOWN_100K);
	gpio_set_func(BUTTON_RESET, AS_GPIO);
	gpio_set_input_en(BUTTON_RESET, 1);

	RD_init_handle_output();
}

static void RD_in_out_check_provision(void) {
	if (get_provision_state() == STATE_DEV_UNPROV) {
//		RD_mod_io_blink(4,4,0);
//		RD_mod_io_blink(4,4,1);
	}
}

void RD_mod_in_out_init(void) {
	RD_GPIO_Init();

	RD_Flash_Init();

	RD_in_out_check_provision();

//	RD_Secure_CheckInit();
	//	Send_Relay_Stt_Message_RALL_PowerUp();
}


void RD_mod_in_out_factory_reset() {
	uart_CSend("Factory reset \n");
	for (int j = 0; j < 5; j++) {
		for (int i = 0; i < 6; i++) {
			gpio_write(led_arr[i],0);
		}
		sleep_ms(200);
		for (int i = 0; i < 6; i++) {
			gpio_write(led_arr[i], 1);
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


void rd_check_button_reset()
{
	static u16 count = 0;
	static u8 fl_reset = 0;

	if(gpio_read(BUTTON_RESET) == 0)
	{
		count ++;
		if(count == CYCLE_HOLD_BUTTON)
		{
			fl_reset = 1;
		}
		if(count>= (CYCLE_HOLD_BUTTON+1))
		{
			count = CYCLE_HOLD_BUTTON+1;
		}
	}
	else
	{
		fl_reset = 0;
		count = 0;
	}
	if(fl_reset == 1)
	{
		RD_ev_log("reset mod inout\n");
		RD_mod_in_out_factory_reset();
	}
}

void RD_mod_in_out_loop(void) {
	static uint64_t clockTick_ReadBt_ms = 0;

	if(clock_time_ms() < clockTick_ReadBt_ms) clockTick_ReadBt_ms = clock_time_ms();
	if(clock_time_ms() - clockTick_ReadBt_ms >= CYCLE_READ_BT_MS)
	{
		clockTick_ReadBt_ms = clock_time_ms();
		RD_SwitchAC4Ch_ScanB_V2();
		rd_check_button_reset();
	}

//	RD_SwitchAC4Ch_UpdateCtr();

//	RD_SwitchAC4Ch_ScanReset();

//	RD_K9B_ScanPairOnOff();
//
//	RD_K9B_TimeOutScanK9BHC();


	rd_check_update_in_stt();

	RD_handle_caller_state_out();

	RD_handle_callee_state_out();

//	RD_Secure_CheckLoop();
}


void RD_SetAndRsp_Switch(int Light_index, u8 OnOff_Set, uint16_t GW_Add_Rsp_G_onoff)
{
	uart_CSend(" SetAndRsp_Switch\n");
	light_onoff_idx(Light_index,OnOff_Set, 0);
	set_on_power_up_onoff(Light_index, 0, OnOff_Set); // save for POWer_up

	if(GW_Add_Rsp_G_onoff != 0x0000)			// Rsp to Gw if GW_Add_Rsp_G_onoff != 0x0000;
	{
		uart_CSend("send on_off\n");
		RD_Send_Relay_Stt(Light_index + 1, OnOff_Set);
	}
}

void RD_mod_io_gw_reset(void) //RD_Todo:
{
	for(int i=0; i<5; i ++)
	{
		for(int i=0; i< NUM_OF_ELEMENT; i++)
		{
			OUTPUT_WRITE(i,0);
		}
		sleep_ms(200);
		for(int i=0; i< NUM_OF_ELEMENT; i++)
		{
			OUTPUT_WRITE(i,0);
		}
		sleep_ms(200);
		wd_clear(); //clear watch dog
	}
	if((get_provision_state() == STATE_DEV_UNPROV))
	{
		reset_Mess_Flag = 0;
	}
	RD_Flash_Save_DataDefault();
	sleep_ms(200);
}


void RD_Set_UpdateStt(uint8_t Relay_ID)
{
	Sw_Update_Stt_Val.Time_Last_Update_ms = clock_time_ms() + (ele_adr_primary%50)*100; // set time rsp in many time task
	if(Relay_ID <= 5 )
	{
		Sw_Update_Stt_Val.Update_Stt_Flag[Relay_ID-1] = RD_EN;
	}
	if(Relay_ID == 0xff)
	{
		Sw_Update_Stt_Val.Update_Stt_Flag[0] = RD_EN;
		Sw_Update_Stt_Val.Update_Stt_Flag[1] = RD_EN;
		Sw_Update_Stt_Val.Update_Stt_Flag[2] = RD_EN;
		Sw_Update_Stt_Val.Update_Stt_Flag[3] = RD_EN;
		Sw_Update_Stt_Val.Update_Stt_Flag[4] = RD_EN;
	}
	char UART_Buff[64];
	sprintf(UART_Buff,"set update stt Relay ID: %d", Relay_ID);
	uart_CSend(UART_Buff);
}




