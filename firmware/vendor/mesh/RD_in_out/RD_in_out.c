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

#include "../rd_queue.h"

uint8_t Kick_all_Flag = 0;;
Sw_Working_Stt_Str Sw_Working_Stt_Val = {0};
u8 Train_Factory = 0;

uint32_t Input_Array[] = ARR_INPUT_PIN;
uint32_t Output_Array[NUM_OF_ELEMENT] = ARR_OUTPUT_PIN;


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

	for(u8 i =0; i<NUM_LED; i++)
	{
		gpio_set_func(arr_led[i], AS_GPIO);
		gpio_set_output_en(arr_led[i], 1);
		gpio_write(arr_led[i], 1);
	}


	gpio_setup_up_down_resistor(BUTTON_RESET, PM_PIN_PULLDOWN_100K);
	gpio_set_func(BUTTON_RESET, AS_GPIO);
	gpio_set_input_en(BUTTON_RESET, 1);
}

static void RD_in_out_check_provision(void) {
//	if (get_provision_state() == STATE_DEV_PROVED){
	if (get_provision_state() == STATE_DEV_UNPROV) {
		rd_blink_led(0,5,5);
		rd_blink_led(1,5,5);
		rd_blink_led(2,5,5);
		rd_blink_led(3,5,5);
		rd_blink_led(4,5,5);
		rd_blink_led(5,5,5);
	}
}

void RD_mod_in_out_init(void) {
	RD_GPIO_Init();

	rd_init_queue_rsp();
	rd_init_queue_led();
	rd_init_queue_relay();

	RD_in_out_check_provision();
	RD_Flash_Init();


	RD_Secure_CheckInit();
}


void RD_mod_in_out_factory_reset() {
	uart_CSend("Factory reset \n");
	for (int j = 0; j < 5; j++) {
		for (int i = 0; i < 6; i++) {
			gpio_write(arr_led[i],0);
		}
		sleep_ms(200);
		for (int i = 0; i < 6; i++) {
			gpio_write(arr_led[i], 1);
		}
		sleep_ms(200);
		wd_clear(); //clear watch dog
	}
	RD_Flash_Save_DataDefault();
	sleep_ms(200);
	factory_reset();
	start_reboot();
}

typedef void (*handle_t)(void);
u8 rd_filter_input(u8 *num_max,u8 *count, u8 *stt,handle_t handle_func)
{
	if(*stt)
	{
		*count ++;
		if(*count == *num_max)
		{
			handle_func();
		}
		if(*count > *num_max)
		{
			*count = *num_max+1;
		}
	}
	else
	{
		*count = 0;
	}
	return 0;
}

void rd_check_button_reset()
{
	static u16 count = 0;
	if(gpio_read(BUTTON_RESET) == 0)
	{
		count ++;
		if(count == CYCLE_HOLD_BUTTON)
		{
			RD_ev_log("reset mod inout\n");
			RD_mod_in_out_factory_reset();
		}
		if(count > CYCLE_HOLD_BUTTON)
		{
			count = CYCLE_HOLD_BUTTON+1;
		}
	}
	else
	{
		count = 0;
	}
}

void RD_mod_in_out_loop(void) {
	static uint64_t clockTick_ReadBt_ms = 0;
	static u32 clock_time_read_adc_ms = 0;
	if(clock_time_ms() < clockTick_ReadBt_ms) clockTick_ReadBt_ms = clock_time_ms();
	if(clock_time_ms() - clockTick_ReadBt_ms >= CYCLE_READ_BT_MS)
	{
		clockTick_ReadBt_ms = clock_time_ms();
		rd_module_io_handle_input_onoff();
		rd_check_button_reset();
	}
	if(clock_time_ms() - clock_time_read_adc_ms > CYCLE_READ_ADC_MS)
	{
		rd_check_adc();
		clock_time_read_adc_ms = clock_time_ms();
	}
	RD_Secure_CheckLoop();
}


void rd_rsp_stt_relay(int Light_index, u8 OnOff_Set, uint16_t GW_Add_Rsp_G_onoff)
{
	uart_CSend(" SetAndRsp_Switch\n");
	light_onoff_idx(Light_index,OnOff_Set, 0);
	set_on_power_up_onoff(Light_index, 0, OnOff_Set); // save for POWer_up

	if(GW_Add_Rsp_G_onoff != 0x0000)			// Rsp to Gw if GW_Add_Rsp_G_onoff != 0x0000;
	{
		uart_CSend("send on_off\n");
		rd_send_relay_stt(Light_index + 1, OnOff_Set);
	}
}

void RD_mod_io_gw_reset(void) //RD_Todo:
{
	for (int j = 0; j < 5; j++)
	{
		for (int i = 0; i < 6; i++) {
			gpio_write(arr_led[i],0);
		}
		sleep_ms(200);
		for (int i = 0; i < 6; i++) {
			gpio_write(arr_led[i], 1);
		}
		sleep_ms(200);
		wd_clear(); //clear watch dog
	}
	RD_Flash_Save_DataDefault();
	sleep_ms(200);
}

void rd_train_factory()
{
	static u32 Time_Pre=0;
	static u8 stt = 1;
	if(Train_Factory)
	{
		mesh_adv_prov_link_close();
		if(get_provision_state() == STATE_DEV_UNPROV)
		{
			if((clock_time_s() - Time_Pre) >= TIME_TRAINING_TOGGLE_STATE_S)
			{
				Time_Pre = clock_time_s();
				stt = !stt;
				for(u8 i= 0; i<NUM_OF_RELAY;i++)
				{
//					rd_init_onoff_relay(stt,i);
				}
				for(u8 i=0; i< NUM_LED;i++)
				{
//					rd_on_off_led(i,stt);
				}
			}
		}
	}
}
