################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../vendor/mesh/RD_flash/rd_flash.c 

OBJS += \
./vendor/mesh/RD_flash/rd_flash.o 


# Each subdirectory must supply rules for building sources it contributes
vendor/mesh/RD_flash/%.o: ../vendor/mesh/RD_flash/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: TC32 Compiler'
	tc32-elf-gcc -ffunction-sections -fdata-sections -I"C:\Users\NV-Smart03\Downloads\telink_code\RD_SW_Tou_v3\tc_ble_mesh-4.1.0.1\firmware" -I"C:\Users\NV-Smart03\Downloads\telink_code\RD_SW_Tou_v3\tc_ble_mesh-4.1.0.1\firmware\proj\include" -I"C:\Users\NV-Smart03\Downloads\telink_code\RD_SW_Tou_v3\tc_ble_mesh-4.1.0.1\firmware\vendor\common\mi_api\mijia_ble_api" -I"C:\Users\NV-Smart03\Downloads\telink_code\RD_SW_Tou_v3\tc_ble_mesh-4.1.0.1\firmware\vendor\common\mi_api\libs" -I"C:\Users\NV-Smart03\Downloads\telink_code\RD_SW_Tou_v3\tc_ble_mesh-4.1.0.1\firmware\vendor\common\llsync\sdk_src\include" -I"C:\Users\NV-Smart03\Downloads\telink_code\RD_SW_Tou_v3\tc_ble_mesh-4.1.0.1\firmware\vendor\common\llsync\include" -I"C:\Users\NV-Smart03\Downloads\telink_code\RD_SW_Tou_v3\tc_ble_mesh-4.1.0.1\firmware\vendor\common\llsync\cfg" -I"C:\Users\NV-Smart03\Downloads\telink_code\RD_SW_Tou_v3\tc_ble_mesh-4.1.0.1\firmware\vendor\common\llsync\data_template" -D__PROJECT_MESH__=1 -D__telink__ -DCHIP_TYPE=CHIP_TYPE_8258 -Wall -O2 -fpack-struct -fshort-enums -finline-small-functions -std=gnu99 -fshort-wchar -fms-extensions -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


