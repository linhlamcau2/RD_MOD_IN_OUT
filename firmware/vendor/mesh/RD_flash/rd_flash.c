#include "rd_flash.h"
#include "../RD_in_out/rd_in_out.h"

Sw_Flash_Data Sw_Flash_Data_Val = { 0 };

void RD_Flash_Save_DataDefault(void) {
	Sw_Flash_Data Flash_Data_Buff = { 0 };
	Flash_Data_Buff.Factory_Check = 0x00;
	Flash_Data_Buff.Gw_Add = GW_ADD_DEFAULT;
	Flash_Data_Buff.Secure_RD = RD_Failure;
	Flash_Data_Buff.PowerUpStt = RD_PowUpStore;

	Flash_Data_Buff.Type_CtrLocal_K9B = K9B_TYPE_CTR_LOCAL_DF;
	flash_erase_sector(FLASH_ADDR);
	flash_write_page(FLASH_ADDR, sizeof(Flash_Data_Buff),
			(uint8_t *) &Flash_Data_Buff.Factory_Check);
	flash_read_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val),
			(uint8_t *) &Sw_Flash_Data_Val.Factory_Check);

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
	char UART_TempSend[128];
	sprintf(UART_TempSend,"Flash data 0x780000: size: %d Secure:0x%x  \n GW add: 0x%x  \n  ",sizeof(Sw_Flash_Data_Val), Sw_Flash_Data_Val.Secure_RD, RD_GATEWAYADDRESS);
	uart_CSend(UART_TempSend);
}

void RD_Flash_Init(void)
{
	RD_Flash_Data_Init();
}

void RD_Flash_Save_Secure(uint8_t Secure_Stt)
{
	Sw_Flash_Data_Val.Secure_RD = RD_Success;

	flash_erase_sector(FLASH_ADDR);
	flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );
}


void RD_Flash_SaveGwAdd(uint16_t Gw_Add)
{
	Sw_Flash_Data_Val.Gw_Add = Gw_Add;

	// Flash_Data_Buff  = (uint8_t *) (&Sw_Flash_Data_Val);
   	// flash_erase_sector(FLASH_ADDR);
	// flash_write_page(FLASH_ADDR, FLASH_DATA_SIZE,Flash_Data_Buff );
	flash_erase_sector(FLASH_ADDR);
	flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );
#if UART_ON
	char UART_TempSend[128];
	sprintf(UART_TempSend,"Save GW Address: 0x%x\n", Sw_Flash_Data_Val.Gw_Add);
	uart_CSend(UART_TempSend);

#endif


}
