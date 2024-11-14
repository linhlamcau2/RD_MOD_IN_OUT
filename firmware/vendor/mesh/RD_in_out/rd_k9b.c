//#include "rd_in_out.h"
//
//Sw_Woring_K9B_Str Sw_Woring_K9B_Val = { 0 };
//
//void RD_K9B_Pair_OnOffClearFlag(void){
//	Sw_Woring_K9B_Val.Button_K9BOnOff_Pair = 0x00;
//	Sw_Woring_K9B_Val.Pair_K9BOnOff_Flag   = 0x00;
//}
//
//void RD_K9B_Pair_OnOffSetFlag(uint8_t Button_ID){
//	Sw_Woring_K9B_Val.Button_K9BOnOff_Pair = Button_ID;
//	Sw_Woring_K9B_Val.Pair_K9BOnOff_Flag   = 1;
//}
//
//uint8_t RD_K9B_Pair_OnOffGetFlag(void)
//{
//	return Sw_Woring_K9B_Val.Button_K9BOnOff_Pair;
//}
//
//static int RD_K9B_SaveOnOff_yet(uint32_t macDevice, uint8_t key, uint8_t switch_button)
//{
//
//	for(int i=0; i< MAX_NUM_K9ONOFF; i++)
//	{
//		uint32_t mac_buf = Sw_Flash_Data_Val.K9B_Data.OnOff[switch_button - 1].MacK9B[i];
//		uint8_t  key_buf = Sw_Flash_Data_Val.K9B_Data.OnOff[switch_button - 1].K9B_BtKey[i];
//		if( (mac_buf == macDevice) && (key_buf == key) )
//		{
//			uart_CSend("This key K9B saved \n");
//			return 1;
//		}
//	}
//	return 0; // this key don't save yet
//}
//
//void RD_K9B_SaveOnOff(uint32_t macDevice, uint8_t key)
//{
//	uint8_t keyFillterStt =0;
//
//	if( ((1 == key) || (2 == key) || (4 == key) || (8 == key)|| (16 == key) || (32 == key)|| (40 == key) ) )  // only single button
//	{
//		keyFillterStt = 1;
//	}
//	if(0x00 != RD_K9B_Pair_OnOffGetFlag() && (1 == keyFillterStt))
//	{
//		if(RD_K9B_SaveOnOff_yet(macDevice, key, RD_K9B_Pair_OnOffGetFlag()) == 0) // this key don't save yet
//		{
//			RD_Flash_SaveK9BOnOff(macDevice, key, RD_K9B_Pair_OnOffGetFlag());
//		}
//		RD_K9B_Pair_OnOffClearFlag();
//	}
//}
//
//void RD_K9B_PairHCSet(uint8_t pairStt, uint16_t K9BAdd)
//{
//	Sw_Woring_K9B_Val.Add_K9B_HCSet = K9BAdd;
//	Sw_Woring_K9B_Val.Pair_K9BHc_Flag = pairStt;
//	Sw_Woring_K9B_Val.ClockTimeSetHc_ms = clock_time_ms();
//}
//
//void RD_K9B_CheckScanK9BHc(uint32_t K9BMac_Buff, uint8_t Num_Of_Button, s8 rssi)
//{
//	if(0x01 == Sw_Woring_K9B_Val.Pair_K9BHc_Flag)
//	{
//	//	uint8_t saveStt = RD_Flash_SaveK9BHc(K9BMac_Buff, Num_Of_Button, Sw_Woring_K9B_Val.Add_K9B_HCSet);
//		char UART_TempSend1[128] = {0};
//		sprintf(UART_TempSend1,"send HC mac: %08x, Type: %d, rssi: %d--%x--%x--%x Rssi: %d",K9BMac_Buff, Num_Of_Button, rssi);
//		uart_CSend(UART_TempSend1);
//
//		RD_Mess_ScanK9BHC_Rsp(RD_GATEWAYADDRESS, K9BMac_Buff, Num_Of_Button, rssi );
//		RD_K9B_PairHCSet(0x00, 0x0000); //clear phase working
////		RD_SwitchAc4Ch_BlinkSet(3, 500);
//	}
//}
//
//// check pos button ID in button aray of remote K9B HC
//int RD_CheckButtonPosK9BHC(uint8_t ButtonID_Aray[MAX_MUM_K9BPRESSTYPE], uint8_t ButtonID_Check){
//	for(int i=0; i<MAX_MUM_K9BPRESSTYPE; i++){
//		if(ButtonID_Aray[i] == ButtonID_Check) {
//			char UART_TempSend[128];
//			sprintf(UART_TempSend,"Button Pos %d\n",i);
//			uart_CSend(UART_TempSend);
//			return i;
//		}
//	}
//	uart_CSend("don't find BID \n");
//	return -1;
//}
//
//
//uint8_t RD_K9B_ScanPress2HC(uint32_t macDevice, uint8_t key, uint32_t par_signature)
//{
//	uint8_t scanSttReturn = 0;
//	static uint32_t clockTime_toggleButton_ms[MAX_NUM_K9BHC] ={0};
//	static uint32_t signatureLast = 0;
//
//	static uint8_t K9BButton_ID=0;
//	uint8_t press_access =0;
//
//	static K9bHc_countLast_st Hc_CountLast;
//	for(int i= 0; i< MAX_NUM_K9BHC; i++)
//	{
//		Hc_CountLast.countLastOfMacID[i] = 0;
//	}
//	if( (key>0) && (key <0x80)) // only rising press.
//	{
//		K9BButton_ID = key;
//		press_access = 1;
//	}
//	if( ((0x00 == par_signature) || (par_signature != signatureLast)) && (1 == press_access) )
//	{
//		for(int i=0; i< MAX_NUM_K9BHC; i++)
//		{
//			if(clock_time_ms() < clockTime_toggleButton_ms[i])
//				clockTime_toggleButton_ms[i] = clock_time_ms();
//
//			if(macDevice == Sw_Flash_Data_Val.K9B_Data.Scene[i].MacK9B)
//			{
//				scanSttReturn = 1;
//				if(clock_time_ms() - clockTime_toggleButton_ms[i] >= CLOCK_TIME_OUT_PRESS_K9B_MS) // check time last call
//				{
//					if(Hc_CountLast.countLastOfMacID[i]  != par_signature){
//						Hc_CountLast.countLastOfMacID[i] = par_signature;
//						clockTime_toggleButton_ms[i] = clock_time_ms();
//						uint16_t K9BHC_Add = Sw_Flash_Data_Val.K9B_Data.Scene[i].AddK9B;
//						uint16_t SceneID		= 0x0000;
//
//						int ButtonPos_Buff = RD_CheckButtonPosK9BHC(Sw_Flash_Data_Val.K9B_Data.Scene[i].Button_ID, key);
//						if(ButtonPos_Buff != -1) SceneID = Sw_Flash_Data_Val.K9B_Data.Scene[i].Scene_ID_OnePress[ButtonPos_Buff];
//
//
//						RD_MessK9BHc_Press(K9BHC_Add, K9BButton_ID, 1, SceneID);
//
//						if(0x0000 != SceneID) RD_Call_Scene(SceneID, K9BButton_ID);
//
//
//						signatureLast = par_signature;
//
//						char UART_TempSend1[64] = {0};
//						sprintf(UART_TempSend1,"send K9B HC: 0x%08x, button: %d, Scene: 0x%04x",macDevice, K9BButton_ID, SceneID);
//						uart_CSend(UART_TempSend1);
//					}
//				}
//			}
//		}
//	}
//
//
//	for(int i=0; i< MAX_NUM_K9BHC; i++)
//	{
//		if(macDevice == Sw_Flash_Data_Val.K9B_Data.Scene[i].MacK9B)
//		{
//			scanSttReturn = 1;
//		}
//	}
//	return scanSttReturn;
//}
//
//extern uint32_t Input_Array[NUM_OF_INPUT];
//
//void RD_K9B_ScanPairOnOff(void)
//{
//	static uint8_t Sw1_PairK9B_OnOff_Flag = 0;
//	const uint8_t timeOut_K9BPairOnOff_s =10;
//	static uint32_t clockTime_K9BPairOnOff_s =0;
///*----------------------------Set------------------------------------------*/
//	if(( (Button1_Hold_Flag + Button2_Hold_Flag) == 1)|| (1 == Sw1_PairK9B_OnOff_Flag)) // only 1 buttuon hold
//	{
//		uint8_t NumOfButton_Pressing =0;
//		uint8_t ButtuonId_Hold =0;
//		for(int i=0; i< NUM_OF_ELEMENT; i++)
//		{
//			if((INPUT_READ(i)) == 0)
//			{
//				NumOfButton_Pressing++; // press
//				ButtuonId_Hold = i+1;  // istart 0
//			}
//		}
//
//		if(NumOfButton_Pressing ==1)
//		{
//			Sw1_PairK9B_OnOff_Flag =0;
//			clockTime_K9BPairOnOff_s = clock_time_s();
//			RD_K9B_Pair_OnOffSetFlag(ButtuonId_Hold);
//			Button1_Hold_Flag=0;
//			Button2_Hold_Flag=0;
//		}
//	}
//
///*-------------------------------- clear-----------------------------------*/
//	if(clock_time_s() < clockTime_K9BPairOnOff_s) clockTime_K9BPairOnOff_s =0;
//	if((0x00 != RD_K9B_Pair_OnOffGetFlag()) &&  (clock_time_s() - clockTime_K9BPairOnOff_s > timeOut_K9BPairOnOff_s))
//	{
//		uart_CSend("Time out Pair K9B onOff \n");
////		RD_Sw_SingleBlinkSet(RD_K9B_Pair_OnOffGetFlag(), 3, 300);
//		//RD_Flash_SaveK9BOnOff(0x00000000, 0x00, RD_K9B_Pair_OnOffGetFlag());
//		RD_K9B_Pair_OnOffClearFlag();
//	}
//}
//
//
//void RD_K9B_TimeOutScanK9BHC(void)
//{
//	const uint32_t TimeOutK9BPiartHc_ms = 10000;
//
//	if(clock_time_ms() < Sw_Woring_K9B_Val.ClockTimeSetHc_ms ) Sw_Woring_K9B_Val.ClockTimeSetHc_ms =0; // resset count
//
//	// pair time out
//	if( (0x01 == Sw_Woring_K9B_Val.Pair_K9BHc_Flag) && ( clock_time_ms() - Sw_Woring_K9B_Val.ClockTimeSetHc_ms > TimeOutK9BPiartHc_ms))
//	{
//		uart_CSend("time out ScanK9BHC\n");
//		RD_Mess_ScanK9BHC_Rsp(RD_GATEWAYADDRESS, 0x00, 0x00, 0x00 );
//		RD_K9B_PairHCSet(0x00, 0x0000); //clear phase working
////		RD_SwitchAc4Ch_BlinkSet(3, 500);
//	}
//}

