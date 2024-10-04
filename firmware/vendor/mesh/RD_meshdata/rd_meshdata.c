#include "rd_meshdata.h"

#include "../RD_secure/rd_secure.h"
#include "../RD_log/RD_log.h"

uint8_t Train_Factory =0;
extern STT_Relay_Mess STT_Relay_Mess_Config;
extern void mesh_g_onoff_st_rsp_par_fill(mesh_cmd_g_onoff_st_t *rsp, u8 idx);
uint8_t reset_Mess_Flag =0;
unsigned char vr_RD_ProvDone;

uint16_t RD_GATEWAYADDRESS  = GW_ADD_DEFAULT;
uint16_t RD_ELEMENT_ADDR_1	= 0x0000;
uint16_t RD_ELEMENT_ADDR_2	= 0x0000;
uint16_t RD_ELEMENT_ADDR_3	= 0x0000;
uint16_t RD_ELEMENT_ADDR_4	= 0x0000;

void Send_Relay_Stt_Message_RALL_PowerUp(void)
{
	uint8_t Mess_Buff[8] = {0};

	u8 relay_stt_buff[5] ={0};
	for(int i=0; i<LIGHT_CNT; i++)
	{
		mesh_cmd_g_onoff_st_t onoff_stt = {0};

		mesh_g_onoff_st_rsp_par_fill(&onoff_stt, i);
		relay_stt_buff[i] = onoff_stt.present_onoff;
	}
	Mess_Buff[0]  = RD_HEADER_SWITCH_TOUCH_STT & 0xFF;
	Mess_Buff[1]  = RD_HEADER_SWITCH_TOUCH_STT >>8;
	Mess_Buff[2]  = NUM_OF_ELEMENT;
	Mess_Buff[3]  = relay_stt_buff[0];
	Mess_Buff[4]  = relay_stt_buff[1];
	Mess_Buff[5]  = relay_stt_buff[2];
	Mess_Buff[6]  = relay_stt_buff[3];
	Mess_Buff[7]  = relay_stt_buff[4];

	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP, Mess_Buff, 8, RD_GATEWAYADDRESS, RD_MAXRESPONESEND); //SENSOR_STATUS

}

/*----------------------------------- E0 mess handle----------------------------------------*/
/**
 * @brief Respone Secure mess.
 *
 * @param Secure_Rsp_Stt RD_Success: Respone secure Pass to gateway
 * 						 RD_Failure: Respone secure Fail to gateway
 * @param Gw_Add_Buff 		gateway address to send
 */
static void RD_Mess_RspSecure(uint8_t Secure_Rsp_Stt, uint16_t Gw_Add_Buff)
{
	uint8_t Mess_buff[8] = {0};

	if(Secure_Rsp_Stt == RD_EN)
	{
		Mess_buff[0]	= RD_HEADER_RSP_TYPE_DEVICE & 0xff;
		Mess_buff[1]	= (RD_HEADER_RSP_TYPE_DEVICE >> 8) & 0xff;
		Mess_buff[2]	= Device_BROADCRARD_0;
		Mess_buff[3]	= Device_BROADCRARD_1;
		Mess_buff[4]	= Device_BROADCRARD_2;
		Mess_buff[5]	= 0x00;
		Mess_buff[6] 	= VERSION_FIRM_H;
		Mess_buff[7]	= VERSION_FIRM_L;
		mesh_tx_cmd2normal_primary(RD_OPCODE_TYPE_RSP, Mess_buff, 8, Gw_Add_Buff, RD_MAXRESPONESEND);
		uart_CSend("Rsp Secure success \n");
	}
	else
	{
		Mess_buff[0]	= RD_HEADER_RSP_TYPE_DEVICE & 0xff;
		Mess_buff[1]	= (RD_HEADER_RSP_TYPE_DEVICE >> 8) & 0xff;
		Mess_buff[2]	= 0xFF;
		Mess_buff[3]	= 0xFE;
		Mess_buff[4]	= 0xFF;
		Mess_buff[5]	= 0xFE;
		Mess_buff[6] 	= 0xFF;
		Mess_buff[7]	= 0xFE;
		mesh_tx_cmd2normal_primary(RD_OPCODE_TYPE_RSP, Mess_buff, 8, Gw_Add_Buff, RD_MAXRESPONESEND);
		uart_CSend("Rsp Secure failed \n");
	}
}

/**
 * @brief Handle mess type device.  check secure key and rsp
 *
 * @param par
 * @param Gw_Add_Buff
 */
static void RD_Handle_MessType(uint8_t par[8], uint16_t Gw_Add_Buff)
{
	uint8_t Secure_return =0;
	#if(SECURE_ON ==1)

		if(get_provision_state() == STATE_DEV_PROVED)
		{
			Secure_return = RD_Secure_AesreCheck(&RD_key[0], &tbl_mac[0], ele_adr_primary, &par[2]);

			if(Secure_return == 1)
			{
				uart_CSend("Secure key Pass \n");
				RD_Flash_Save_Secure(RD_EN);			// save secure pass to flash memory
				RD_Secure_Set(RD_EN);					// set secure stt pass
				RD_Mess_RspSecure(RD_EN, Gw_Add_Buff);
			}
			else
			{
				uart_CSend("Secure key fail \n");
				RD_Flash_Save_Secure(RD_DIS);			// save secure pass to flash memory
				RD_Secure_Set(RD_DIS);					// set secure stt pass
				RD_Mess_RspSecure(RD_DIS, Gw_Add_Buff);
				RD_Secure_Check_Key_Fail();

			}
		}
		else
		{
			RD_Mess_RspSecure(RD_EN, Gw_Add_Buff);
		}
	#else
	Secure_return =1;
	RD_Flash_Save_Secure(RD_EN);			// save secure pass to flash memory
	RD_Mess_RspSecure(RD_EN, Gw_Add_Buff);
	#endif
}

static void RD_Handle_SaveGw(uint8_t par[8], uint16_t Gw_Add_Buff)
{
	uint16_t GwAddToSave = par[3]<<8 | par[2];

	if(GwAddToSave == 0x0000) 	{RD_GATEWAYADDRESS = Gw_Add_Buff;} // save gw add send mess
	else						{RD_GATEWAYADDRESS = GwAddToSave;} // save other gw add

	char UART_TempSend[64];
	sprintf(UART_TempSend,"Save GWAdd:%x \n", RD_GATEWAYADDRESS);
	uart_CSend(UART_TempSend);
	if(get_provision_state() == STATE_DEV_PROVED)
	{
		RD_Flash_SaveGwAdd(RD_GATEWAYADDRESS);
		char UART_TempSend[64];
		sprintf(UART_TempSend,"Save GWAdd to Flash :%x \n", RD_GATEWAYADDRESS);
		uart_CSend(UART_TempSend);
	}
	else
	{
		char UART_TempSend[64];
		sprintf(UART_TempSend,"Save GWAdd to Ram for test :%x \n", RD_GATEWAYADDRESS);
		uart_CSend(UART_TempSend);
	}
	uint8_t Mess_buff[8] = {0};

	Mess_buff[0]	= RD_HEADER_SAVE_GW_ADD 				& 0xff;
	Mess_buff[1]	= (RD_HEADER_SAVE_GW_ADD >> 8) 			& 0xff;
	Mess_buff[2]	= Sw_Flash_Data_Val.Gw_Add		 		& 0xff;
	Mess_buff[3]	= (Sw_Flash_Data_Val.Gw_Add >> 8) 		& 0xff;
	Mess_buff[4]	= PROVIDER_ID  							& 0xff;
	Mess_buff[5]	= (PROVIDER_ID >> 8) 					& 0xff;
	Mess_buff[6] 	= 0x00;
	Mess_buff[7]	= 0x00;

	mesh_tx_cmd2normal_primary(RD_OPCODE_TYPE_RSP, Mess_buff, 8, Gw_Add_Buff, RD_MAXRESPONESEND); // rsp to Gw send mess
//	RD_SwitchAc4Ch_BlinkSet(5, 100);
}


void RD_Update_Relay_Stt(uint8_t Relay_ID)
{
//	SETMIN(Relay_ID, 1);
#if(0)
	uint8_t Mess_Buff[8] = {0};
	uint16_t Element_Add = 0x0000;

	Mess_Buff[0]		= relay_Stt[Relay_ID-1];
	Mess_Buff[1]		= 0x0A;
	Element_Add 		= ele_adr_primary + (Relay_ID -1);

	mesh_tx_cmd2normal(G_ONOFF_STATUS, Mess_Buff, 2, Element_Add, RD_GATEWAYADDRESS, 3);

	char UART_TempSend[128];
	sprintf(UART_TempSend,"Update On-Off Gw:0x%x- Relay: %d--%d--%d--%d element: %x \n",RD_GATEWAYADDRESS, relay_Stt[0], relay_Stt[1], relay_Stt[2], relay_Stt[3], Element_Add);
	uart_CSend(UART_TempSend);
#endif

	uint8_t Mess_Buff[8] = {0};

	Mess_Buff[0] 	= RD_HEADER_REUP_STT & 0xFF ;
	Mess_Buff[1]	= RD_HEADER_REUP_STT >>8;
	Mess_Buff[2]  	= NUM_OF_ELEMENT;
	Mess_Buff[3]  	= relay_Stt[0];
	Mess_Buff[4]  	= relay_Stt[1];
	Mess_Buff[5]  	= relay_Stt[2];
	Mess_Buff[6]  	= relay_Stt[3];
	Mess_Buff[7]  	= relay_Stt[4];

	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP, Mess_Buff, 8, RD_GATEWAYADDRESS, RD_MAXRESPONESEND); //SENSOR_STATUS
	uart_CSend("update stt \n");
}

static void RD_Hanlde_FactoryTestEnd(uint16_t Gw_Add_Buff)
{
	uint8_t Mess_buff[8] = {0};

	Mess_buff[0]	= RD_HEADER_FACTORY_TEST_END & 0xff;
	Mess_buff[1]	= (RD_HEADER_FACTORY_TEST_END >> 8) & 0xff;
	Mess_buff[2]	= 0x00;
	Mess_buff[3]	= 0x00;
	Mess_buff[4]	= 0x00;
	Mess_buff[5]	= 0x00;
	Mess_buff[6] 	= 0x00;
	Mess_buff[7]	= 0x00;

	mesh_tx_cmd2normal_primary(RD_OPCODE_TYPE_RSP, Mess_buff, 8, Gw_Add_Buff, RD_MAXRESPONESEND);

	uint16_t GWAdresss = 0x0000;
	GWAdresss = 0x0001;				// rRess
	RD_Flash_SaveGwAdd(GWAdresss);
	reset_Mess_Flag = 1;
//	RD_SwitchAc4Ch_BlinkSet(5, 100);
}

static void RD_Handle_KickAll(uint8_t par[8], uint16_t Gw_Add_Buff)
{
	if((par[2]==0x01) && (par[3]==0x02) && (par[4]==0x03) && (par[5]==0x04) && (par[6]==0x05) && (par[7]==0x06))
	{
		uart_CSend("Reset All device. countdown 60s to reset \n");
		Kick_all_Flag=1;
		mesh_tx_cmd2normal_primary(RD_OPCODE_TYPE_RSP, par, 8, Gw_Add_Buff, RD_MAXRESPONESEND);
//		RD_SwitchAc4Ch_BlinkSet(5, 100);
	}
}


int RD_Messenger_ProcessCommingProcess_Type(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par){
	#if UART_ON
		uart_CSend("Co ban tin");
		char UART_TempSend[128];
		sprintf(UART_TempSend,"Messenger type:%d-Content:%x-%x-%x-%x-%x-%x-%x-%x\n",par_len,par[0],par[1],par[2],par[3],par[4],par[5],par[6],par[7]);
		uart_CSend(UART_TempSend);
		uart_CSend("..\n");
	#endif

	uint16_t Gw_Add_Buff =0x00;
	Gw_Add_Buff = cb_par->adr_src;

	uint16_t header_Buff = 0x0000;
	header_Buff = (par[1] << 8 | par[0]);
	switch (header_Buff)
	{
		case(RD_HEADER_RSP_TYPE_DEVICE):
			RD_Handle_MessType(par, Gw_Add_Buff);
			break;
		case(RD_HEADER_SAVE_GW_ADD):
			RD_Handle_SaveGw(par, Gw_Add_Buff);
			break;
		case(RD_HEADER_FACTORY_TEST_END):
			RD_Hanlde_FactoryTestEnd( Gw_Add_Buff);
			break;
		case(RD_HEADER_KICK_ALL):
			RD_Handle_KickAll(par, Gw_Add_Buff);
			break;
		default:
			uart_CSend("Wrong header E0 \n");
			break;
	}
	return 0;
}


