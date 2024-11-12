#ifndef RD_FLASH_H_
#define RD_FLASH_H_


#include "proj/tl_common.h"
#include "proj/mcu/watchdog_i.h"
#include "vendor/common/user_config.h"
#include "proj_lib/rf_drv.h"
#include "proj_lib/pm.h"
#include "proj_lib/ble/blt_config.h"
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/sig_mesh/app_mesh.h"

#include "../RD_in_out/rd_in_out.h"
#include "../RD_meshdata/rd_meshdata.h"
#include "../RD_hardware/rd_hardware.h"
#include "../RD_secure/rd_secure.h"
#include "../RD_log/RD_log.h"

#define FLASH_ADDR               		0x78000//0x023000
#define DATA_POWER_OFF_FLASH_ADDR      	0x79000//0x023000
#define FLASH_BUFF_LEN           		91
#define FLASH_DATA_SIZE			 		91
#define FLASH_DATA_POWER_OFF_SIZE		9
#define SCENE_MAX 						5
#define NUM_ELEMENT						2

typedef struct
{
  uint32_t MacK9B[MAX_NUM_K9ONOFF];
  uint8_t K9B_BtKey[MAX_NUM_K9ONOFF];
  uint8_t NumSaveNext;  // num off array ID save to struct. NumSaveNext>=MAX_NUM_K9ONOFF NumSaveNext=0;
} RemoteK9BOnOff_Str;

typedef struct
{
	uint32_t MacK9B;
	uint8_t K9B_BtKey;
}infor_k9b_t;

typedef struct
{
	infor_k9b_t onoff[MAX_NUM_K9ONOFF];
	uint8_t num_onoff;
}remotek9b_call_onoff_t;

typedef struct
{
	uint16_t scence;
	uint8_t but;
}but_pair_scence_t;

typedef struct
{
  uint32_t MacK9B;
  uint16_t AddK9B;
  uint16_t Scene_ID_OnePress[MAX_MUM_K9BPRESSTYPE];
  uint8_t Button_ID[MAX_MUM_K9BPRESSTYPE];
  //uint16_t Scene_ID_DoublePress[7];
  uint8_t Num_Of_Button;
} RemoteK9BScene_Str;

typedef struct
{
	RemoteK9BOnOff_Str OnOff[NUM_ELEMENT];
	RemoteK9BScene_Str Scene[MAX_NUM_K9BHC]; // Max 3 remote
}RemoteK9B_Str;

typedef struct
{
	uint8_t Factory_Check;
	uint8_t Future[3];
	uint8_t Secure_RD;
	uint8_t PowerUpStt;
	uint16_t Gw_Add;
	uint8_t input_setting[NUM_OF_INPUT];
} Sw_Flash_Data;

extern Sw_Flash_Data Sw_Flash_Data_Val;



void RD_Flash_Init(void);
void RD_Flash_Save_Secure(uint8_t Secure_Stt);
void RD_Flash_SaveGwAdd(uint16_t Gw_Add);
void RD_Flash_Save_DataDefault(void);
void RD_Flash_PowUpConfig(uint8_t PowUpConfig);
void RD_Flash_Save_LinkControl(uint8_t Button_Set, uint16_t Add_Control);

void RD_init_flash_out_handle();

void RD_Flash_SaveK9BOnOff(uint32_t macDevice, uint8_t key, uint8_t SwButtonID);
uint8_t RD_Flash_SaveSceneK9BHC(uint16_t K9BAdd, uint8_t ButtonID, uint16_t SceneID);
uint8_t RD_Flash_DeleteSceneK9BHC(uint16_t K9BAdd);
uint8_t RD_Flash_SaveK9BHc(uint32_t macDevice, uint8_t Num_Of_Button, uint16_t K9BAdd);
uint8_t RD_Flash_DeleteK9BHC(uint16_t K9BAdd);
void RD_Flash_K9BSaveLocalMode(uint8_t local_mode);
void RD_Flash_DeleteAllK9BOnOff(uint8_t SwButtonID);


void rd_handle_setting_input(u8 *par, u16 gw_addr);
#endif

