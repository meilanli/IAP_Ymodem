/*******************
*2021-2-21 by meilanli
********************/
//添加到bootloader工程
#include "iap.h"	//与app工程中的iap.h是同一个
#include "stdint.h"

//检查是否需要升级
void bl_iap_init(void)
{
	//----方式1：检查升级标志---------//
    if( *(uint32_t *)UPDATE_FLAG_ADDRESS != UPDATE_FLAG_VALUE )
    {
		/*****************************************************
		*这里处理下，如果新程序有BUG，导致卡死，则APP无法写入升级标志，导致无法升级。
		*只要不是看门狗复位重启，则正常进入APP，否则就去重新升级。
		*这里针对STM32，不同芯片，判断方式可能不同。
		*
		*这个判断方法可以不要，改写bl_iap_init函数，创建一个新的升级触发方式，
		*比如开机检测某个按键是否被按下;开机等待几秒，看串口是否有升级指令等...
		*或者自信点的，直接跳转APP(一般来说，给用户升级的程序，已经被我们反复调试好了，跑死几率不大)
		*****************************************************/
//		if( RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != SET &&
//			RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != SET )
		{
			bl_go_to_app();
		}
    }
	
	
//	//----方式2：-------//
//	key_init();
//	if( !key1 )
//	{
//		delay_ms(60);
//		if( !key1 )
//		{
//			return;
//		}
//	}
//	bl_go_to_app();


//	//----方式3----------//
//	uint16_t ms=3000;
//	usart1_init(115200);
//	while(ms--)
//	{
//		if( usart1_rx() == 0x5A )
//		{
//			return;
//		}
//		delay_ms(1);
//	}
//	bl_go_to_app();

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
