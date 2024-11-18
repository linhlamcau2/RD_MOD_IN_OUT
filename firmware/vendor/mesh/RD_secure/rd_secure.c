/*
 * RD_Secure.c
 *
 *  Created on: Jul 5, 2022
 *      Author: Hao PC
 */

#include "rd_secure.h"

Secure_Stt_Str Secure_Stt_Val = {0};
unsigned char RD_key[16] = "Digital@28112804"; //6B 65 79 52 61 6E 67 44 6F 6E 67 5F 52 6E 44 44
unsigned char SN_key[16] = "SONGNAM@RAL@1235";
unsigned char HUE_key[16] = "HUEPRESS@RAL@123"; // 48 55 45 50 52 45 53 53 40 52 41 4c 40 31 32 33
//unsigned char Secure_key[16] = HUE_key[16];
unsigned char param_RD[8] = {0x24,0x02,0x28,0x04,0x28,0x11,0x20,0x20}; // 8 byte selfgen by RD
unsigned char RD_mac[6] = {0};

/**
 * @brief Assembly param_RD - mac - Unicast to 16byte Result key
 *
 * @param param_RD : 8 btye const by RD define
 * @param mac 		: 6 byte mac ID of device
 * @param unicast_ID : 2 byte unicast ID proved after provision
 * @param result_Key : 16 byte result_Key
 */
static void RD_Secure_Assembly_For_KeyIn(uint8_t param_RD[8], uint8_t mac[6], uint16_t unicast_ID, unsigned char *result_Key)
{
	for(unsigned char i=0;i<8;i++){
		result_Key[i] = param_RD[i];
	}

	for(unsigned char i=0;i<6;i++){
		result_Key[i+8] = mac[i];
	}

	result_Key[14] = unicast_ID & 0xFF;
	result_Key[15] = (unicast_ID >> 8) & 0xff;
}
//
unsigned char RD_Secure_AesreCheck(uint8_t key[16], uint8_t mac[6], uint16_t unicast, uint8_t compare_key[6])
{
	unsigned char compareBuff[16] = {0};
	unsigned char aesEncrypt[16] = {0};

	RD_Secure_Assembly_For_KeyIn(param_RD, &mac[0], unicast, &compareBuff[0]);

	if(aes_encrypt(&key[0],compareBuff,aesEncrypt)!= 0) uart_CSend("\n gen key fail \n: ");

	for(unsigned char i=0;i< 6;i++){
		if(aesEncrypt[i+10] != compare_key[i]){
			return 0;
		}
	}
	return 1;
}
//
//
void RD_Secure_CheckLoop(void)
{
	if((Secure_Stt_Val.Secure_Pass != 1) && (get_provision_state() == STATE_DEV_PROVED)) 	// check secure if device in state provided and secure faile
	{
		if(Secure_Stt_Val.Check_Secure_Ready == 0)
		{
			uart_CSend("Provisioned Start waiting for secure key \n");
			Secure_Stt_Val.Check_Secure_Ready 	= 1;
			Secure_Stt_Val.Time_Start_check 	= clock_time_ms();			// set time Start check secure
		}

		if( ((clock_time_ms() - Secure_Stt_Val.Time_Start_check) >=  TIME_UOT_SECURE) && (Sw_Working_Stt_Val.OTA_Flag == 0x00))
		{
			uart_CSend("Secure Time out.\n");
			//RD_Sw_FacReset_ByUser();
//			RD_Switchx4_FactoryReset();
			RD_mod_in_out_factory_reset();
		}

		if(((clock_time_ms() - Secure_Stt_Val.Time_Check_Key_Fail) >=  TIME_UOT_KEY_FAIL) && (Secure_Stt_Val.Check_Key_Fail == RD_EN))
		{
			uart_CSend("Secure Key fail\n");
			//RD_Sw_FacReset_ByUser();
//			RD_Switchx4_FactoryReset();
			RD_mod_in_out_factory_reset();
		}

	}
}
//
static void RD_Secure_Check(void)
{
	unsigned char Key_in[16] = {0x44, 0x69, 0x67, 0x69, 0x74, 0x61, 0x6C, 0x40, 0x32, 0x38, 0x31, 0x31, 0x32, 0x38, 0x30, 0x34};
	unsigned char Param[16] = {0x24, 0x02, 0x28, 0x04, 0x28, 0x11, 0x20, 0x20, 0x5D, 0x14, 0x30, 0x38, 0xC1, 0xA4, 0x39, 0x00};
	unsigned char Result[32] = {0x00};
	aes_encrypt(Key_in,Param,Result);
	uart_CSend("\nkey out: ");
	for(int i=0; i<32; i++)
	{
		char UART_TempSend[128];
		sprintf(UART_TempSend,"0x%x -",Result[i]);
		uart_CSend(UART_TempSend);
	}
}

void RD_Secure_CheckInit(void)
{
	Secure_Stt_Val.Secure_Pass = Sw_Flash_Data_Val.Secure_RD;

	if((Secure_Stt_Val.Secure_Pass == FALSE) && (get_provision_state() == STATE_DEV_PROVED))
	{
		uart_CSend("Secure Init fail. reset factory");
		RD_mod_in_out_factory_reset();
	}
	else uart_CSend("Secure Init pass\n");

	RD_Secure_Check();
}
//
void RD_Secure_Set(uint8_t Secure_Pass_Stt)
{
	Secure_Stt_Val.Secure_Pass = Secure_Pass_Stt;
}

void RD_Secure_Check_Key_Fail(void)
{
	Secure_Stt_Val.Check_Key_Fail = RD_EN;
	Secure_Stt_Val.Time_Check_Key_Fail = clock_time_ms();
}

