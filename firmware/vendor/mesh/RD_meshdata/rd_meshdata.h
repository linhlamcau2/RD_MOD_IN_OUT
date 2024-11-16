#ifndef RD_MESS_DATA_H
#define RD_MESS_DATA_H

#include "proj/tl_common.h"
#include "proj/mcu/watchdog_i.h"
#include "vendor/common/user_config.h"
#include "proj_lib/rf_drv.h"
#include "proj_lib/pm.h"
#include "proj_lib/ble/blt_config.h"
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "../../common/scene.h"


#include "../RD_in_out/rd_in_out.h"
#include "../RD_hardware/rd_hardware.h"
//#include "../RD_Secure/RD_Secure.h"

//#include "../RD_Flash/RD_Flash.h"
//extern uint8_t reset_Mess_Flag;
//extern uint8_t Train_Factory;


#define SENSOR_OPCODE_SWITCH 1

#define RD_OPCODE_TYPE_SEND				(0xE0)
#define RD_OPCODE_TYPE_RSP				(0xE1)

#define RD_HEADER_SET_TYPE_DEVICE		(0x0001)
#define RD_HEADER_SAVE_GW_ADD			(0x0002)
#define RD_HEADER_RSP_TYPE_DEVICE		(0x0003)
#define RD_HEADER_FACTORY_TEST_END		(0x0004)
#define RD_HEADER_KICK_ALL				(0xFFFF)

#define RD_HEADER_INPUT_STATUS				(0x0115)
#define RD_HEADER_ADC_STATUS				(0x0215)
#define RD_HEADER_SETTING_INPUT				(0x0315)
#define RD_HEADER_LINKER_INPUT				(0x0415)
#define RD_HEADER_SETTING_SENCE_INPUT		(0x0515)
#define RD_HEADER_STATUS_ALL_INPUT			(0x0615)

#define RD_HEADER_RELAY_STARTUP				(0x100b)

#define RD_OPCODE_SCENE_SEND			(0xE2)
#define RD_OPCODE_SCENE_RSP				(0xE3)
#define RD_OPCODE_INPUT_RSP				(0x52)

#define RD_OPCODE_TRAIN_FAC						0xFE

#define MANY_HEADER_EN					1



#define RD_AUTO_CREATE_GR					(0x0C0A)
#define RD_AUTO_DELETE_GR					(0x0D0A)



#define RD_OPCODE_POWER 						(0x0001)

#define RD_VENDOR_EN							1

#define RD_MAXRESPONESEND 						(2)

#define HEADER_CONTROL							0x0104

#define GW_ADDRESS								0x0001

#define TIME_CYCLE_MESS_MS   					300		// time between 2 mess

typedef struct
		{
			uint8_t Header[2];
			uint8_t Relay_Stt[4];
			uint8_t Future[2];
		} STT_Relay_Mess;
STT_Relay_Mess STT_Relay_Mess_Config;

typedef struct
		{
			uint8_t Header[2];
			uint8_t Scene_ID[2];
			uint8_t Relay_Stt[4];
		} Save_Scene_Mess;
Save_Scene_Mess Save_Scene_Mess_Config;

typedef struct
		{
			uint8_t Header[2];
			uint8_t Scene_ID[2];
			uint8_t Future[4];
		} Delete_Scene_Mess;
Delete_Scene_Mess Delete_Scene_Mess_Config;
typedef struct
	{
		uint8_t Scene_ID[2];
		uint8_t Null_Byte;
		uint8_t Tss[2];
		uint8_t Future[3];
	} Call_Scene_Str;


typedef struct
	{
		uint8_t Header[2];
		uint8_t Mac[4];
		uint8_t Button_ID;
		uint8_t Future[3];
	} K9B_RspHc_Str;

typedef struct{
	unsigned char Header[2];
	unsigned char MainType;
	unsigned char Feature;
	unsigned char TypeScene;
	unsigned char Reserved[3];
}RD_TypeDevice_Message;
 RD_TypeDevice_Message	vrts_RD_TypeDeviceMessage_Config;

 typedef struct
 		{
 			uint8_t Data[8];
 		} FPT_Rsp_Mesh_str;
FPT_Rsp_Mesh_str FPT_Rsp_Mesh_Null;


#define TTS_DEFAULT 0x0005
extern uint8_t reset_Mess_Flag;

extern uint8_t Train_Factory  ;


// RD_EDIT RD_VENDOR cac ham xu ly.h
#define mesh_cmd_sig_RD_respone_status      		(0)
#define mesh_cmd_sig_RD_respone_status_type      	(0)
#define mesh_cmd_sig_RD_respone_status_SCENE      	(0)
#define mesh_cmd_sig_RD_respone_status_Switch		(0)

//#define   cb_vd_light_onoff_get 					(0)
#define   cb_vd_light_onoff_status 					(0)

extern uint16_t RD_GATEWAYADDRESS;
extern uint16_t RD_ELEMENT_ADDR_1;
extern uint16_t RD_ELEMENT_ADDR_2;
extern uint16_t RD_ELEMENT_ADDR_3;
extern uint16_t RD_ELEMENT_ADDR_4;
int mesh_cmd_sig_set_switch(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par) ; //SENSOR_GET
int RD_Messenger_ProcessCommingProcess_SCENE(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int RD_Messenger_ProcessCommingProcess_Type(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);

void Send_Relay_Stt_Message(void);
//Send 1 relay Stt- opcode 0x52- use it when press button
void Send_Relay_Stt_Message_RID(uint8_t Relay_ID);
// send all 4 relay stt opcode E3 - use when GW ask and joy mesh
void Send_Relay_Stt_Message_RALL(uint16_t GW_ADR);
void Send_Relay_Stt_Message_RALL_PowerUp(void);
// check bind all
int RD_Messenger_BindAll(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);


int cb_vd_light_onoff_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int RD_mesh_cmd_sig_cfg_bind(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);


int RD_Messenger_ProcessCommingProcess_TRAIN(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);

void RD_Send_Relay_Stt(uint8_t Relay_ID, uint8_t Relay_Stt);

void Send_Relay_SttCtrAll_ReUp_Message(uint16_t GW_ADR, uint8_t relay_Stt[5]);
/**
 * @brief call Scene if configed Stair Scene before
 *
 * @param Button_ID : button press
 * @param Type_Scene : Scene oon or off
 */
void RD_Call_LinkControl(uint8_t Button_ID, uint8_t Type_Scene);
/**
 * @brief send update stt of relay_id to gateway. through G_on_off_status opdoce
 *
 * @param Relay_ID 1->4: Relay Id want send
 */
void RD_Update_Relay_Stt(uint8_t Relay_ID);

void RD_Call_Scene(uint16_t Scene_ID, uint8_t Mess_ID);

void RD_Mess_K9BPairHC_Rsp(uint16_t Gw_Add_Buff, uint8_t modeSet, uint16_t K9BAdd, uint8_t Num_Of_Button, uint8_t RspStt);

void RD_Mess_ScanK9BHC_Rsp(uint16_t Gw_Add_Buff, uint32_t K9Bmac, uint8_t type, s8 TxPow);

void RD_MessK9BHc_Press(uint16_t K9B_Add, uint8_t ButtonID, uint8_t ModeID, uint16_t SenceID);

uint8_t RD_K9B_ScanOnOff(uint32_t macDevice, uint8_t key, uint32_t par_signature);
void RD_Handle_ScanK9BHc(uint8_t par[8], uint16_t Gw_Add_Buff);
void RD_Handle_K9BSaveScene(uint8_t par[8], uint16_t Gw_Add_Buff);
void RD_Handle_K9BDeleteScene(uint8_t par[8], uint16_t Gw_Add_Buff);
void RD_Handle_SaveK9BHc(uint8_t par[8], uint16_t Gw_Add_Buff);
void RD_Handle_DeleteK9BHC(uint8_t par[8], uint16_t Gw_Add_Buff);
void RD_Handle_K9BLocalModeConfig(u8 *par, uint16_t Gw_Add_Buff);

void RD_Call_Scene(uint16_t Scene_ID, uint8_t Mess_ID);

void rd_update_input_stt(u8 idx_in, u8 status_in, u16 sence);

void rd_init_queue_rsp();
void rd_call_tx(u16 op_code, u8 *par, u16 par_len, u16 addr_dst);
void rd_handle_tx();
void rd_call_tx2(u16 op_code, u8 *par, u16 par_len, u16 addr_dst, u16 addr_src);
void rd_input_call_sence(uint16_t sence_id);

#endif /* RD_MESSDATA_H_ */
