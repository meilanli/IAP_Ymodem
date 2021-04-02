#ifndef __FLASH_H
#define __FLASH_H
#include "stm32f10x.h"

#define PAGE_SIZE 				1024			//页大小
#define FLASH_START_ADDR	    0x8000000
#define FLASH_SIZE 				(32*PAGE_SIZE)		//flash总大小

void ReadFlash(uint32_t addr, uint8_t *data, uint16_t length);	
int8_t WriteFlash(uint32_t addr, uint8_t *data, uint16_t length);
void EraseFlash(uint32_t addr, uint16_t length);
#endif 
