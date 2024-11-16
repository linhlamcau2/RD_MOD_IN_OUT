#include "rd_in_out.h"

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
//	rd_set_update_in_stt();
	u8 status_in = MODE_PULSING;
	u16 sence_id = get_sence_input(idx);
	rd_update_input_stt(idx,status_in,sence_id);
	if(sence_id)
	{
		rd_input_call_sence(sence_id);
	}
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
	u16 sence_id = get_sence_input(idx);
	rd_update_input_stt(idx,status,sence_id);

	u8 stt_sence = get_stt_sence(idx);
	if(sence_id && stt_sence == status)
	{
		rd_input_call_sence(sence_id);
	}

//	rd_set_update_in_stt();
	u8 idx_ele = get_ele_linked(idx);
	if(idx_ele != 0xff )
	{
		RD_mod_io_onoff(rd_input_state[idx],idx_ele,1);
	}
	return 0;
}

u8 rd_read_input(u8 id)
{
	u16 stt = RD_IN_READ(id);
	u8 st = (stt && 0xff) && BIT_INPUT(id);
	return (st >0);
}

u8 rd_detect_press(u8 id)
{
	if (rd_read_input(id) != 0)
	{
		input_count[id]++;
		if (input_count[id] == CYCLE_ACTIVE_BUTTON)
		{
			buttonSttBuff[id] = 1;
		}
		if (input_count[id] > CYCLE_ACTIVE_BUTTON)
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
	if ((buttonSttBuff[id] == 1))
	{
		if((input_count[id] < CYCLE_ACTIVE_BUTTON + 1))
		{
			RD_ev_log("but %d press\n",id);
			rd_input_state[id] = Button_Press;
			return 1;
		}
		else
			return 0;
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
	u8 stt = rd_read_input(id);
	if(rd_input_state[id] != stt)
	{
		input_count[id]++;
		if (input_count[id] == CYCLE_DETECT_LEVEL)
		{
			buttonSttBuff[id] = 1;
		}
		if (input_count[id] > CYCLE_DETECT_LEVEL )
		{
			input_count[id] = CYCLE_DETECT_LEVEL + 1;
			buttonSttBuff[id] = 0;
		}
	}
	else
	{
		input_count[id] = 0;
		buttonSttBuff[id] = 0;
	}
	if ((buttonSttBuff[id] == 1))
	{
		RD_ev_log("in %d change logic %d\n",id,stt);
		rd_input_state[id] = stt;
		return 1;
	}
	return 0;
}



static input_ctr_t in_handle_arr[] ={
		{SYNC_PRESS_STATE, rd_detect_level,rd_handle_input_asyn},
		{PRESS_RELEASE_STATE,rd_detect_press,rd_handle_input_press},
};

void rd_module_io_handle_input_onoff(void)
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

void rd_printf_adc()
{
	static u32 log_adc_ms = 0;
	if(clock_time_ms() - log_adc_ms > 500)
	{
		u16 adc_value = adc_sample_and_get_result();
		RD_ev_log("rd_printf_adc: %d\n",adc_value);
		log_adc_ms = clock_time_ms();
	}
}

void rd_check_adc()
{
	static u8 handle_check = 0;
	static u8 count_check = 0;
	rd_printf_adc();
	if(get_adc_sence())
	{
		u16 adc_val = adc_sample_and_get_result();
		if(rd_check_state_adc(adc_val))
		{
			count_check ++;
			if(count_check == COUNT_CHECK_ADC)
			{
				handle_check = 1;
			}
			else if(count_check > COUNT_CHECK_ADC)
			{
				count_check = COUNT_CHECK_ADC +1;
				handle_check = 0;
			}
		}
		else
		{
			adc_value = adc_val;
			handle_check = 0;
			count_check = 0;
		}
		if(handle_check)
		{
			adc_value = adc_val;
			rd_input_call_sence(get_adc_sence());
		}
	}
}


