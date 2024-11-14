#include "rd_in_out.h"

static uint8_t Reset_Count_Buff = 0;
static u8 input_count[NUM_OF_INPUT] = {0};
static u8 rd_input_state[NUM_OF_INPUT] = {0};
static uint8_t buttonSttBuff[NUM_OF_INPUT] = { 0 };

static u16 adc_value = 0xffff;
static int need_update = 0;
u32 last_time_update_input_stt = 0;


typedef u8 (*handle_detect_input_t)(u8 id);
typedef u8 (*handle_input_t)(u8 id);

typedef struct
{
	u8 mode;
	handle_detect_input_t detect_handler;
	handle_input_t handler;
}input_ctr_t;

u16 get_adc_value()
{
	return adc_value;
}
u8 get_status_input(u8 idx)
{
	if(idx < NUM_OF_INPUT)
	{
		return rd_input_state[idx];
	}
	return 0xff;
}

u8 rd_handle_input_press(u8 idx)
{
	rd_set_update_in_stt();
	u8 idx_ele = get_ele_linked(idx);
	if(idx_ele != 0xff )
	{
		rd_toggle_output(idx_ele,1);
	}
	return 0;
}

u8 rd_handle_input_asyn(u8 idx)
{
	u8 status = rd_input_state[idx];
	u16 sence = get_sence_input(idx);

	rd_update_input_stt(idx,status_in,sence);
	rd_set_update_in_stt();
	u8 idx_ele = get_ele_linked(idx);
	if(idx_ele != 0xff )
	{
		RD_mod_io_onoff(rd_input_state[idx],idx_ele,1);
	}
	return 0;
}
//
u8 rd_detect_press(u8 id)
{
	if (RD_IN_READ(id) == 0)
	{
		input_count[id]++;
		if (input_count[id] == CYCLE_ACTIVE_BUTTON)
		{
			buttonSttBuff[id] = 1;
		}
		if (input_count[id] >= (CYCLE_ACTIVE_BUTTON + 1))
		{
			input_count[id] = CYCLE_ACTIVE_BUTTON + 1;
		}
	}
	else
	{
		input_count[id] = 0;
		buttonSttBuff[id] = 0;
	}

	//------------------update input status------------------------//
	if ((buttonSttBuff[id] == 1) && (input_count[id] < CYCLE_ACTIVE_BUTTON + 1))
	{
		RD_ev_log("but %d press\n",id);
		rd_input_state[id] = Button_Press;
		return 1;
	}
	else if (buttonSttBuff[id] == 0)
	{
		rd_input_state[id] = Button_None;
		return 0;
	}
	return 0;
}

u8 rd_detect_level(u8 id)
{
	u8 stt = RD_IN_READ(id);
	if(rd_input_state[id] != stt)
	{
		input_count[id]++;
		if (input_count[id] == CYCLE_ACTIVE_BUTTON)
		{
			buttonSttBuff[id] = 1;
		}
		if (input_count[id] >= (CYCLE_ACTIVE_BUTTON + 1))
		{
			input_count[id] = CYCLE_ACTIVE_BUTTON + 1;
		}
	}
	else
	{
		input_count[id] = 0;
		buttonSttBuff[id] = 0;
	}
	if ((buttonSttBuff[id] == 1))
	{
		RD_ev_log("in %d change logic %d",id,stt);
		rd_input_state[id] = stt;
		return 1;
	}
	return 0;
}



static input_ctr_t in_handle_arr[] ={
		{SYNC_PRESS_STATE, rd_detect_level,rd_handle_input_asyn},
		{PRESS_RELEASE_STATE,rd_detect_press,rd_handle_input_press},
};

void RD_SwitchAC4Ch_ScanB_V2(void)
{
	for (u8 i = 0; i<NUM_OF_INPUT; i++)
	{
		u8 stt = get_mode_setting_input(i);
		for( u8 j= 0; j<sizeof(in_handle_arr) /sizeof(in_handle_arr[0]) ; j++)
		{
			if(stt == in_handle_arr[j].mode)
			{
				u8 err = in_handle_arr[j].detect_handler(i);
				if(err)
				{
					in_handle_arr[j].handler(i);
				}
				break;
			}
		}
	}
}



void reset_detect_input(u8 id)
{
	input_count[id] = 0;
	buttonSttBuff[id] = 0;
}

void reset_all_detect_input(u8 id)
{
	for(int i =0; i<NUM_OF_INPUT; i++)
	{
		reset_detect_input(i);
	}
}

void RD_ScanKickAll(void)
{
	if(Kick_all_Flag==1)
	{
		static uint32_t countDownt =0;
		static uint8_t check_reset_Flag=0;
		if(check_reset_Flag == 0)
		{
			check_reset_Flag =1;
			countDownt = clock_time_ms();
		}
		if( (check_reset_Flag == 1) && ((clock_time_ms() - countDownt) >= TIME_RESET_ALL) )
		{
			uart_CSend("\nFactory Reset\n");
			RD_mod_in_out_factory_reset();
		}
	}
}


void rd_check_update_in_stt()
{
	if(need_update == 1)
	{
		RD_ev_log("update stt\n");
//		rd_update_input_stt(rd_input_state,adc_value,NUM_OF_INPUT);
		last_time_update_input_stt = clock_time_ms();
		need_update = 0;
	}
	if(need_update > 0)
	{
		if(clock_time_ms() - last_time_update_input_stt > 500)
		{
			need_update = 0;
		}
	}
}

void rd_set_update_in_stt()
{
	need_update ++;
}

void rd_read_adc()
{
	adc_value = adc_sample_and_get_result();
}
