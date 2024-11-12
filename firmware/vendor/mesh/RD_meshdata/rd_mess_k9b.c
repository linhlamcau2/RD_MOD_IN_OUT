#include "rd_meshdata.h"


void RD_Mess_ScanK9BHC_Rsp(uint16_t Gw_Add_Buff, uint32_t K9Bmac, uint8_t type, s8 TxPow)
{
	uint8_t Mess_RspBuff[8] ={0};
	Mess_RspBuff[0]	= RD_HEADER_SW_PAIR_K9BREMOTE 				& 0xFF;
	Mess_RspBuff[1]	= (RD_HEADER_SW_PAIR_K9BREMOTE >> 8) 		& 0xFF;
	Mess_RspBuff[2] = (uint8_t) (K9Bmac      & 0xFF);
	Mess_RspBuff[3] = (uint8_t) ((K9Bmac >> 8) & 0xFF);
	Mess_RspBuff[4] = (uint8_t) ((K9Bmac >> 16) & 0xFF);
	Mess_RspBuff[5] = (uint8_t) ((K9Bmac >> 24) & 0xFF);
	Mess_RspBuff[6] = type;
	Mess_RspBuff[7] = TxPow;
	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP , &Mess_RspBuff[0], 8, Gw_Add_Buff, RD_MAXRESPONESEND);

}


void RD_MessK9BHc_Press(uint16_t K9B_Add, uint8_t ButtonID, uint8_t ModeID, uint16_t SenceID)
{
	uint8_t Mess_Buff[8] = {0};

	Mess_Buff[0] 	= RD_HEADER_SW_K9B_PRESS & 0xFF ;
	Mess_Buff[1]	= RD_HEADER_SW_K9B_PRESS >>8;
	Mess_Buff[2]  	= K9B_Add & 0xFF;
	Mess_Buff[3]  	= K9B_Add >>8;
	Mess_Buff[4]  	= ButtonID;
	Mess_Buff[5]  	= ModeID;
	Mess_Buff[6]  	= SenceID & 0xFF;
	Mess_Buff[7]  	= SenceID >>8;

	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP, Mess_Buff, 8, RD_GATEWAYADDRESS, RD_MAXRESPONESEND); //SENSOR_STATUS
	uart_CSend("send K9B Press HC \n");
}


uint8_t RD_K9B_ScanOnOff(uint32_t macDevice, uint8_t key, uint32_t par_signature)
{
	uint8_t scanSttReturn = 0;
	static uint32_t clockTime_toggleButton_ms[MAX_NUM_K9ONOFF] ={0};
	static uint32_t signatureLast = 0;
	static K9bOnOff_countLast_st OnOff_CountLast[NUM_OF_ELEMENT];
	memset((void *)OnOff_CountLast,0,sizeof(K9bOnOff_countLast_st)*NUM_OF_ELEMENT);
	if((0x00 == par_signature) || (par_signature != signatureLast))
	{
		uart_CSend("Scan K9b onOff  V1:\n");
		for(int i=0; i< NUM_OF_ELEMENT; i++)
		{
			//uint8_t numBtSaved = Sw_Flash_Data_Val.K9B_Data.OnOff[i].NumSaveNext;
			uint8_t numBtSaved = MAX_NUM_K9ONOFF;
			if(clock_time_ms() < clockTime_toggleButton_ms[i]) clockTime_toggleButton_ms[i] = clock_time_ms();
			for(int j=0; j < numBtSaved; j++)
			{
				if( (Sw_Flash_Data_Val.K9B_Data.OnOff[i].K9B_BtKey[j] /*- 0x80*/ & key) && (Sw_Flash_Data_Val.K9B_Data.OnOff[i].MacK9B[j] == macDevice) )
				{
					uart_CSend("key pass \n");
					if(clock_time_ms() - clockTime_toggleButton_ms[i] >= CLOCK_TIME_OUT_PRESS_K9B_MS)
					{
						if(OnOff_CountLast[i].countLastOfMacID[j] != par_signature){
							OnOff_CountLast[i].countLastOfMacID[j] = par_signature;
							clockTime_toggleButton_ms[i] = clock_time_ms();
							signatureLast = par_signature;
						}
					}
					break; // out off this element of switch
				}
			}
		}
	}
	return scanSttReturn;
}


void RD_Handle_ScanK9BHc(uint8_t par[8], uint16_t Gw_Add_Buff)
{
	uart_CSend("Start wating K9B HC \n");
	uint8_t  K9BPairHc_Stt = par[2];
//	RD_SwitchAc4Ch_BlinkSet(5, 100);
	RD_K9B_PairHCSet(K9BPairHc_Stt, 0x0000); // turn on K9B pair Hc
}

void RD_Handle_K9BSaveScene(uint8_t par[8], uint16_t Gw_Add_Buff)
{
	uint16_t K9BAdd = (uint16_t) ((par[3] << 8) | par[2] );
	uint8_t ButtonID = par[4];
//	uint8_t ModeID = par[5];
	uint16_t Scene_ID	= (uint16_t) ((par[7] << 8) | par[6] );

	char UART_TempSend[128];
	sprintf(UART_TempSend,"save Scene add:%x  BID:%d Scene ID: 0x%x\n",K9BAdd,  ButtonID, Scene_ID);
	uart_CSend(UART_TempSend);

	uint8_t SaveStt = RD_Flash_SaveSceneK9BHC(K9BAdd, ButtonID, Scene_ID);

	uint8_t Mess_RspBuff[3] ={0};
	Mess_RspBuff[0]	= RD_HEADER_SW_SAVESCENE_K9BREMOTE 				& 0xFF;
	Mess_RspBuff[1]	= (RD_HEADER_SW_SAVESCENE_K9BREMOTE >> 8) 		& 0xFF;
	Mess_RspBuff[2]	= SaveStt;

	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP , &Mess_RspBuff[0], 3, Gw_Add_Buff, RD_MAXRESPONESEND);

//	RD_SwitchAc4Ch_BlinkSet(5, 100);
}

void RD_Handle_K9BDeleteScene(uint8_t par[8], uint16_t Gw_Add_Buff)
{
	uint16_t K9BAdd 	= (uint16_t) ((par[3] << 8) | par[2] );
	uint8_t ButtonID 	= par[4];
//	uint8_t ModeID 		= par[5];
	uint8_t deleteStt=0;
	if( 0xFFFF == K9BAdd ) 		deleteStt = RD_Flash_DeleteSceneK9BHC(K9BAdd); 			// delete all Scene of all remote
	else
	{
		if( 0xff == ButtonID)   deleteStt = RD_Flash_DeleteSceneK9BHC(K9BAdd); 							// delete all scene of 1 remote K9BAdd
		else					deleteStt = RD_Flash_SaveSceneK9BHC(K9BAdd, ButtonID, 0x0000); // delete 1 scene
	}



	uint8_t Mess_RspBuff[3] ={0};
	Mess_RspBuff[0]	= RD_HEADER_SW_DELETESCENE_K9BREMOTE 				& 0xFF;
	Mess_RspBuff[1]	= (RD_HEADER_SW_DELETESCENE_K9BREMOTE >> 8) 		& 0xFF;
	Mess_RspBuff[2]	= deleteStt;

	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP , &Mess_RspBuff[0], 3, Gw_Add_Buff, RD_MAXRESPONESEND);

//	RD_SwitchAc4Ch_BlinkSet(5, 100);
}


void RD_Handle_SaveK9BHc(uint8_t par[8], uint16_t Gw_Add_Buff)
{
	uint32_t MacK9B_Buff = (uint32_t) ( (par[5]<<24) | (par[4]<<16) | (par[3]<<8) | (par[2]));
	uint16_t K9BAdd	 	 = (uint16_t) ((par[7] << 8) | par[6]  );
	uint8_t SaveK9B_Stt = RD_Flash_SaveK9BHc(MacK9B_Buff, 3, K9BAdd);// save max button

	uint8_t MessRsp[7];
	for(int i=0; i<6; i++) MessRsp[i] = par[i];

	MessRsp[6] = SaveK9B_Stt;

	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP , &MessRsp[0], 7, Gw_Add_Buff, RD_MAXRESPONESEND);
	uart_CSend("Save Mac K9BHC \n");
//	RD_SwitchAc4Ch_BlinkSet(5, 100);
}


void RD_Handle_DeleteK9BHC(uint8_t par[8], uint16_t Gw_Add_Buff)
{
	uint16_t K9BAdd	 	 = (uint16_t) ((par[3] << 8) | par[2]  );
	uint8_t SaveK9B_Stt = RD_Flash_DeleteK9BHC(K9BAdd);

	uint8_t Mess_RspBuff[5] ={0};
	Mess_RspBuff[0]	= RD_HEADER_SW_DELETEK9BHC 				& 0xFF;
	Mess_RspBuff[1]	= (RD_HEADER_SW_DELETEK9BHC >> 8) 		& 0xFF;
	Mess_RspBuff[2]	= K9BAdd 				& 0xFF;
	Mess_RspBuff[3]	= (K9BAdd >> 8) 		& 0xFF;
	Mess_RspBuff[4] =  SaveK9B_Stt;

	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP , &Mess_RspBuff[0], 5, Gw_Add_Buff, RD_MAXRESPONESEND);
	uart_CSend("Delete K9BHC \n");
//	RD_SwitchAc4Ch_BlinkSet(5, 100);
}


void RD_Handle_K9BLocalModeConfig(u8 *par, uint16_t Gw_Add_Buff)
{
	uint8_t  K9B_LocalMode_Config = par[2];
	RD_Flash_K9BSaveLocalMode(K9B_LocalMode_Config);

	uint8_t Mess_RspBuff[3] ={0};
	Mess_RspBuff[0]	= RD_HEADER_SET_K9B_LOCAL_MODE 				& 0xFF;
	Mess_RspBuff[1]	= (RD_HEADER_SET_K9B_LOCAL_MODE >> 8) 		& 0xFF;
	Mess_RspBuff[2]	= K9B_LocalMode_Config;

	mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP , &Mess_RspBuff[0], 3, Gw_Add_Buff, RD_MAXRESPONESEND);

//	RD_SwitchAc4Ch_BlinkSet(10, 200);
}

