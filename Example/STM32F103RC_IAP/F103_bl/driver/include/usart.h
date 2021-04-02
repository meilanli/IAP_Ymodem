#ifndef __USART_H_
#define __USART_H_

#include "stdint.h"


#define FIFO_NUM 1050	//DMA缓冲区大小

void usart_init(uint8_t port, uint32_t baud);	//串口初始化
void usart_tx(uint8_t port, uint8_t *tx_buf, uint16_t len);	//发送数据
uint32_t usart_rx(uint8_t port, uint8_t *rx_buf, uint32_t rx_len);	//接收数据，返回接收长度
void usart_rx_enable(uint8_t port);	//开始接收

#endif
