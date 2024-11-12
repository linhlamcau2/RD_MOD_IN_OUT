#include"rd_flash.h"

void RD_Flash_SaveK9BOnOff(uint32_t macDevice, uint8_t key, uint8_t SwButtonID)
{
	uint8_t ID_ArraySave = Sw_Flash_Data_Val.K9B_Data.OnOff[SwButtonID-1].NumSaveNext;
	Sw_Flash_Data_Val.K9B_Data.OnOff[SwButtonID-1].MacK9B[ID_ArraySave] = macDevice;
	Sw_Flash_Data_Val.K9B_Data.OnOff[SwButtonID-1].K9B_BtKey[ID_ArraySave] = key;

	Sw_Flash_Data_Val.K9B_Data.OnOff[SwButtonID-1].NumSaveNext++;  // next ID array to save
	if(Sw_Flash_Data_Val.K9B_Data.OnOff[SwButtonID-1].NumSaveNext >= MAX_NUM_K9ONOFF) // resave over arrayy 0ad
	{
		uart_CSend("save max button K9B\n");
		Sw_Flash_Data_Val.K9B_Data.OnOff[SwButtonID-1].NumSaveNext = 0;
	}

	flash_erase_sector(FLASH_ADDR);
	flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );
#if UART_ON
	char UART_TempSend[128];
	sprintf(UART_TempSend,"Save K9B OnOff: Button:%d Mac:%x  Key:%x \n",SwButtonID,  macDevice, key);
	uart_CSend(UART_TempSend);
#endif
}

uint8_t RD_Flash_SaveSceneK9BHC(uint16_t K9BAdd, uint8_t ButtonID, uint16_t SceneID)
{
	for(int i=0; i<MAX_NUM_K9BHC; i++)
	{
		if(Sw_Flash_Data_Val.K9B_Data.Scene[i].AddK9B == K9BAdd)		// task memory is ready for new
		{
			int ButtonPos_Buff = RD_CheckButtonPosK9BHC(&Sw_Flash_Data_Val.K9B_Data.Scene[i].Button_ID[0], ButtonID);

			if(-1 == ButtonPos_Buff){ // new ButtonID //RD_Note
				int ButtonPos_New = RD_CheckButtonPosK9BHC(&Sw_Flash_Data_Val.K9B_Data.Scene[i].Button_ID[0], 0x00);
				Sw_Flash_Data_Val.K9B_Data.Scene[i].Scene_ID_OnePress[ButtonPos_New] = SceneID;
				Sw_Flash_Data_Val.K9B_Data.Scene[i].Button_ID[ButtonPos_New] = ButtonID;
			}
			else{ // duplicate Button ID, over save
				if(0x0000 == SceneID) ButtonID =0; // delete scene
				Sw_Flash_Data_Val.K9B_Data.Scene[i].Scene_ID_OnePress[ButtonPos_Buff] = SceneID;
				Sw_Flash_Data_Val.K9B_Data.Scene[i].Button_ID[ButtonPos_Buff] = ButtonID;
			}

			flash_erase_sector(FLASH_ADDR);
			flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );
			char UART_TempSend[128];
			sprintf(UART_TempSend,"Save K9B HC scene:%x  BID:%d, ButtonPos_Buff: %d\n",SceneID,  ButtonID, ButtonPos_Buff);
			uart_CSend(UART_TempSend);
			return 1;
		}
	}
	return 0; // don;t find k9b remote
}

static void RD_Flash_SetNullSceneK9B(uint8_t numOfArray)
{
	if( 0xFF == numOfArray)
	{
		for(int i=0; i< MAX_NUM_K9BHC; i++)
		{
			for(int j=0; j< MAX_MUM_K9BPRESSTYPE; j++)
			{
				Sw_Flash_Data_Val.K9B_Data.Scene[i].Scene_ID_OnePress[j] = 0;
				Sw_Flash_Data_Val.K9B_Data.Scene[i].Button_ID[j] = 0;
			}
		}
	}
	else
	{
		for(int j=0; j < MAX_MUM_K9BPRESSTYPE; j++)
		{
			Sw_Flash_Data_Val.K9B_Data.Scene[numOfArray].Scene_ID_OnePress[j] = 0;
			Sw_Flash_Data_Val.K9B_Data.Scene[numOfArray].Button_ID[j] = 0;
		}
	}
}

uint8_t RD_Flash_DeleteSceneK9BHC(uint16_t K9BAdd)
{
	if(0xFFFF != K9BAdd)
	{
		for(int i=0; i<MAX_NUM_K9BHC; i++)
		{
			if(Sw_Flash_Data_Val.K9B_Data.Scene[i].AddK9B == K9BAdd)
			{
				RD_Flash_SetNullSceneK9B(i);
				flash_erase_sector(FLASH_ADDR);
				flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );
				char UART_TempSend[128];
				sprintf(UART_TempSend,"delete all scene K9B add: 0x%x\n",K9BAdd);
				uart_CSend(UART_TempSend);
				return 1;
			}
		}
		uart_CSend("don't have  K9B HC to delete Scene");
		return 0; // no data to delete
	}
	else
	{
		RD_Flash_SetNullSceneK9B(0xFF);
		flash_erase_sector(FLASH_ADDR);
		flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );
		uart_CSend("delete all scene of all K9B \n");
		return 1;
	}
}


uint8_t RD_Flash_SaveK9BHc(uint32_t macDevice, uint8_t Num_Of_Button, uint16_t K9BAdd)
{
	uint8_t overSave_Flag =0; // resave when duplicate mac
	uint8_t  overSave_ID =0;
	// check duplicate remote saved
	for(int i=0; i<MAX_NUM_K9BHC; i++)
	{
		if(Sw_Flash_Data_Val.K9B_Data.Scene[i].MacK9B == macDevice)
		{
			overSave_Flag =1;
			overSave_ID = i;
			break;
		}
	}

	if(1 == overSave_Flag)
	{
		Sw_Flash_Data_Val.K9B_Data.Scene[overSave_ID].MacK9B = macDevice;
		Sw_Flash_Data_Val.K9B_Data.Scene[overSave_ID].Num_Of_Button = Num_Of_Button;
		Sw_Flash_Data_Val.K9B_Data.Scene[overSave_ID].AddK9B = K9BAdd;
		flash_erase_sector(FLASH_ADDR);
		flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );
		char UART_TempSend[128];
		sprintf(UART_TempSend,"over Save K9B HC Mac:%x  NumOfButton:%d add: 0x%x\n",macDevice,  Num_Of_Button, K9BAdd);
		uart_CSend(UART_TempSend);
		return 1;
	}
	// check null task memory
	for(int i=0; i<MAX_NUM_K9BHC; i++)
	{
		if(Sw_Flash_Data_Val.K9B_Data.Scene[i].MacK9B == 0x00)		// task memory is ready for new
		{
			Sw_Flash_Data_Val.K9B_Data.Scene[i].MacK9B = macDevice;
			Sw_Flash_Data_Val.K9B_Data.Scene[i].Num_Of_Button = Num_Of_Button;
			Sw_Flash_Data_Val.K9B_Data.Scene[i].AddK9B = K9BAdd;
			flash_erase_sector(FLASH_ADDR);
			flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );
			char UART_TempSend[128];
			sprintf(UART_TempSend,"Save K9B HC Mac:%x  NumOfButton:%d add: 0x%x\n",macDevice,  Num_Of_Button, K9BAdd);
			uart_CSend(UART_TempSend);
			return 1;
		}
//		if(i == (MAX_NUM_K9BHC-1) ) return 0; // fail full mem
	}
	return 0;
}


void RD_Flash_SetNullK9B(uint8_t numOfArray)
{
	if( 0xFF == numOfArray)
	{
		for(int i=0; i< MAX_NUM_K9BHC; i++)
		{
			Sw_Flash_Data_Val.K9B_Data.Scene[i].MacK9B = 0x00;
			Sw_Flash_Data_Val.K9B_Data.Scene[i].Num_Of_Button = 0x00;
			Sw_Flash_Data_Val.K9B_Data.Scene[i].AddK9B = 0x00;
			for(int j=0; j< MAX_MUM_K9BPRESSTYPE; j++)
			{
				Sw_Flash_Data_Val.K9B_Data.Scene[i].Scene_ID_OnePress[j] = 0;
				Sw_Flash_Data_Val.K9B_Data.Scene[i].Button_ID[j] = 0;
			}
		}
	}
	else
	{
		Sw_Flash_Data_Val.K9B_Data.Scene[numOfArray].MacK9B = 0x00;
		Sw_Flash_Data_Val.K9B_Data.Scene[numOfArray].Num_Of_Button = 0x00;
		Sw_Flash_Data_Val.K9B_Data.Scene[numOfArray].AddK9B = 0x00;
		for(int j=0; j< MAX_MUM_K9BPRESSTYPE; j++)
		{
			Sw_Flash_Data_Val.K9B_Data.Scene[numOfArray].Scene_ID_OnePress[j] = 0;
			Sw_Flash_Data_Val.K9B_Data.Scene[numOfArray].Button_ID[j] = 0;
		}
	}


}

uint8_t RD_Flash_DeleteK9BHC(uint16_t K9BAdd)
{
	if(0xFFFF != K9BAdd)
	{
		for(int i=0; i<MAX_NUM_K9BHC; i++)
		{
			if(Sw_Flash_Data_Val.K9B_Data.Scene[i].AddK9B == K9BAdd)
			{
				RD_Flash_SetNullK9B(i);
				flash_erase_sector(FLASH_ADDR);
				flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );
				char UART_TempSend[128];
				sprintf(UART_TempSend,"delete K9B HC add: 0x%x\n",K9BAdd);
				uart_CSend(UART_TempSend);
				return 1;
			}
		}
		uart_CSend("don't have  K9B HC to delete");
		return 0; // no data to delete
	}
	else
	{
		RD_Flash_SetNullK9B(0xFF);
		flash_erase_sector(FLASH_ADDR);
		flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );
		uart_CSend("delete all remote K9B HC");
		return 1;
	}
}

void RD_Flash_K9BSaveLocalMode(uint8_t local_mode)
{
	if(K9B_CTR_LOCAL_SCENE != local_mode){
		local_mode = K9B_TYPE_CTR_LOCAL_DF;
		uart_CSend("save K9B_TYPE_CTR_LOCAL_DF \n");
	}
	else{
		local_mode = K9B_CTR_LOCAL_SCENE;
		uart_CSend("save K9B_CTR_LOCAL_SCENE \n");
	}

	Sw_Flash_Data_Val.Type_CtrLocal_K9B = local_mode;

	flash_erase_sector(FLASH_ADDR);
	flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );
}


void RD_Flash_DeleteAllK9BOnOff(uint8_t SwButtonID)
{
	Sw_Flash_Data_Val.K9B_Data.OnOff[SwButtonID-1].NumSaveNext =0;  // next ID array to save

	for(int i=0; i < MAX_NUM_K9ONOFF; i++)
	{
		Sw_Flash_Data_Val.K9B_Data.OnOff[SwButtonID-1].MacK9B[i] = 0x00;
		Sw_Flash_Data_Val.K9B_Data.OnOff[SwButtonID-1].K9B_BtKey[i] = 0x00;
	}
	flash_erase_sector(FLASH_ADDR);
	flash_write_page(FLASH_ADDR, sizeof(Sw_Flash_Data_Val), (uint8_t *) &Sw_Flash_Data_Val.Factory_Check );
#if UART_ON
	char UART_TempSend[64];
	sprintf(UART_TempSend,"delete All K9B on button:%d \n",SwButtonID);
	uart_CSend(UART_TempSend);
#endif
}
