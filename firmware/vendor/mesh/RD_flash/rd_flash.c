#include "rd_flash.h"
#include "../RD_in_out/rd_in_out.h"


extern uint16_t RD_GATEWAYADDRESS;
uint8_t Jump_Flash=0x00;												// val to set add Flash for Power Off data
uint32_t Counter_Flash_Power_Eraser =0x00000000;

Sw_Flash_Data Sw_Flash_Data_Val = { 0 };

void RD_Flash_Save_DataDefault(void) {
	memset((void *)&Sw_Flash_Data_Val, 0 , sizeof(Sw_Flash_Data_Val));

	Flash_Data_Buff.Gw_Add = GW_ADD_DEFAULT;
	Flash_Data_Buff.PowerUpStt = RD_PowUpStore;

	flash_erase_sector(FLASH_ADDR);
	flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val),(uint8_t *) &Sw_Flash_Data_Val.Factory_Check);
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
					RD_mod_io_onoff(PowUpStt,i,0);
				}
			}
			else
			{
				for(int i =0; i< NUM_OF_ELEMENT; i++)
				{
					u8 state = RD_get_on_off(i,0);
					RD_ev_log("out %d: %d\n",i,state);
					RD_mod_io_onoff(state,i,0);
				}
			}
		#endif
	}

	char UART_TempSend[128];
	sprintf(UART_TempSend,"Flash data 0x780000: size: %d Secure:0x%x  \n GW add: 0x%x  \n  ",sizeof(Sw_Flash_Data_Val), Sw_Flash_Data_Val.Secure_RD, RD_GATEWAYADDRESS);
	uart_CSend(UART_TempSend);
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

	flash_erase_sector(FLASH_ADDR);
	flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );
}


void RD_Flash_SaveGwAdd(uint16_t Gw_Add)
{
	Sw_Flash_Data_Val.Gw_Add = Gw_Add;
	flash_erase_sector(FLASH_ADDR);
	flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );
#if UART_ON
	char UART_TempSend[128];
	sprintf(UART_TempSend,"Save GW Address: 0x%x\n", Sw_Flash_Data_Val.Gw_Add);
	uart_CSend(UART_TempSend);
#endif
}

void RD_Flash_PowUpConfig(uint8_t PowUpConfig)
{
	Sw_Flash_Data_Val.PowerUpStt = PowUpConfig;

	flash_erase_sector(FLASH_ADDR);
	flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );
#if UART_ON
	char UART_TempSend[128];
	sprintf(UART_TempSend,"PowUp Stt : %d\n", Sw_Flash_Data_Val.PowerUpStt);
	uart_CSend(UART_TempSend);
#endif
}

void RD_Flash_Save_LinkControl(uint8_t Button_Set, uint16_t Add_Control)
{
	Sw_Flash_Data_Val.Add_Link_Control[Button_Set -1] 	= Add_Control;
	flash_erase_sector(FLASH_ADDR);
	flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );

	#if UART_ON
		char UART_TempSend[128];
		sprintf(UART_TempSend,"Save Scene Stair for button: %d\n",	Button_Set-1);
		uart_CSend(UART_TempSend);
	#endif

}

void rd_handle_setting_input(u8 *par,int par_len, u16 gw_addr)
{

}

