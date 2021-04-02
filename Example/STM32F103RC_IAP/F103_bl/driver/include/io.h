//=============================================================================
//�ļ����ƣ�LED.h
//���ܸ�Ҫ��LED����ͷ�ļ�
//����ʱ�䣺2013-12-31
//=============================================================================
#ifndef __IO_H
#define __IO_H

#include "stm32f10x.h"

#define IO_OUT_LED1(x)	do{ \
													if(x){GPIO_SetBits(GPIOA, GPIO_Pin_0);} \
													else{GPIO_ResetBits(GPIOA, GPIO_Pin_0);} \
												}while(0)

#define IO_OUT_LED2(x)	do{ \
													if(x){GPIO_SetBits(GPIOA, GPIO_Pin_1);} \
													else{GPIO_ResetBits(GPIOA, GPIO_Pin_1);} \
												}while(0)
												
int IO_Init(void);
#endif
