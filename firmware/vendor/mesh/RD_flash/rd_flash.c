#include "rd_flash.h"
#include "../RD_in_out/rd_in_out.h"


extern uint16_t RD_GATEWAYADDRESS;
extern u8 rd_read_input(u8 id);
static Sw_Flash_Data Sw_Flash_Data_Val = { 0 };

u8 get_mode_setting_input(u8 idx)
{
	if(idx >= 0 && idx < NUM_OF_INPUT)
	{
		return Sw_Flash_Data_Val.input_setting[idx].mode;
	}
	return 0xff;
}

u16 get_sence_input(u8 idx, u8 status)
{
	if(get_mode_setting_input(idx) == SYNC_PRESS_STATE)
	{
		if(status < LOGIC_STT_MAX)
		{
			return Sw_Flash_Data_Val.input_setting[idx].id_sence[status];
		}
	}
	else if(get_mode_setting_input(idx) == PRESS_RELEASE_STATE)
	{
		return Sw_Flash_Data_Val.input_setting[idx].id_sence[LOGIC_RAISING];
	}
	return 0;
}

u8 get_ele_linked(u8 idx)
{
	for( u8 i=0; i<NUM_OF_ELEMENT; i++)
	{
		if(Sw_Flash_Data_Val.output_linked[i].input == idx)
		{
			return i;
		}
	}
	return 0xff;
}

u8 is_output_linked(u8 idx_out)
{
	if(idx_out < NUM_OF_ELEMENT )
	{
		if(Sw_Flash_Data_Val.output_linked[idx_out].input == 0xff)
			return 0;
		return 1;
	}
	return 1;
}

u16 get_adc_sence()
{
	return Sw_Flash_Data_Val.adc_setting.id_sence;
}

u16 get_adc_threshold()
{
	return Sw_Flash_Data_Val.adc_setting.adc_threshold;
}

static void rd_flash_save()
{
	flash_erase_sector(FLASH_ADDR);
	flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );
}

void RD_Flash_Save_DataDefault(void) {
	memset((void *)&Sw_Flash_Data_Val, 0 , sizeof(Sw_Flash_Data_Val));

	Sw_Flash_Data_Val.Gw_Add = GW_ADD_DEFAULT;
	Sw_Flash_Data_Val.adc_setting.delta = DELTA_PERCENT_ADC_DEFAULT;
	for( u8 i=0; i<NUM_OF_ELEMENT; i++)
	{
		Sw_Flash_Data_Val.output_linked[i].input = 0xff;
		Sw_Flash_Data_Val.output_linked[i].pow_up = RD_PowUpStore;
	}
	rd_flash_save();
#if UART_ON

	uart_CSend("Save Data default\n");

#endif
}

u8 get_power_start_output(u8 idx)
{
	u8 buff = 0xff;
	if(idx < NUM_OF_ELEMENT)
	{
		buff = Sw_Flash_Data_Val.output_linked[idx].pow_up;
	}
	return buff;
}

__attribute__((unused)) void RD_init_flash_out_handle()
{
	for(int i =0; i< NUM_OF_ELEMENT; i++)
	{
		u8 state = RD_get_on_off(i,0);
		RD_ev_log("out %d: %d\n",i,state);
	}
}

static void RD_Flash_Data_Init(void) {
	flash_read_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val),
			(uint8_t *) &Sw_Flash_Data_Val.Factory_Check);
	if (Sw_Flash_Data_Val.Factory_Check == 0xFF) // fist time to read reset memory
	{
		RD_Flash_Save_DataDefault();
	}
	RD_GATEWAYADDRESS = Sw_Flash_Data_Val.Gw_Add ? Sw_Flash_Data_Val.Gw_Add : 0x0001;

	RD_ev_log("Sw_Flash_Data: Sec %d,gw: %d\n",Sw_Flash_Data_Val.Secure_RD,Sw_Flash_Data_Val.Gw_Add);
	RD_ev_log("1: mode: %d,sence0: %d,sence1: %d\n",Sw_Flash_Data_Val.input_setting[0].mode,Sw_Flash_Data_Val.input_setting[0].id_sence[0],Sw_Flash_Data_Val.input_setting[0].id_sence[1]);
	RD_ev_log("2: mode: %d,sence0: %d,sence1: %d\n",Sw_Flash_Data_Val.input_setting[1].mode,Sw_Flash_Data_Val.input_setting[1].id_sence[0],Sw_Flash_Data_Val.input_setting[1].id_sence[1]);
	RD_ev_log("3: mode: %d,sence0: %d,sence1: %d\n",Sw_Flash_Data_Val.input_setting[2].mode,Sw_Flash_Data_Val.input_setting[2].id_sence[0],Sw_Flash_Data_Val.input_setting[2].id_sence[1]);
	RD_ev_log("4: mode: %d,sence0: %d,sence1: %d\n",Sw_Flash_Data_Val.input_setting[3].mode,Sw_Flash_Data_Val.input_setting[3].id_sence[0],Sw_Flash_Data_Val.input_setting[3].id_sence[1]);
	RD_ev_log("adc: %d %d %d %d\n",Sw_Flash_Data_Val.adc_setting.adc_threshold,Sw_Flash_Data_Val.adc_setting.id_sence,Sw_Flash_Data_Val.adc_setting.type,Sw_Flash_Data_Val.adc_setting.delta);
	RD_ev_log("linked-powup: 1-%d-%d, 2-%d-%d\n\n",Sw_Flash_Data_Val.output_linked[0].input,Sw_Flash_Data_Val.output_linked[0].pow_up,Sw_Flash_Data_Val.output_linked[1].input,Sw_Flash_Data_Val.output_linked[1].pow_up);
	RD_init_flash_out_handle();

	if(get_provision_state() == STATE_DEV_PROVED)
	{
		for(int i=0; i<NUM_OF_ELEMENT; i++)
		{
			u8 idx_in_link = Sw_Flash_Data_Val.output_linked[i].input;
			if(idx_in_link < NUM_OF_INPUT && Sw_Flash_Data_Val.input_setting[idx_in_link].mode == SYNC_PRESS_STATE)
			{
				u8 stt = rd_read_input(idx_in_link);
				light_onoff_idx(i,stt, 0);
				set_on_power_up_onoff(i, 0, stt);
				rd_init_onoff_relay(stt,i);
			}
			else
			{
				u8 pow_start = get_power_start_output(i);
				if(pow_start == RD_PowUpOff || pow_start == RD_PowUpOn)
				{
					light_onoff_idx(i,pow_start, 0);
					set_on_power_up_onoff(i, 0, pow_start); // save for POWer_up
					rd_init_onoff_relay(pow_start,i);
				}
				else if(pow_start == RD_PowUpStore)
				{
					u8 state = RD_get_on_off(i,0);
					rd_init_onoff_relay(state,i);
				}
			}
		}
		rd_rsp_state_output(Sw_Flash_Data_Val.Gw_Add);
	}
	else
	{
//		RD_Flash_Save_DataDefault();
		for(int i=0; i< NUM_OF_ELEMENT; i++)
		{
			light_onoff_idx(i,0, 0);
			set_on_power_up_onoff(i, 0, 0); // save for POWer_up
			rd_init_onoff_relay(0,i);
		}
	}
}

void rd_init_linked()
{

}
u8 get_state_secure()
{
	return Sw_Flash_Data_Val.Secure_RD;
}


void RD_Flash_Init(void)
{
	RD_Flash_Data_Init();
}

void RD_Flash_Save_Secure(uint8_t Secure_Stt)
{
	Sw_Flash_Data_Val.Secure_RD = Secure_Stt;
	rd_flash_save();
}


void RD_Flash_SaveGwAdd(uint16_t Gw_Add)
{
	Sw_Flash_Data_Val.Gw_Add = Gw_Add;
	rd_flash_save();
	RD_ev_log("save gw :%d\n",Gw_Add);
}

u8 rd_save_mode_input(u8 idx, u8 mode)
{
	if(idx > 0 && idx <= NUM_OF_INPUT  && mode <= INPUT_STATE_MAX)
	{
		Sw_Flash_Data_Val.input_setting[idx-1].mode = mode ;
		Sw_Flash_Data_Val.input_setting[idx-1].id_sence[0] = 0;
		Sw_Flash_Data_Val.input_setting[idx-1].id_sence[1] = 0;

		if(mode == SYNC_PRESS_STATE)
		{
			u8 stt = rd_read_input(idx -1);
			rd_on_off_led(idx -1,stt);
			u8 id_out = get_ele_linked(idx-1);
			if(id_out < NUM_OF_ELEMENT)
			{
				rd_onoff_relay(id_out,stt,1,0);
			}
		}
		else if(mode == PRESS_RELEASE_STATE)
		{
			rd_on_off_led(idx -1,0);
		}

		rd_flash_save();
		return 1;
	}
	return 0;
}

u8 rd_save_linked_io(u8 idx_in,u8 idx_out)
{
	if(idx_in <= NUM_OF_INPUT && idx_out >0 && idx_out <= NUM_OF_ELEMENT)
	{
		if(idx_in >0)
		{
			for(u8 i=0; i<NUM_OF_ELEMENT; i++)
			{
				if(Sw_Flash_Data_Val.output_linked[i].input == idx_in -1)
				{
					Sw_Flash_Data_Val.output_linked[i].input = 0xff;
				}
			}
			if(Sw_Flash_Data_Val.input_setting[idx_in-1].mode == SYNC_PRESS_STATE)
			{
				u8 stt = rd_read_input(idx_in -1);
				rd_onoff_relay(stt,idx_out-1,1,0);
			}
		}
		Sw_Flash_Data_Val.output_linked[idx_out -1].input= (idx_in > 0) ? (idx_in-1) : 0xff;
		rd_flash_save();
		return 1;
	}
	return 0;
}

u8 rd_save_powerup_cf(u8 idx_out,u8 pow_cf)
{
	if(idx_out >0 && idx_out <= NUM_OF_ELEMENT && pow_cf <= RD_POW_SET_MAX && NUM_OF_ELEMENT <= 4)
	{
		Sw_Flash_Data_Val.output_linked[idx_out-1].pow_up = pow_cf;
		rd_flash_save();
		return 1;
	}
	return 0;
}

u8 rd_save_sence_in(u8 idx_in, u8 stt_sence,u16 id_sence)
{
	if(idx_in <= NUM_OF_INPUT && stt_sence <= MODE_MAX)
	{
		if(Sw_Flash_Data_Val.input_setting[idx_in -1].mode == PRESS_RELEASE_STATE )
		{
			if(stt_sence == MODE_PULSING)
			{
				Sw_Flash_Data_Val.input_setting[idx_in -1].id_sence[LOGIC_RAISING] = id_sence;
				rd_flash_save();
				return 1;
			}
		}
		else if(Sw_Flash_Data_Val.input_setting[idx_in -1].mode == SYNC_PRESS_STATE)
		{
			if(stt_sence < LOGIC_STT_MAX)
			{
				Sw_Flash_Data_Val.input_setting[idx_in -1].id_sence[stt_sence] = id_sence;
				rd_flash_save();
				return 1;
			}
		}
	}
	return 0;
}

u8 rd_save_sence_adc(u16 adc_threshold, u16 id_sence,u8 type)
{
	Sw_Flash_Data_Val.adc_setting.adc_threshold = adc_threshold;
	Sw_Flash_Data_Val.adc_setting.id_sence = id_sence;
	Sw_Flash_Data_Val.adc_setting.type = type;
	rd_flash_save();
	return 1;
}

u8 rd_save_delta_adc(u8 delta)
{
	if(delta >4 && delta <100)
	{
		Sw_Flash_Data_Val.adc_setting.delta = delta;
		rd_flash_save();
		return 1;
	}
	return 0;
}

u16 get_delta_adc()
{
	return Sw_Flash_Data_Val.adc_setting.delta * MAX_ADC / 100;
}

u8 rd_check_state_adc(u16 adc_value)
{
	if(Sw_Flash_Data_Val.adc_setting.type == TYPE_ADC_GREATER)
		return (adc_value > Sw_Flash_Data_Val.adc_setting.adc_threshold);
	else if(Sw_Flash_Data_Val.adc_setting.type == TYPE_ADC_LOWER)
		return (adc_value < Sw_Flash_Data_Val.adc_setting.adc_threshold);
	return 0;
}

