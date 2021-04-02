#include "main.h"
#include "string.h"
#include "stdio.h"

#include "delay.h"
#include "usart.h"
#include "ymodem.h"
#include "iap.h"

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
    bl_iap_init();  //����Ƿ���Ҫ����
    
    Delay_Init(72);
    
//	IO_Init();
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
    
    usart_init(2, 115200);  //����3����printf

    uint8_t rate;
    Ymodem ymode;
    
    ymodem_init(&ymode);    //��������1��ʼ��,���ڴ���
    
    IWDG_Init();
    
	while(1)
	{
        
        if( ymodem_poll(&ymode) == 0)
        {
            printf("\r\nYmodem receive OK!\r\n");
            printf("file name:\"%s\"\r\n", ymode.file_name);
            printf("file size:%dbyte\r\n", ymode.file_size);
            
            bl_go_to_app();
        }
        else
        {
            if( ymode.status == YM_STA_RECVING )
            {
                if( rate != (uint8_t)(100 - ymode.rem_size*100.0/ymode.file_size) )
                {
                    rate = (uint8_t)(100 - ymode.rem_size*100.0/ymode.file_size);
                    printf("Receive: %d%%\r\n", rate);
                }
            }
        }
        ym_timer_1ms_cb();  //�ж�ʱ�����Էŵ���ʱ����
        delay_ms(1);
        
		IWDG_ReloadCounter();
	}
}
