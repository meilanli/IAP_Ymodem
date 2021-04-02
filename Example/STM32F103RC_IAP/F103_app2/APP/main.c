#include "main.h"
#include "string.h"
#include "stdio.h"

#include "delay.h"
#include "iap.h"
#include "usart.h"

//看门狗初始化
int IWDG_Init(void)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);//关闭IWDG_PR和IWDG_RLR的写保护
	IWDG_SetReload(0xfff);//设置重装载值为0xfff   间隔 = 载值*(1/(40000/分频数)
	IWDG_SetPrescaler(IWDG_Prescaler_64);//设置预分频系数为64,大概6S左右//
	IWDG_ReloadCounter();
	IWDG_Enable();//使能看门狗
	return 0;
}



int main(void)
{
    app_iap_init(); 
    
    Delay_Init(72);
	
	IO_Init();
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
    
	IWDG_Init();
    
	delay_ms(100);
    
	while(1)
	{
		IO_OUT_LED2(1);
		IO_OUT_LED1(0);
		delay_ms(100);
		IO_OUT_LED2(0);
		IO_OUT_LED1(1);
		delay_ms(100);
		IWDG_ReloadCounter();
	}
}
