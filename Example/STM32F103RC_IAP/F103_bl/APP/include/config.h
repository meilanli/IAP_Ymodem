#ifndef __CONFIG_H_
#define __CONFIG_H_

#include "stm32f10x.h"

#define RT_THREAD_OS

#define BSP_DEBUG 	0

#ifndef NULL
#define NULL 0L
#endif

#if BSP_DEBUG
#define REG_HOLD_DEBUG_MEM_ADDR				0x04
#define REG_HOLD_DEBUG_ADDR						0x05
#endif





#define REG_HOLD_SLAVE_ID_ADDR				0x20		//地址 1-247
#define REG_HOLD_USART_BUAD_ADDR			0x21		//波特率	2400-57600


//modbus默认参数
#define MODBUS_SLAVE_ID_DEF		0x01
#define MODBUS_BAUD_DEF				9600

#define MODBUS_PAR_DEFAULT 	{	MODBUS_SLAVE_ID_DEF, \
															MODBUS_BAUD_DEF}


//所有默认参数
#define APP_PAR_DEFAULT {	0x55, \
													MODBUS_PAR_DEFAULT }

													
													
//存放需要保存的数据
struct ModbusParameter
{
	uint8_t SlaveID;
	uint16_t Baud;
};
													
struct AppParameter
{
	uint8_t SaveFlag;
	struct ModbusParameter modbus;
};

extern struct AppParameter g_AppPar;

#endif

