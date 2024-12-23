#include "rd_meshdata.h"

#include "../RD_secure/rd_secure.h"
#include "../RD_log/RD_log.h"

extern void mesh_g_onoff_st_rsp_par_fill(mesh_cmd_g_onoff_st_t *rsp, u8 idx);
uint16_t RD_GATEWAYADDRESS  = GW_ADD_DEFAULT;


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

	rd_call_tx(RD_OPCODE_TYPE_RSP, Mess_buff, 8, Gw_Add_Buff);
//	mesh_tx_cmd2normal_primary(RD_OPCODE_TYPE_RSP, Mess_buff, 8, Gw_Add_Buff, RD_MAXRESPONESEND);
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
	RD_GATEWAYADDRESS = GwAddToSave ? GwAddToSave : Gw_Add_Buff;
	RD_ev_log("Save GWAdd:%x \n", RD_GATEWAYADDRESS);
	RD_Flash_SaveGwAdd(RD_GATEWAYADDRESS);
	uint8_t Mess_buff[8] = {0};

	Mess_buff[0]	= RD_HEADER_SAVE_GW_ADD 				& 0xff;
	Mess_buff[1]	= (RD_HEADER_SAVE_GW_ADD >> 8) 			& 0xff;
	Mess_buff[2]	= RD_GATEWAYADDRESS		 				& 0xff;
	Mess_buff[3]	= (RD_GATEWAYADDRESS >> 8) 				& 0xff;
	Mess_buff[4]	= PROVIDER_ID  							& 0xff;
	Mess_buff[5]	= (PROVIDER_ID >> 8) 					& 0xff;

	rd_call_tx(RD_OPCODE_TYPE_RSP, Mess_buff, 8, Gw_Add_Buff);
//	mesh_tx_cmd2normal_primary(RD_OPCODE_TYPE_RSP, Mess_buff, 8, Gw_Add_Buff, RD_MAXRESPONESEND); // rsp to Gw send mess
}


static void RD_Hanlde_FactoryTestEnd(uint16_t Gw_Add_Buff)
{
	uint8_t Mess_buff[8] = {0};

	Mess_buff[0]	= RD_HEADER_FACTORY_TEST_END & 0xff;
	Mess_buff[1]	= (RD_HEADER_FACTORY_TEST_END >> 8) & 0xff;


	rd_call_tx(RD_OPCODE_TYPE_RSP, Mess_buff, 8, Gw_Add_Buff);
//	mesh_tx_cmd2normal_primary(RD_OPCODE_TYPE_RSP, Mess_buff, 8, Gw_Add_Buff, RD_MAXRESPONESEND);
	RD_Flash_SaveGwAdd(0x0001);
}

static void RD_Handle_KickAll(uint8_t par[8], uint16_t Gw_Add_Buff)
{
	if((par[2]==0x01) && (par[3]==0x02) && (par[4]==0x03) && (par[5]==0x04) && (par[6]==0x05) && (par[7]==0x06))
	{
		uart_CSend("Reset All device. countdown 60s to reset \n");
		Kick_all_Flag=1;

		rd_call_tx(RD_OPCODE_TYPE_RSP, par, 8, Gw_Add_Buff);
//		mesh_tx_cmd2normal_primary(RD_OPCODE_TYPE_RSP, par, 8, Gw_Add_Buff, RD_MAXRESPONESEND);
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

int rd_send_relay_stt(uint8_t Relay_ID, uint8_t Relay_Stt)
{
	uint8_t Mess_Buff[8] = {0};
	uint16_t Element_Add = 0x0000;

	Mess_Buff[1]		= Relay_Stt;
	Element_Add 		= ele_adr_primary + Relay_ID;

	rd_call_tx2(G_ONOFF_STATUS, Mess_Buff, 8, RD_GATEWAYADDRESS, Element_Add);
	return 0;
//	mesh_tx_cmd2normal(G_ONOFF_STATUS, Mess_Buff, 1, Element_Add, RD_GATEWAYADDRESS, 2);
}
extern u8 rd_read_input(u8 id);
static void rd_handle_setting_input(u8 *par,int par_len, u16 gw_addr)
{
	u8 idx = par[2];
	u8 mode = par[3];
	RD_ev_log("setting_input id: %d mode : %d par_len: %d\n",idx,mode,par_len);
	u8 err = rd_save_mode_input(idx,mode);
	if(err)
	{
		rd_blink_led(idx-1,LED_NUM_BLINK_SETTING_INPUT,TIME_500MS);
		reset_detect_input(idx-1);
		rd_call_tx(RD_OPCODE_SCENE_RSP,par,8,gw_addr);
//		mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP, par, 8, gw_addr, RD_MAXRESPONESEND);
	}
}

static void rd_handle_setting_link(u8 *par,int par_len, u16 gw_addr)
{
	u8 idx_in = par[2];
	u8 idx_out = par[3];
	RD_ev_log("setting_link in: %d out : %d\n",idx_in,idx_out);
	u8 err = rd_save_linked_io(idx_in,idx_out);
	if(err)
	{
		rd_blink_led(LED_OUT + idx_out-1,LED_NUM_BLINK_SET_LINKED,TIME_500MS);
		rd_call_tx(RD_OPCODE_SCENE_RSP,par,8,gw_addr);
//		mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP, par, 8, gw_addr, RD_MAXRESPONESEND);
	}
}

void rd_handle_powerup_cf(u8 *par,int par_len, u16 gw_addr)
{
	u8 idx_out = par[2];
	u8 pow_cf = par[3];
	u8 err = rd_save_powerup_cf(idx_out,pow_cf);
	RD_ev_log("powerup_cf: idx: %d stt: pow: %d\n",idx_out,pow_cf);
	if(err)
	{
		rd_call_tx(RD_OPCODE_SCENE_RSP, par,8,gw_addr);
		rd_blink_led(LED_OUT + idx_out-1,LED_NUM_BLINK_POW_UP_OUTPUT,TIME_500MS);
//		mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP, par, 8, gw_addr, RD_MAXRESPONESEND);
	}
}

void rd_handle_setting_sence_input(u8 *par,int par_len, u16 gw_addr)
{
	u8 type_input = par[2];
	u16 id_sence = (par[4] << 8) | (par[3]);

	RD_ev_log("setting_sence type_input: %d sc : %d idx: %d stt_sence: %d\n",type_input,id_sence,par[5],par[6]);
	u8 err = 0;
	if(type_input == TYPE_IN)
	{
		u8 idx_in = par[5];
		u8 stt_sence = par[6] ;
		err = rd_save_sence_in(idx_in,stt_sence,id_sence);
		if(err)
		{
			rd_blink_led(idx_in-1,LED_NUM_SET_SENCE,TIME_500MS);
		}
	}
	else if(type_input == TYPE_ADC_GREATER || type_input == TYPE_ADC_LOWER)
	{
		u16 adc_threshold = (par[5] << 8) | (par[6]);
		err = rd_save_sence_adc(adc_threshold,id_sence,type_input);
//		mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP, par, 8, gw_addr, RD_MAXRESPONESEND);
	}
	if(err)
	{
		rd_call_tx(RD_OPCODE_SCENE_RSP,par,8,gw_addr);
	}
}

void rd_rsp_state_output(u16 gw_addr)
{
	u8 rsp[8] = {0};
	rsp[0] = RD_HEADER_OUTPUT_STATUS & 0xff;
	rsp[1] = RD_HEADER_OUTPUT_STATUS >> 8;
	rsp[2] = NUM_OF_ELEMENT;
	rsp[3] = RD_get_on_off(0,0);
	rsp[4] = RD_get_on_off(1,0);
	rd_call_tx(RD_OPCODE_SCENE_RSP, rsp, 8, gw_addr);
}
void rd_handle_rsp_state_output(u8 *par,int par_len,u16 gw_addr)
{
	u8 rsp[8] = {0};
	rsp[0] = par[0];
	rsp[1] = par[1];
	rsp[2] = NUM_OF_ELEMENT;
	rsp[3] = RD_get_on_off(0,0);
	rsp[4] = RD_get_on_off(1,0);
//	rsp[5] = get_status_input(3);
//	RD_ev_log("state_all 1:%d 2:%d 3:%d 4:%d adc:%d\n",rsp[2],rsp[3],rsp[4],rsp[5],adc_val);
	rd_call_tx(RD_OPCODE_SCENE_RSP, rsp, 8, gw_addr);
}

void rd_handle_rsp_state_all_input(u8 *par,int par_len,u16 gw_addr)
{
	u16 adc_val = get_adc_value();
	u8 rsp[8] = {0};
	rsp[0] = par[0];
	rsp[1] = par[1];
	rsp[2] = get_status_input(0);
	rsp[3] = get_status_input(1);
	rsp[4] = get_status_input(2);
	rsp[5] = get_status_input(3);
	rsp[7] = adc_val & 0xff;
	rsp[6] = adc_val >> 8;
//	RD_ev_log("state_all 1:%d 2:%d 3:%d 4:%d adc:%d\n",rsp[2],rsp[3],rsp[4],rsp[5],adc_val);
	rd_call_tx(RD_OPCODE_SCENE_RSP, rsp, 8, gw_addr);
//	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP, rsp, 8, gw_addr, RD_MAXRESPONESEND);
}

void rd_handle_rsp_set_delta_adc(u8 *par,int par_len,u16 gw_addr)
{
	u8 delta = par[2];
	int err = rd_save_delta_adc(delta);
	if(err && par_len == 8)
	{
		rd_call_tx(RD_OPCODE_SCENE_RSP, par, 8, gw_addr);
	}
}

void rd_handle_rsp_set_stt_all_relay(u8 *par,int par_len,u16 gw_addr)
{
	u8 idx = par[2];
	u8 stt = par[3];
	if(idx == 0xff)
	{
		for(u8 i = 0; i<NUM_OF_ELEMENT; i++)
		{
			rd_onoff_relay(stt,i,1,1);
		}
	}
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
		case RD_HEADER_SETTING_INPUT:
			rd_handle_setting_input(par,par_len,Gw_Add_Buff);
			break;
		case RD_HEADER_LINKER_INPUT:
			rd_handle_setting_link(par,par_len, Gw_Add_Buff);
			break;
		case RD_HEADER_RELAY_STARTUP:
			rd_handle_powerup_cf(par, par_len,Gw_Add_Buff);
			break;
		case RD_HEADER_SETTING_SENCE_INPUT:
			rd_handle_setting_sence_input(par, par_len,Gw_Add_Buff);
			break;
		case RD_HEADER_STATUS_ALL_INPUT:
			rd_handle_rsp_state_all_input(par, par_len,Gw_Add_Buff);
			break;
		case RD_HEADER_SET_DELTA_ADC:
			rd_handle_rsp_set_delta_adc(par, par_len,Gw_Add_Buff);
			break;
		case RD_HEADER_OUTPUT_STATUS:
			rd_handle_rsp_state_output(par, par_len,Gw_Add_Buff);
			break;
		case RD_HEADER_SET_STT_ALL_RELAY:
			rd_handle_rsp_set_stt_all_relay(par, par_len,Gw_Add_Buff);
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

	rd_call_tx(SCENE_RECALL_NOACK,Mess_Buff,8,0xffff);

//	mesh_tx_cmd2normal_primary(SCENE_RECALL_NOACK, Mess_Buff, 8, 0xffff, 0); //SENSOR_STATUS

	//mesh_tx_cmd2normal_primary(SCENE_RECALL_NOACK, (uint8_t *) (&Scene_Mess_Buff.Scene_ID[0]), 8, 0xffff, 0); //SENSOR_STATUS
}

void rd_input_call_sence(uint16_t sence_id)
{
	static u16 mess_id = 0;
	RD_Call_Scene(sence_id, mess_id ++);
}


void rd_update_input_stt(u8 idx_in, u8 status_in, u16 sence)
{
	uint8_t buff[8] = {0};

	buff[0] 	= RD_HEADER_INPUT_STATUS & 0xFF ;
	buff[1]		= RD_HEADER_INPUT_STATUS >>8;
	buff[2]		= idx_in +1;
	buff[3]		= status_in;
	buff[4] 	= sence & 0xFF;
	buff[5] 	= sence >>8;

//	RD_ev_log("buff send:%d %d %d %d %d %d\n",buff[0],buff[1],buff[2],buff[3],buff[4],buff[5]);
	rd_call_tx(RD_OPCODE_INPUT_RSP,buff,8,RD_GATEWAYADDRESS);
//	mesh_tx_cmd2normal_primary(RD_OPCODE_INPUT_RSP, Mess_Buff, 8, RD_GATEWAYADDRESS, RD_MAXRESPONESEND); //SENSOR_STATUS
	uart_CSend("update stt \n");
}

void rd_update_adc_stt(u16 adc, u16 id_sence)
{
	uint8_t buff[8] = {0};

	buff[0] 	= RD_HEADER_ADC_STATUS & 0xFF ;
	buff[1]		= RD_HEADER_ADC_STATUS >>8;
	buff[3]		= adc & 0xFF;
	buff[2]		= adc >> 8;
	buff[4] 	= id_sence & 0xFF;
	buff[5] 	= id_sence >>8;

	rd_call_tx(RD_OPCODE_INPUT_RSP,buff,8,RD_GATEWAYADDRESS);
}

u8 training_fac = 0;
extern u32 tick_time_start_rev_mess_trainning;
int RD_Messenger_ProcessCommingProcess_TRAIN(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	uint16_t	Header_Buff = (par[1] << 8) | (par[0]);

	if((training_fac == 0) && (Header_Buff == 0x0101))
	{
		uart_CSend("start Train\n");
		training_fac =1;
		Train_Factory = CATCH_UP_MESS_TRAIN_FAC;
		tick_time_start_rev_mess_trainning = clock_time_s();
		for(u8 i=0; i<NUM_OF_RELAY; i++)
		{
			rd_init_onoff_relay(1,i);
		}
	}
	return 0;
}
