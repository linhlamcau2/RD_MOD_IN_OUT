#ifndef RD_HARDWARE_TYPE_H_
#define RD_HARDWARE_TYPE_H_


#define VERSION_FIRM_H				0x01   // sw new cover
#define VERSION_FIRM_L				0x01

#define RD_PROVIDER					0x0001
#define VHT_PROVIDER				0x0200
#define HUE_PROVIDER				0x0201
#define PROVIDER_ID					RD_PROVIDER

#define	Device_BROADCRARD_0		0x0D	//Switch
#define	Device_BROADCRARD_3		0x01
#define	Device_BROADCRARD_4		((PROVIDER_ID >>8) & 0xFF) // khong dao
#define	Device_BROADCRARD_5		(PROVIDER_ID       & 0xFF) // khong dao
#define	Device_BROADCRARD_6		VERSION_FIRM_H
#define	Device_BROADCRARD_7		VERSION_FIRM_L
#define	Device_BROADCRARD_2		0x01
#define	Device_BROADCRARD_1     0x01

#define CONFIG_POWUP_EN			1
#define CYCLE_ACTIVE_BUTTON			10 // num of read CYCLE_ACTIVE_BUTTON times active iss press
#define CYCLE_DETECT_LEVEL			20
#define CYCLE_READ_BT_MS			5 // time of 1 cycle check read bt
#define CYCLE_HOLD_BUTTON			500
#define CYCLE_READ_ADC_MS			200
#define COUNT_CHECK_ADC				5
#define TIME_MAX_SEND_ADC_MINUTE	5

#define FAST_PROVISION_EN_RD			1
#define K9B_REMOTE_SUPPORT_ON		1
#define CONFIG_POWUP_EN				1

#define MAX_NUM_K9BHC				5
#define MAX_NUM_K9ONOFF				5
#define MAX_MUM_K9BPRESSTYPE		12 //Ver 2 7 // 3 button -> 7 type press 001-010-100-110-011-101-111

#define K9B_RD_MESS_EN				0		// new format of RD
#define K9B_TYPE_CTR_LOCAL_DF		K9B_CTR_LOCAL_ONOFF

#define K9B_BLE_CID_ATM				0x09E2 // atomosic CID ble
#define K9B_BLE_CID_RD				0x2804


#define K9B_CTR_LOCAL_ONOFF				1	// Mode V1 toggle anf offall button 6
#define K9B_CTR_LOCAL_SCENE				2	// Mode V2 control as Scene offall button 6


#define RD_VID_TYPE 	((VERSION_FIRM_H << 8) | VERSION_FIRM_L)
#define RD_PID_TYPE 	(0xFFFF & ((Device_BROADCRARD_0 << 12) | (Device_BROADCRARD_1 << 8) | Device_BROADCRARD_2))


#define SECURE_ON					1

#endif
