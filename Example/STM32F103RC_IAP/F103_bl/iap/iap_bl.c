//iap_bl.c
//��ӵ�bootloader����
#include "iap.h"	//��app�����е�iap.h��ͬһ��
#include "stdint.h"

//����Ƿ���Ҫ����
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
        __disable_irq();    //�����ж�
        /* Jump to user application */
        JumpAddress = *(__IO uint32_t*) (APP_ADDRESS + 4);
        Jump_To_Application = (pFunction) JumpAddress;
        
        /* Initialize user application's Stack Pointer */
        __set_MSP(*(__IO uint32_t*) APP_ADDRESS);
        
        /* Jump to application */
        Jump_To_Application();
    }
}
