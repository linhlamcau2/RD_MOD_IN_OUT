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
	uint8_t Scene_ID[2];
	uint8_t Null_Byte;
	uint8_t Tss[2];
	uint8_t Future[3];
} Call_Scene_Str;

#define TTS_DEFAULT 0x0005



// RD_EDIT RD_VENDOR cac ham xu ly.h
#define mesh_cmd_sig_RD_respone_status      		(0)
#define mesh_cmd_sig_RD_respone_status_type      	(0)
#define mesh_cmd_sig_RD_respone_status_SCENE      	(0)
#define mesh_cmd_sig_RD_respone_status_Switch		(0)

//#define   cb_vd_light_onoff_get 					(0)
#define   cb_vd_light_onoff_status 					(0)

extern uint16_t RD_GATEWAYADDRESS;

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

int RD_Messenger_ProcessCommingProcess_TRAIN(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);

void RD_Send_Relay_Stt(uint8_t Relay_ID, uint8_t Relay_Stt);


void RD_Call_Scene(uint16_t Scene_ID, uint8_t Mess_ID);

void RD_Call_Scene(uint16_t Scene_ID, uint8_t Mess_ID);

void rd_update_input_stt(u8 idx_in, u8 status_in, u16 sence);

void rd_init_queue_rsp();
void rd_call_tx(u16 op_code, u8 *par, u16 par_len, u16 addr_dst);
void rd_handle_tx();
void rd_call_tx2(u16 op_code, u8 *par, u16 par_len, u16 addr_dst, u16 addr_src);
void rd_input_call_sence(uint16_t sence_id);

#endif /* RD_MESSDATA_H_ */
