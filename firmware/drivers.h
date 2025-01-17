/********************************************************************************************************
 * @file	drivers.h
 *
 * @brief	This is the header file for TLSR8258
 *
 * @author	author@telink-semi.com;
 * @date	May 8, 2018
 *
 * @par     Copyright (c) 2018, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#pragma once

#include "proj/mcu/config.h"

#if(__TL_LIB_8255__ || (MCU_CORE_TYPE == MCU_CORE_8255))
#include "drivers/8255/driver_8255.h"
#elif(__TL_LIB_8258__ || (MCU_CORE_TYPE == MCU_CORE_8258))
#include "drivers/8258/driver_8258.h"
#include "drivers/8258/driver_ext/driver_ext.h"
#elif(MCU_CORE_TYPE == MCU_CORE_8278)
#include "drivers/8278/driver_8278.h"
#include "drivers/8278/driver_ext/driver_ext.h"
#else
#include "proj/common/compatibility.h"
#include "proj/mcu/analog.h"
#include "proj/mcu/compiler.h"
#include "proj/mcu/register.h"
#include "proj/mcu/gpio.h"
#include "proj/mcu/pwm.h"
#include "proj/mcu/cpu.h"
#include "proj/mcu/dma.h"
#include "proj/mcu/clock.h"
#include "proj/mcu/clock_i.h"
#include "proj/mcu/random.h"
#include "proj_lib/rf_drv.h"
#endif

#include "proj/drivers/uart.h"
#include "drivers/8258/flash.h"
