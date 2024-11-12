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

	Mess_buff[0]	= RD_HEADER_RSP_TYPE_DEVICE & 0xff;
	Mess_buff[1]	= (RD_HEADER_RSP_TYPE_DEVICE >> 8) & 0xff;
	Mess_buff[2]	= Secure_Rsp_Stt ? Device_BROADCRARD_0 : 0xFF;
	Mess_buff[3]	= Secure_Rsp_Stt ? Device_BROADCRARD_1 : 0xFE;
	Mess_buff[4]	= Secure_Rsp_Stt ? Device_BROADCRARD_2 : 0xFF;
	Mess_buff[5]	= Secure_Rsp_Stt ? 0x00: 0xFE ;
	Mess_buff[6] 	= Secure_Rsp_Stt ? VERSION_FIRM_H : 0xFF;
	Mess_buff[7]	= Secure_Rsp_Stt ? VERSION_FIRM_L : 0xFE;
	mesh_tx_cmd2normal_primary(RD_OPCODE_TYPE_RSP, Mess_buff, 8, Gw_Add_Buff, RD_MAXRESPONESEND);
	RD_ev_log("Rsp Secure : %d \n",Secure_Rsp_Stt);
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
	uint8_t Mess_Buff[8] = {0};

	Mess_Buff[0] 	= RD_HEADER_REUP_STT & 0xFF ;
	Mess_Buff[1]	= RD_HEADER_REUP_STT >>8;
	Mess_Buff[2]  	= NUM_OF_ELEMENT;
	for(int i = 0; i<NUM_OF_ELEMENT; i++)
	{
		Mess_Buff[i+3] = RD_get_on_off(i,0);
	}
	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP, Mess_Buff, 8, RD_GATEWAYADDRESS, RD_MAXRESPONESEND); //SENSOR_STATUS
	uart_CSend("update stt \n");
}


void rd_update_input_stt(u8 *stt,u16 adc,int num)
{
	uint8_t Mess_Buff[8] = {0};

	Mess_Buff[0] 	= RD_HEADER_INPUT_STT & 0xFF ;
	Mess_Buff[1]	= RD_HEADER_INPUT_STT >>8;

	for(int i =0; i<num; i++)
	{
		Mess_Buff[2] |= (stt[i] ? 1:0) << ((sizeof(u8) * 8)-1 -i);
	}
	Mess_Buff[3] = adc & 0xFF;
	Mess_Buff[3] = adc >>8;
	mesh_tx_cmd2normal_primary(RD_OPCODE_INPUT_RSP, Mess_Buff, 8, RD_GATEWAYADDRESS, RD_MAXRESPONESEND); //SENSOR_STATUS
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
		case RD_HEADER_SETTING_INPUT:
			rd_handle_setting_input(par,par_len,Gw_Add_Buff);
			break;
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

void RD_Send_Relay_Stt(uint8_t Relay_ID, uint8_t Relay_Stt)
{
	uint8_t Mess_Buff[8] = {0};
	uint16_t Element_Add = 0x0000;

	Mess_Buff[0]		= Relay_Stt;
	Element_Add 		= ele_adr_primary + (Relay_ID -1);

	mesh_tx_cmd2normal(G_ONOFF_STATUS, Mess_Buff, 1, Element_Add, RD_GATEWAYADDRESS, 2);

	char UART_TempSend[128];
	sprintf(UART_TempSend,"Messenger On-Off Gw:0x%x- Relay: %d--%d--%d--%d  \n",RD_GATEWAYADDRESS, relay_Stt[0], relay_Stt[1], relay_Stt[2], relay_Stt[3]);
	uart_CSend(UART_TempSend);
}


static void RD_Handle_CTR_SW(uint8_t par[8], uint16_t Gw_Add_Buff)
{
	uint8_t id_output 	= par[2];
	uint8_t Control_Stt		= par[3];

	Control_Stt   = (Control_Stt >1) ? 1:Control_Stt;
	if(id_output == 0xFF) // control All
	{
		for(int i=0; i<NUM_OF_ELEMENT; i++)
		{
			RD_SetAndRsp_Switch(i, Control_Stt, Gw_Add_Buff);
		}
	}
	else						// control single
	{
		id_output   = (id_output > 5) ? 5:id_output;

		RD_SetAndRsp_Switch(id_output -1, Control_Stt, Gw_Add_Buff);
	}

	#if UART_ON
		char UART_TempSend[128];
		sprintf(UART_TempSend,"Control SW: %x-%x-%x-%x-%x \n",relay_Stt[0], relay_Stt[1], relay_Stt[2], relay_Stt[3], relay_Stt[4]);
		uart_CSend(UART_TempSend);
	#endif
	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP, par, 8, Gw_Add_Buff, RD_MAXRESPONESEND);
}

void Send_Relay_Stt_Message_RALL(uint16_t GW_ADR)
{
	uint8_t Mess_Buff[8] = {0};

	Mess_Buff[0]  = RD_HEADER_SWITCH_TOUCH_STT & 0xFF;
	Mess_Buff[1]  = RD_HEADER_SWITCH_TOUCH_STT >>8;
	Mess_Buff[2]  = NUM_OF_ELEMENT;
	for(int i = 0; i<NUM_OF_ELEMENT; i++)
	{
		Mess_Buff[i+3] = RD_get_on_off(i,0);
	}
	if(GW_ADR == 0x0000)
	{
		GW_ADR = 0x0001;
	}
	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP, Mess_Buff, 8, GW_ADR, RD_MAXRESPONESEND); //SENSOR_STATUS
}


static void RD_Handle_PowUpConfig(uint8_t par[8], uint16_t Gw_Add_Buff)
{
	RD_PowUp_Type PowUpStt_Config = par[2];
	RD_Flash_PowUpConfig(PowUpStt_Config);

	uint8_t Mess_RspBuff[3] ={0};
	Mess_RspBuff[0]	= RD_HEADER_SW_POWUP_CONFIG 				& 0xFF;
	Mess_RspBuff[1]	= (RD_HEADER_SW_POWUP_CONFIG >> 8) 		& 0xFF;
	Mess_RspBuff[2]	= PowUpStt_Config;

	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP , &Mess_RspBuff[0], 3, Gw_Add_Buff, RD_MAXRESPONESEND);

//	RD_SwitchAc4Ch_BlinkSet(5, 100);
}

static void RD_GroupAuto(uint16_t groupID, mesh_cb_fun_par_t *cb_par, uint16_t OpGroup){
	mesh_cb_fun_par_t *cb_par_g = cb_par;
	cb_par_g->op = OpGroup;
	cb_par_g->op_rsp = CFG_MODEL_SUB_STATUS;
	cb_par_g->model_idx = 0;
	uint8_t parGroup[8] = {0};
	if(cb_par_g->adr_dst == 0xFFFF) cb_par_g->adr_dst = ele_adr_primary;

	for(int i=0; i< ELE_CNT; i++)
	{
		if(OpGroup == CFG_MODEL_SUB_ADD){
			parGroup[0] = (cb_par_g->adr_dst+i) & 0xff ;
			parGroup[1] = (cb_par_g->adr_dst+i) >> 8 & 0xff;
			parGroup[2] = groupID & 0xff;
			parGroup[3] = groupID >>8 & 0xff;
			parGroup[4] = 0x00;
			parGroup[5] = 0x10;

			mesh_cmd_sig_cfg_model_sub_set(parGroup, 6, cb_par_g);
		}
		else if(OpGroup == CFG_MODEL_SUB_DEL){
			parGroup[0] = (cb_par_g->adr_dst+i) & 0xff ;
			parGroup[1] = (cb_par_g->adr_dst+i) >> 8 & 0xff;
			parGroup[2] = groupID & 0xff;
			parGroup[3] = groupID >>8 & 0xff;
			parGroup[4] = 0x00;
			parGroup[5] = 0x10;

			mesh_cmd_sig_cfg_model_sub_set(parGroup, 6, cb_par_g);
		}
		else if(OpGroup == CFG_MODEL_SUB_DEL_ALL){
			parGroup[0] = (cb_par_g->adr_dst+i) & 0xff ;
			parGroup[1] = (cb_par_g->adr_dst+i) >> 8 & 0xff;
			parGroup[2] = 0x00;
			parGroup[3] = 0x10;

			mesh_cmd_sig_cfg_model_sub_set(parGroup, 4, cb_par_g);
		}
	}
}

static void RD_Handle_AutoCreateGr(u8 *par, uint16_t Gw_Add_Buff, mesh_cb_fun_par_t *cb_par)
{
	const uint16_t groupType 		= 0x001f; // switch Group
	uint16_t id_group = 0x0000;
	uint16_t id_group_type = 0x0000;
	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP, par, 8, Gw_Add_Buff, 2);

	id_group = (par[2] | par[3]<<8);
	id_group_type = id_group + groupType;

	RD_GroupAuto(id_group, cb_par, CFG_MODEL_SUB_ADD);
	RD_GroupAuto(id_group_type, cb_par, CFG_MODEL_SUB_ADD);

	uart_CSend("Auto create Group default \n");
//	RD_SwitchAc4Ch_BlinkSet(5, 100);
}

static void RD_Handle_AutoDeleteGr(u8 *par, uint16_t Gw_Add_Buff, mesh_cb_fun_par_t *cb_par)
{
	const uint16_t groupType 		= 0x001f; // switch Group
	uint16_t id_group = 0x0000;
	uint16_t id_group_type = 0x0000;
	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP, par, 8, Gw_Add_Buff, 2);

	id_group = (par[2] | par[3]<<8);
	id_group_type = id_group + groupType;

	RD_GroupAuto(id_group, cb_par, CFG_MODEL_SUB_DEL);
	RD_GroupAuto(id_group_type, cb_par, CFG_MODEL_SUB_DEL);

	uart_CSend("Auto delete Group default\n");
//	RD_SwitchAc4Ch_BlinkSet(5, 100);
}


static void RD_Handle_SetUp_LinkControl2(uint8_t par[8], uint16_t Gw_Add_Buff, uint16_t Receive_add)
{

	uint8_t Button_Set = 1;
	Button_Set   = Receive_add - ele_adr_primary +1;	// Switch Add to button

//	Button_Set	 = SETMAX(Button_Set, 4);
//	Button_Set	 = SETMIN(Button_Set, 1);

	uint16_t Add_Link_Control;
	Add_Link_Control 	= par[3]<<8 | par[2];

	if(Add_Link_Control == 0xffff)	Add_Link_Control =0x0000;
	RD_Flash_Save_LinkControl(Button_Set, Add_Link_Control);

	//RD_SwitchAc4Ch_BlinkSet(5, 100);
//	RD_Sw_SingleBlinkSet(Button_Set, 5, 300);
	//mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP, par, 4, Gw_Add_Buff, RD_MAXRESPONESEND);
	mesh_tx_cmd2normal(RD_OPCODE_SCENE_RSP, par, 4,Receive_add , Gw_Add_Buff, RD_MAXRESPONESEND);
}

int RD_Messenger_ProcessCommingProcess_SCENE(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	uart_CSend("co ban tin E2\n");

	uint16_t Header_buff =0x00;
	uint16_t Gw_Add_Buff =0x01;
	uint16_t Add_Receiver = ele_adr_primary;

	Header_buff = ((par[1]<<8) | par[0]);
	Gw_Add_Buff = cb_par->adr_src;
	Add_Receiver = cb_par->adr_dst;
	char UART_Buff[128];
	switch (Header_buff)
	{
		case RD_HEADER_CONTROL_SW_TOUCH:
			RD_Handle_CTR_SW(par, Gw_Add_Buff);
			break;
		case RD_HEADER_SWITCH_TOUCH_STT:
			Send_Relay_Stt_Message_RALL(Gw_Add_Buff);
			break;
		case RD_HEADER_SW_SETUP_LINKCTR:
			RD_Handle_SetUp_LinkControl2(par, Gw_Add_Buff, Add_Receiver);
			break;
//		case RD_HEADER_SW_SETUP_COUNT_DOWN:
//			#if(COUNT_DOWN_EN == 1)
//				RD_Handle_SetUp_CountDown(par, Gw_Add_Buff, Add_Receiver);
//			#else
//				uart_CSend("This function only on Switch x1\n");
//			#endif
//			break;
		case RD_HEADER_SW_PAIR_K9BREMOTE:
			//RD_Handle_PairK9BHc(par, Gw_Add_Buff);
			RD_Handle_ScanK9BHc(par, Gw_Add_Buff);
			break;
		case RD_HEADER_SW_SAVESCENE_K9BREMOTE:
			RD_Handle_K9BSaveScene(par, Gw_Add_Buff);
			break;
		case RD_HEADER_SW_DELETESCENE_K9BREMOTE:
			RD_Handle_K9BDeleteScene(par, Gw_Add_Buff);
			break;
		case RD_HEADER_SW_SAVEK9BHC:
			RD_Handle_SaveK9BHc(par, Gw_Add_Buff);
			break;
		case RD_HEADER_SW_DELETEK9BHC:
			RD_Handle_DeleteK9BHC(par, Gw_Add_Buff);
			break;
		case RD_HEADER_SW_POWUP_CONFIG:
			RD_Handle_PowUpConfig(par, Gw_Add_Buff);
			break;
		case RD_HEADER_SET_K9B_LOCAL_MODE:
			RD_Handle_K9BLocalModeConfig(par, Gw_Add_Buff);
			break;
		case RD_AUTO_CREATE_GR:
			RD_Handle_AutoCreateGr(par, Gw_Add_Buff, cb_par );
			break;
		case RD_AUTO_DELETE_GR:
			RD_Handle_AutoDeleteGr(par, Gw_Add_Buff, cb_par );
			break;
		default:
			sprintf(UART_Buff,"wrong header:  %x \n",Header_buff);
			uart_CSend(UART_Buff);
			break;
	}

	return 0;
}

void RD_Call_Scene(uint16_t Scene_ID, uint8_t Mess_ID)
{
	Call_Scene_Str Scene_Mess_Buff;
	Scene_Mess_Buff.Scene_ID[0] = (uint8_t) ((Scene_ID) & 0xFF );
	Scene_Mess_Buff.Scene_ID[1] = (uint8_t) ((Scene_ID >> 8) & 0xFF );
	Scene_Mess_Buff.Null_Byte   = Mess_ID;
	Scene_Mess_Buff.Tss[0]		= (uint8_t) ((TSS_DEFAULT) & 0xFF);
	Scene_Mess_Buff.Tss[1]		= (uint8_t) ((TSS_DEFAULT>>8) & 0xFF);
	Scene_Mess_Buff.Future[0]   = 0x00;
	Scene_Mess_Buff.Future[1]   = 0x00;
	Scene_Mess_Buff.Future[2]   = 0x00;

	uint8_t* Mess_Buff;
	Mess_Buff = (uint8_t *) (&Scene_Mess_Buff);
	mesh_tx_cmd2normal_primary(SCENE_RECALL_NOACK, Mess_Buff, 8, 0xffff, 0); //SENSOR_STATUS

	//mesh_tx_cmd2normal_primary(SCENE_RECALL_NOACK, (uint8_t *) (&Scene_Mess_Buff.Scene_ID[0]), 8, 0xffff, 0); //SENSOR_STATUS
}
