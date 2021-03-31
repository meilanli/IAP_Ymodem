#ifndef __IAP_H
#define __IAP_H
//bootloader��app��Ҫ������ͷ�ļ�

#define SOC_STM32F1
#include "stm32f10x.h"

//����app�����λ
#define SCB_AIRCR           (*(volatile unsigned long *)0xE000ED0C)
#define SCB_RESET_VALUE     0x05FA0004

#ifdef SOC_STM32F0
#define VECTOR_SIZE 0x10C   //�ж��������С,�����ļ�����DCD����*4
#endif

//Ӧ�ó���ʼ��ַ
#define APP_ADDRESS   (uint32_t)(0x8000000+10*1024)

//������־��ַ
//������app�û����ݷ���һ�𣬷����������Լ�ռ�
#define UPDATE_FLAG_ADDRESS     (APP_ADDRESS-1024)   
#define UPDATE_FLAG_VALUE       0x5A5A      //������־ֵ

//bootloaderʹ�ýӿ�
void bl_iap_init(void);  //�����������������ֱ����ת
void bl_go_to_app(void);    //��ת��app

//appʹ�ýӿ�
void app_iap_init(void);    //����������ô˺���
void app_go_to_update(void);    //ȥ����

#endif

