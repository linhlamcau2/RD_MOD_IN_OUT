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
#define NUM_OUT							4

#define DELTA_PERCENT_ADC_DEFAULT		5
#define MAX_ADC							4800

typedef struct
{
	u8 mode;
	u8 stt_sence;
	u16 id_sence;
}input_inf_t;

typedef struct
{
	u16 adc_threshold;
	u16 id_sence;
	u8 type;
	u8 delta;
	u8 band;
}adc_inf_t;

typedef struct
{
	u8 Factory_Check;
	u8 Future[3];
	u8 Secure_RD;
	u8 PowerUpStt;
	u16 Gw_Add;
	input_inf_t input_setting[NUM_OUT];
	adc_inf_t adc_setting;
	u8 output_linked[NUM_ELEMENT];
} Sw_Flash_Data;

void RD_Flash_Init(void);
void RD_Flash_Save_Secure(uint8_t Secure_Stt);
void RD_Flash_SaveGwAdd(uint16_t Gw_Add);
void RD_Flash_Save_DataDefault(void);
void RD_Flash_PowUpConfig(uint8_t PowUpConfig);

void RD_init_flash_out_handle();

u8 rd_save_mode_input(u8 idx, u8 mode);
u8 rd_save_linked_io(u8 idx_in,u8 idx_out);
u8 rd_save_powerup_cf(u8 pow_cf);
u8 rd_save_sence_in(u8 idx_in, u8 stt_sence,u16 id_sence);
u8 rd_save_sence_adc(u16 adc_threshold, u16 id_sence,u8 type);
u8 rd_save_delta_adc(u8 delta);

u8 rd_check_state_adc(u16 adc_value);

u8 is_output_linked(u8 idx_out);
u8 get_state_secure();
u8 get_mode_setting_input(u8 idx);
u8 get_ele_linked(u8 idx);
u16 get_adc_sence();
u16 get_adc_threshold();
u16 get_sence_input(u8 idx);
u8 get_stt_sence(u8 idx);
u16 get_delta_adc();

#endif

