#ifndef __USART_H_
#define __USART_H_

#include "stdint.h"


#define FIFO_NUM 1050	//DMA��������С

void usart_init(uint8_t port, uint32_t baud);	//���ڳ�ʼ��
void usart_tx(uint8_t port, uint8_t *tx_buf, uint16_t len);	//��������
uint32_t usart_rx(uint8_t port, uint8_t *rx_buf, uint32_t rx_len);	//�������ݣ����ؽ��ճ���
void usart_rx_enable(uint8_t port);	//��ʼ����

#endif
