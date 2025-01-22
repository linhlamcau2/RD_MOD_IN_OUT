#include "rd_in_out.h"

static u8 input_count[NUM_OF_INPUT] = {0};
static u8 rd_input_state[NUM_OF_INPUT] = {0};
static uint8_t buttonSttBuff[NUM_OF_INPUT] = { 0 };

static u16 adc_value = 0;
static u16 val = 0;
static int need_update = 0;
u32 last_time_update_input_stt = 0;

u8 rd_mode_adc = MODE_NORMAL;
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
	u8 status_in = MODE_PULSING;
	u16 sence_id = get_sence_input(idx,0);

	rd_blink_led(idx,LED_NUM_RAISING_INPUT,TIME_500MS);

	rd_update_input_stt(idx,status_in,sence_id);
	if(sence_id)
	{
		rd_input_call_sence(sence_id);
	}
	u8 idx_ele = get_ele_linked(idx);
	if(idx_ele != 0xff )
	{
		rd_toggle_relay(idx_ele,1);
	}
	return 0;
}

u8 rd_handle_input_asyn(u8 idx)
{
	u8 status = rd_input_state[idx];
	u16 sence_id = get_sence_input(idx,!(!status));

	rd_on_off_led(idx,status);
	rd_update_input_stt(idx,status,sence_id);
	if(sence_id)
	{
		rd_input_call_sence(sence_id);
	}

	u8 idx_ele = get_ele_linked(idx);
	if(idx_ele != 0xff )
	{
		rd_onoff_relay(rd_input_state[idx],idx_ele,1,0);
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
	if (rd_read_input(id))
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
			RD_ev_log("in %d change logic %d\n",id,stt);
			rd_input_state[id] = stt;
			return 1;
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
	return 0;
}

void rd_init_input()
{
	for(u8 idx=0; idx< NUM_OF_INPUT; idx++)
	{
		if(get_mode_setting_input(idx) == SYNC_PRESS_STATE)
		{
			u8 status = rd_read_input(idx);
			rd_input_state[idx] = status;

			u16 sence_id = get_sence_input(idx,!(!status));

			rd_on_off_led(idx,status);
			rd_update_input_stt(idx,status,sence_id);
		}
	}
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
	if(Kick_all_Flag)
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


void rd_read_adc()
{
	adc_value = adc_sample_and_get_result();
}

void rd_printf_adc()
{
	static u32 log_adc_ms = 0;
	if(clock_time_ms() - log_adc_ms > 3000)
	{
		app_battery_check_and_re_init_user_adc();
		u16 adc_val = adc_sample_and_get_result();
//		RD_ev_log("rd_printf_adc: %d\n",adc_val);
		log_adc_ms = clock_time_ms();
	}
}

void rd_read_delta_adc(u16 adc_val,u16 id_sence)
{
	static u32 last_tick_send = 0;  //TIME_MAX_SEND_ADC_MINUTE
	static u8 count[2] = {0};

	adc_val = (adc_val > ADC_MAX_MV) ? ADC_MAX_MV: adc_val ;
	u8 idx_count = (adc_val > val) ? 1: 0;
	u16 delta = (adc_val > val) ? (adc_val - val) : (val - adc_val);
	count[!idx_count] = 0;
	if(delta > get_delta_adc())
	{
		count[idx_count] ++;
		if(count[idx_count] > COUNT_CHECK_ADC)
		{
			count[idx_count] = COUNT_CHECK_ADC + 1;
		}
		if(count[idx_count] == COUNT_CHECK_ADC)
		{
			val = adc_val;
			RD_ev_log("val update : %d\n",val);
			adc_value = val;
			rd_update_adc_stt(val,id_sence);
			last_tick_send = clock_time_ms();
		}
	}
	else
	{
		count[0] = 0;
		count[1] = 0;
	}
	if(clock_time_ms() - last_tick_send > TIME_MAX_SEND_ADC_MINUTE * 60 * 1000)
	{
		rd_update_adc_stt(adc_val,id_sence);
		last_tick_send = clock_time_ms();
	}
}

void rd_check_condition_adc(u16 adc_val, u16 id_sence)
{
	static u8 count_check = 0;
	if(rd_check_state_adc(adc_val))
	{
		count_check ++;
		if(count_check == COUNT_CHECK_ADC)
		{
			adc_value = adc_val;
			rd_input_call_sence(id_sence);
		}
		else if(count_check > COUNT_CHECK_ADC)
		{
			count_check = COUNT_CHECK_ADC +1;
		}
	}
	else
	{
		count_check = 0;
	}
}


void rd_handle_adc_calib(u16 adc_val)
{
	static u8 count = 0;
	static u32 sum = 0;
	u16 adc_avg = 0;

	sum += adc_val;
	count ++;

	if(count == 25)
	{
		adc_avg = sum / count;
		count = 0;
		sum = 0;
		u8 err = rd_save_adc_calib(adc_avg);
		if(err)
		{
			RD_ev_log("rd calib sucess : %d\n",adc_avg);
			rd_blink_led(0xff,LED_NUM_ADC_CALIB_SUCCESS,TIME_400MS);
		}
		rd_mode_adc = MODE_NORMAL;
	}
}

void rd_check_adc()
{
	app_battery_check_and_re_init_user_adc();
	u16 adc_val = adc_sample_and_get_result();

	if(rd_mode_adc == MODE_NORMAL )
	{
		u16 id_sence = get_adc_sence();

		u16 adc_calib = get_adc_calib();
		u16 adc_mv = adc_val * ADC_MAX_MV / adc_calib;
		static u32 log_adc_ms = 0;
		if(clock_time_ms() - log_adc_ms > 3000)
		{
			RD_ev_log("rd_printf_adc: %d\n",adc_mv);
			log_adc_ms = clock_time_ms();
		}

		rd_read_delta_adc(adc_mv,id_sence);

		if(id_sence)
		{
			rd_check_condition_adc(adc_mv,id_sence);
		}
	}
	else if (rd_mode_adc == MODE_CALIB)
	{
		rd_handle_adc_calib(adc_val);
	}
}

void rd_init_adc()
{
	app_battery_check_and_re_init_user_adc();
	u16 adc_val = adc_sample_and_get_result();
	u16 adc_calib = get_adc_calib();

	adc_value = adc_val * ADC_MAX_MV / adc_calib;
	adc_val = (adc_val > ADC_MAX_MV) ? ADC_MAX_MV: adc_val ;
	val = adc_value;
	u16 id_sence = get_adc_sence();
	rd_update_adc_stt(adc_value,id_sence);
}

