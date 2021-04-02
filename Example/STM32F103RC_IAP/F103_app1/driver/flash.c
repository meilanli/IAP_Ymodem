#include "flash.h"

void ReadFlash(uint32_t addr, uint8_t *data, uint16_t length)
{
	uint16_t i;
	if( addr+length <= FLASH_SIZE+FLASH_START_ADDR )
	{
		for (i = 0; i < length; i++)
		{
				data[i] = *(uint8_t *)(addr+i);
		}
	}
}

void EraseFlash(uint32_t addr, uint16_t length)
{
    uint16_t i;
    
    if( (addr-FLASH_START_ADDR) % PAGE_SIZE ) return;
    
    FLASH_Unlock();//先解锁
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); //清除相应的标志位
    
    for(i=0; i<= (length/(PAGE_SIZE+1)); i++)
	{
		if( FLASH_ErasePage(addr + i*(PAGE_SIZE)) != FLASH_COMPLETE)
        {            
            break;
        }
	}
    FLASH_Lock();//上锁
}

int8_t WriteFlash(uint32_t addr, uint8_t *data, uint16_t length)
{
	uint32_t end_addr   = addr + length;

	if(end_addr > FLASH_SIZE+FLASH_START_ADDR) return -1;
	
	FLASH_Unlock();//先解锁
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); //清除相应的标志位
    
	while (addr < end_addr)
	{
		if (FLASH_ProgramWord( addr, *((uint32_t *)data)) == FLASH_COMPLETE )
		{
			if (*(uint32_t *)addr != *(uint32_t *)data)
			{
					goto _exit0;
			}
			addr += 4;
			data  += 4;
		}
		else
		{
			break;
		}
	}
	FLASH_Lock();//上锁
	return 0;
_exit0:
	FLASH_Lock();//上锁
	return -1;
}

