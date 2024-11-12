#include "rd_in_out.h"

static uint8_t Reset_Count_Buff = 0;


void RD_SwitchAC4Ch_CheckHold(void)
{
	static uint16_t Count_BuffBT1, Count_BuffBT2;
	static bool4 Flag_Print1 = 0, Flag_Print2= 0;
//--------------------BUTTON 1 --------------------//
	if(button1_Stt == Button_Keeping)
	{
//		uart_CSend("BT1 Keep\n");
		Count_BuffBT1++;
		if(Count_BuffBT1>= 50000) Count_BuffBT1=50000;
		if(Count_BuffBT1 >= TIME_HOLD_BT)
		{

			if(Flag_Print1 == 0)
			{
				Button1_Hold_Flag =1;
				Flag_Print1=1;
			    uart_CSend("BT1 Hold\n");
			}
		}

	}
	else
	{
		Count_BuffBT1=0;
		Flag_Print1=0;
	}


	//--------------------BUTTON 2 --------------------//
	if(button2_Stt == Button_Keeping)
	{
		Count_BuffBT2++;
		if(Count_BuffBT2>= 50000) Count_BuffBT2=50000;
		if(Count_BuffBT2 >= TIME_HOLD_BT)
		{

			if(Flag_Print2 == 0)
			{
				Button2_Hold_Flag =1;
				Flag_Print2=1;
			    uart_CSend("BT2 Hold\n");
			}
		}
	}
	else
	{
		Count_BuffBT2=0;
		Flag_Print2=0;
	}
}
//
void RD_SwitchAC4Ch_ScanB_V2(void)
{
	static uint8_t Bt1_Count_Buff = 0;
	static uint8_t Bt2_Count_Buff = 0;

	static uint8_t buttonSttBuff[2] = { 0 };
	if (INPUT_READ(0) == 0)
	{
		Bt1_Count_Buff++;
		if (Bt1_Count_Buff == CYCLE_ACTIVE_BUTTON)
		{
			buttonSttBuff[0] = 1;
		}
		if (Bt1_Count_Buff >= (CYCLE_ACTIVE_BUTTON + 1))
		{
			Bt1_Count_Buff = CYCLE_ACTIVE_BUTTON + 1;
		}
	}
	else
	{
		Bt1_Count_Buff = 0;
		buttonSttBuff[0] = 0;
	}

	//------------------update BT1 status------------------------//
	if ((buttonSttBuff[0] == 1) && (button1_Stt == Button_None))
	{
		uart_CSend("Bt 1 Press \n");
		button1_Stt = Button_Press;
	}
	else if (buttonSttBuff[0] == 0)
	{
		if (button1_Stt == Button_Keeping)
		{
			uart_CSend("Reset bt Stt \n");
		}
		button1_Stt = Button_None;
	}
	if (INPUT_READ(1) == 0)
	{
		Bt2_Count_Buff++;
		if (Bt2_Count_Buff == CYCLE_ACTIVE_BUTTON)
		{
			//uart_CSend("BT2 On \n");
			buttonSttBuff[1] = 1;
		}
		if (Bt2_Count_Buff >= (CYCLE_ACTIVE_BUTTON + 1)) {
			Bt2_Count_Buff = CYCLE_ACTIVE_BUTTON + 1;
		}
	}
	else
	{
		Bt2_Count_Buff = 0;
		buttonSttBuff[1] = 0;
	}

	//------------------update BT2 status------------------------//
	if ((buttonSttBuff[1] == 1) && (button2_Stt == Button_None))
	{
		uart_CSend("Bt 2 Press \n");
		button2_Stt = Button_Press;
	}
	else if (buttonSttBuff[1] == 0)
	{
		if (button2_Stt == Button_Keeping)
		{
			uart_CSend("Reset bt Stt \n");
		}
		button2_Stt = Button_None;
	}
	RD_SwitchAC4Ch_CheckHold();
	/*------------------------------- Check Hold Button-------------------------------*/
}
//
//void RD_Socket_WaitDetechZero(void)
//{
//	uint16_t overTimeBuff =0;
//	uint8_t detectRezoState_Buff, detectRezoState_Past_Buff, detectRezoState_Unnoise_Buff,  detectRezoState_Unnoise_Buff2;
//	detectRezoState_Unnoise_Buff = detectRezoState_Past_Buff = detectRezoState_Buff = gpio_read(DETECT_ZERO_PIN);
//	do
//	{
//		overTimeBuff ++;
//		detectRezoState_Past_Buff = gpio_read(DETECT_ZERO_PIN);
//		sleep_us(50);
//		detectRezoState_Buff = gpio_read(DETECT_ZERO_PIN);
//
//		sleep_us(500);
//
//		detectRezoState_Unnoise_Buff = gpio_read(DETECT_ZERO_PIN);
//		sleep_us(50);
//		detectRezoState_Unnoise_Buff2 = gpio_read(DETECT_ZERO_PIN);
//		if(overTimeBuff>=NUM_CHECK_DETECH_MAX)
//		{
//			uart_CSend("break \n");
//			break;
//		}
//	}
//	while( !( (detectRezoState_Past_Buff != 0) && (detectRezoState_Buff != 0) && (detectRezoState_Unnoise_Buff == 0) && (detectRezoState_Unnoise_Buff2 ==0)) );
//}
//
//void RD_SwitchAC4Ch_OnOffZero(GPIO_PinTypeDef RelayPin, Relay_Stt_Type OnOff_State)
//{
//
//	#if (METHOD_DETECT_ZERO == DETECT_ZERO_BY_FAILING)
//		RD_Socket_WaitDetechZero();
//		if(OnOff_State == Relay_On)
//		{
//			sleep_us(TIME_DETECT_ON);  // for switch 1-2-3-4 4700
//		}
//		else
//			sleep_us(TIME_DETECT_OFF);    // for switch 1-2-3-4 3200
//	#endif
//	gpio_write(RelayPin, OnOff_State);
//}
//
void RD_SwitchAC4Ch_UpdateCtr(void)
{
	/*--------------------update Relay Stt------------------	 */
	if(button1_Stt == Button_Press)
	{
		uart_CSend("BT1 To Keep\n");
		button1_Stt = Button_Keeping;
		u8 state = RD_get_on_off(0,0);
		u8 toggle = (state > 0) ? 0:1;
		RD_mod_io_onoff(toggle, 0,1);
//		RD_Call_LinkControl(1, relay_Stt[0]);
	}
	if(button2_Stt == Button_Press)
	{
		button2_Stt = Button_Keeping;
		u8 state = RD_get_on_off(1,0);
		u8 toggle = (state > 0) ? 0:1;
		RD_mod_io_onoff(toggle, 1,1);
//		RD_Call_LinkControl(2, relay_Stt[1]);
	}
}

void RD_ScanKickAll(void)
{
	if(Kick_all_Flag==1)
	{
		static uint32_t countDownt =0;
		static uint8_t check_reset_Flag=0;
		if(check_reset_Flag == 0)
		{
			check_reset_Flag =1;
			countDownt = clock_time_ms();
		}
		if( (check_reset_Flag == 1) && ((clock_time_ms() - countDownt) >= TIME_RESET_ALL) )
		{
			uart_CSend("\nFactory Reset\n");
			RD_mod_in_out_factory_reset();
		}
	}
}

void RD_SwitchAC4Ch_ScanReset(void)
{
	/*--------------------------Factory Reset------------------------*/

//	static uint16_t checkTimeBuff2=0;

	if((Button1_Hold_Flag + Button2_Hold_Flag) == 2)
		{
			RD_ev_log("call hold 2 but,Reset_Count_Buff: %d \n",Reset_Count_Buff);
			RD_mod_in_out_factory_reset();
//			checkTimeBuff2=0;
			Reset_Count_Buff++;
			Button1_Hold_Flag=0;
			Button2_Hold_Flag=0;
		}
//	else
//	{
//		checkTimeBuff2++;
//		if(checkTimeBuff2 >= TIME_OUT_RESET)
//		{
//			checkTimeBuff2=TIME_OUT_RESET;
//			Reset_Count_Buff=0;
//		}
//	}
	if(Reset_Count_Buff == 3 ||   ( (reset_Mess_Flag== 1) && (get_provision_state() == STATE_DEV_UNPROV)) )
	{
		RD_mod_in_out_factory_reset();
	}
	RD_ScanKickAll();
}
