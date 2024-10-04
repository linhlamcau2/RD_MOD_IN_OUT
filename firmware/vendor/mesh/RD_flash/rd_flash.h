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


typedef struct
{
  uint32_t MacK9B[MAX_NUM_K9ONOFF];
  uint8_t K9B_BtKey[MAX_NUM_K9ONOFF];
  uint8_t NumSaveNext;  // num off array ID save to struct. NumSaveNext>=MAX_NUM_K9ONOFF NumSaveNext=0;
} RemoteK9BOnOff_Str;

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
	RemoteK9BOnOff_Str OnOff[5]; // 5 button on switch
	RemoteK9BScene_Str Scene[MAX_NUM_K9BHC]; // Max 3 remote
}RemoteK9B_Str;

typedef struct
{
	uint8_t Factory_Check;
	uint8_t Future[3];
	uint8_t Secure_RD;
	uint16_t Gw_Add;
	uint16_t Add_Link_Control[5];   // add  linking control onoff with button
	RemoteK9B_Str K9B_Data;
	uint8_t PowerUpStt;
	uint8_t Type_CtrLocal_K9B;
} Sw_Flash_Data;

extern Sw_Flash_Data Sw_Flash_Data_Val;



void RD_Flash_Init(void);
void RD_Flash_Save_Secure(uint8_t Secure_Stt);
void RD_Flash_SaveGwAdd(uint16_t Gw_Add);
void RD_Flash_Save_DataDefault(void);


#endif

