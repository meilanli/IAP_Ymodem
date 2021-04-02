#include "main.h"
#include "string.h"
#include "stdio.h"

#include "delay.h"
#include "usart.h"
#include "ymodem.h"
#include "iap.h"

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
    bl_iap_init();  //检查是否需要升级
    
    Delay_Init(72);
    
//	IO_Init();
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
    
    usart_init(2, 115200);  //串口3用于printf

    uint8_t rate;
    Ymodem ymode;
    
    ymodem_init(&ymode);    //包含串口1初始化,用于传输
    
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
        ym_timer_1ms_cb();  //有定时器可以放到定时器中
        delay_ms(1);
        
		IWDG_ReloadCounter();
	}
}
