#include "rd_flash.h"
#include "../RD_in_out/rd_in_out.h"


extern uint16_t RD_GATEWAYADDRESS;
uint8_t Jump_Flash=0x00;												// val to set add Flash for Power Off data
uint32_t Counter_Flash_Power_Eraser =0x00000000;

Sw_Flash_Data Sw_Flash_Data_Val = { 0 };

u8 get_mode_setting_input(u8 idx)
{
	if(idx >= 0 && idx < NUM_OF_INPUT)
	{
		return Sw_Flash_Data_Val.input_setting[idx].mode;
	}
	return 0xff;
}

u8 get_stt_sence(u8 idx)
{
	if(idx >= 0 && idx < NUM_OF_INPUT )
	{
		if(Sw_Flash_Data_Val.input_setting[idx].id_sence != 0)
			return Sw_Flash_Data_Val.input_setting[idx].stt_sence;
	}
	return 0xff;
}

u16 get_sence_input(u8 idx)
{
	if(idx >= 0 && idx < NUM_OF_INPUT)
	{
		return Sw_Flash_Data_Val.input_setting[idx].id_sence;
	}
	return 0;
}

u8 get_ele_linked(u8 idx)
{
	for( u8 i=0; i<NUM_OF_ELEMENT; i++)
	{
		if(Sw_Flash_Data_Val.output_linked[i] == idx)
		{
			return i;
		}
	}
	return 0xff;
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
	Sw_Flash_Data_Val.PowerUpStt = RD_PowUpStore;
//	for(u8 i= 0 ; i<NUM_OF_INPUT; i++)
//	{
//		Sw_Flash_Data_Val.input_setting[i].mode = 0xff;
//	}
	for( u8 i=0; i<NUM_OF_ELEMENT; i++)
	{
		Sw_Flash_Data_Val.output_linked[i] = 0xff;
	}
	rd_flash_save();
#if UART_ON

	uart_CSend("Save Data default\n");

#endif
}

static void RD_Flash_Data_Init(void) {
	flash_read_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val),
			(uint8_t *) &Sw_Flash_Data_Val.Factory_Check);
	if (Sw_Flash_Data_Val.Factory_Check == 0xFF) // fist time to read reset memory
	{
		RD_Flash_Save_DataDefault();
	}
	RD_GATEWAYADDRESS = Sw_Flash_Data_Val.Gw_Add;

	if (RD_GATEWAYADDRESS == 0x0000)
	{
		RD_GATEWAYADDRESS = 0x0001;
	}

	RD_ev_log("Sw_Flash_Data: Sec %d,Pup %d,gw: %d\n",Sw_Flash_Data_Val.Secure_RD,Sw_Flash_Data_Val.PowerUpStt,Sw_Flash_Data_Val.Gw_Add);
	RD_ev_log("1: mode: %d,stt: %d,sence %d\n",Sw_Flash_Data_Val.input_setting[0].mode,Sw_Flash_Data_Val.input_setting[0].stt_sence,Sw_Flash_Data_Val.input_setting[0].id_sence);
	RD_ev_log("2: mode: %d,stt: %d,sence %d\n",Sw_Flash_Data_Val.input_setting[1].mode,Sw_Flash_Data_Val.input_setting[1].stt_sence,Sw_Flash_Data_Val.input_setting[1].id_sence);
	RD_ev_log("3: mode: %d,stt: %d,sence %d\n",Sw_Flash_Data_Val.input_setting[2].mode,Sw_Flash_Data_Val.input_setting[2].stt_sence,Sw_Flash_Data_Val.input_setting[2].id_sence);
	RD_ev_log("4: mode: %d,stt: %d,sence %d\n",Sw_Flash_Data_Val.input_setting[3].mode,Sw_Flash_Data_Val.input_setting[3].stt_sence,Sw_Flash_Data_Val.input_setting[3].id_sence);
	RD_ev_log("adc: %d %d %d\n",Sw_Flash_Data_Val.adc_setting.adc_threshold,Sw_Flash_Data_Val.adc_setting.id_sence,Sw_Flash_Data_Val.adc_setting.type);
	RD_ev_log("linked: 1-%d, 2-%d\n\n",Sw_Flash_Data_Val.output_linked[0],Sw_Flash_Data_Val.output_linked[1]);
	if(get_provision_state() == STATE_DEV_PROVED)
	{
		#if(CONFIG_POWUP_EN)
			uint8_t PowUpStt =0;
			if(RD_PowUpOff == Sw_Flash_Data_Val.PowerUpStt) PowUpStt =0;
			if(RD_PowUpOn == Sw_Flash_Data_Val.PowerUpStt) PowUpStt =1;

			if(RD_PowUpStore != Sw_Flash_Data_Val.PowerUpStt)
			{
				uart_CSend("  Set PowUp \n");
				for(int i=0; i< NUM_OF_ELEMENT; i++)
				{
					light_onoff_idx(i,PowUpStt, 0);
					set_on_power_up_onoff(i, 0, PowUpStt); // save for POWer_up
//					RD_mod_io_onoff(PowUpStt,i,0);
				}
			}
			else
			{
				for(int i =0; i< NUM_OF_ELEMENT; i++)
				{
					u8 state = RD_get_on_off(i,0);
					RD_ev_log("out %d: %d\n",i,state);
//					RD_mod_io_onoff(state,i,0);
				}
			}
		#endif
	}
}

void RD_init_flash_out_handle()
{
	for(int i =0; i< NUM_OF_ELEMENT; i++)
	{
		u8 state = RD_get_on_off(i,0);
		RD_ev_log("out %d: %d\n",i,state);
//		RD_mod_io_onoff(state,i,0);
	}
}

void RD_Flash_Init(void)
{
	RD_Flash_Data_Init();

	RD_init_flash_out_handle();
//	RD_Flash_PowerUp_Init();
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
	RD_ev_log("save gw :%d",Gw_Add);
}


u8 rd_save_mode_input(u8 idx, u8 mode)
{
	if(idx <= NUM_OF_INPUT  && mode <= INPUT_STATE_MAX)
	{
		Sw_Flash_Data_Val.input_setting[idx-1].mode = mode ;
		rd_flash_save();
		return 1;
	}
	return 0;
}

u8 rd_save_linked_io(u8 idx_in,u8 idx_out)
{
	if(idx_in <= NUM_OF_INPUT && idx_out >0 && idx_out <= NUM_OF_ELEMENT)
	{
		Sw_Flash_Data_Val.output_linked[idx_out -1]= (idx_in > 0) ? (idx_in-1) : 0xff;
		rd_flash_save();
		return 1;
	}
	return 0;
}

u8 rd_save_powerup_cf(u8 pow_cf)
{
	if(pow_cf <= RD_POW_SET_MAX)
	{
		Sw_Flash_Data_Val.PowerUpStt = pow_cf;
		rd_flash_save();
		return 1;
	}
	return 0;
}

u8 rd_save_sence_in(u8 idx_in, u8 stt_sence,u16 id_sence)
{
	if(idx_in <= NUM_OF_INPUT && stt_sence <= MODE_MAX)
	{
		if(Sw_Flash_Data_Val.input_setting[idx_in -1].mode == PRESS_RELEASE_STATE  && stt_sence != MODE_PULSING)
			return 0;
		Sw_Flash_Data_Val.input_setting[idx_in -1].id_sence = id_sence;
		Sw_Flash_Data_Val.input_setting[idx_in -1].stt_sence = stt_sence;
		rd_flash_save();
		return 1;
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

u8 rd_check_state_adc(u16 adc_value)
{
	if(Sw_Flash_Data_Val.adc_setting.type == TYPE_ADC_GREATER)
		return (adc_value > Sw_Flash_Data_Val.adc_setting.adc_threshold);
	else if(Sw_Flash_Data_Val.adc_setting.type == TYPE_ADC_LOWER)
		return (adc_value < Sw_Flash_Data_Val.adc_setting.adc_threshold);
	return 0;
}
