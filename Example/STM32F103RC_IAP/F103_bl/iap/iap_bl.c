//iap_bl.c
//添加到bootloader工程
#include "iap.h"	//与app工程中的iap.h是同一个
#include "stdint.h"

//检查是否需要升级
void bl_iap_init(void)
{
    if( *(uint32_t *)UPDATE_FLAG_ADDRESS != UPDATE_FLAG_VALUE )
    {
        bl_go_to_app();
    }
}

typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;

void bl_go_to_app(void)
{
    /* Test if user code is programmed starting from address "APP_ADDRESS" */
    if (((*(__IO uint32_t*)APP_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
    { 
        __disable_irq();    //关总中断
        /* Jump to user application */
        JumpAddress = *(__IO uint32_t*) (APP_ADDRESS + 4);
        Jump_To_Application = (pFunction) JumpAddress;
        
        /* Initialize user application's Stack Pointer */
        __set_MSP(*(__IO uint32_t*) APP_ADDRESS);
        
        /* Jump to application */
        Jump_To_Application();
    }
}
