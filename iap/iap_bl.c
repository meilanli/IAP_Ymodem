/*******************
*2021-2-21 by meilanli
********************/
//��ӵ�bootloader����
#include "iap.h"	//��app�����е�iap.h��ͬһ��
#include "stdint.h"

//����Ƿ���Ҫ����
void bl_iap_init(void)
{
	//----��ʽ1�����������־---------//
    if( *(uint32_t *)UPDATE_FLAG_ADDRESS != UPDATE_FLAG_VALUE )
    {
		/*****************************************************
		*���ﴦ���£�����³�����BUG�����¿�������APP�޷�д��������־�������޷�������
		*ֻҪ���ǿ��Ź���λ����������������APP�������ȥ����������
		*�������STM32����ͬоƬ���жϷ�ʽ���ܲ�ͬ��
		*
		*����жϷ������Բ�Ҫ����дbl_iap_init����������һ���µ�����������ʽ��
		*���翪�����ĳ�������Ƿ񱻰���;�����ȴ����룬�������Ƿ�������ָ���...
		*�������ŵ�ģ�ֱ����תAPP(һ����˵�����û������ĳ����Ѿ������Ƿ������Ժ��ˣ��������ʲ���)
		*****************************************************/
//		if( RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != SET &&
//			RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != SET )
		{
			bl_go_to_app();
		}
    }
	
	
//	//----��ʽ2��-------//
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


//	//----��ʽ3----------//
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
