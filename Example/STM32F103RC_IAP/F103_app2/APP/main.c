#include "main.h"
#include "string.h"
#include "stdio.h"

#include "delay.h"
#include "iap.h"
#include "usart.h"

//���Ź���ʼ��
int IWDG_Init(void)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);//�ر�IWDG_PR��IWDG_RLR��д����
	IWDG_SetReload(0xfff);//������װ��ֵΪ0xfff   ��� = ��ֵ*(1/(40000/��Ƶ��)
	IWDG_SetPrescaler(IWDG_Prescaler_64);//����Ԥ��Ƶϵ��Ϊ64,���6S����//
	IWDG_ReloadCounter();
	IWDG_Enable();//ʹ�ܿ��Ź�
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
