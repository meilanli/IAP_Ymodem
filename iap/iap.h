#ifndef __IAP_H
#define __IAP_H
//bootloader和app都要包含此头文件

#define SOC_STM32F1
#include "stm32f10x.h"

//用于app软件复位
#define SCB_AIRCR           (*(volatile unsigned long *)0xE000ED0C)
#define SCB_RESET_VALUE     0x05FA0004

#ifdef SOC_STM32F0
#define VECTOR_SIZE 0x10C   //中断向量表大小,启动文件里面DCD数量*4
#endif

//应用程序开始地址
#define APP_ADDRESS   (uint32_t)(0x8000000+10*1024)

//升级标志地址
//建议与app用户数据放在一起，方便操作，节约空间
#define UPDATE_FLAG_ADDRESS     (APP_ADDRESS-1024)   
#define UPDATE_FLAG_VALUE       0x5A5A      //升级标志值

//bootloader使用接口
void bl_iap_init(void);  //检查升级，不升级则直接跳转
void bl_go_to_app(void);    //跳转到app

//app使用接口
void app_iap_init(void);    //开机尽快调用此函数
void app_go_to_update(void);    //去升级

#endif

