#ifndef __YMODEM_PORT_H
#define __YMODEM_PORT_H
#include "stdint.h"

#include "iap.h"     //iap中定义了APP_ADDRESS
//ymodem接收存放起始地址
#define YM_WRITE_START_ADDR        APP_ADDRESS

void ymode_dev_delay(uint16_t ms);
    
void ymodem_dev_init(void);
void ymode_dev_send_byte(uint8_t ch);
uint32_t ymode_dev_recv(uint8_t *buf, uint16_t len);
void ymode_dev_recv_en(void);

void ymode_dev_ease_flash(uint32_t addr, uint32_t len);
void ymode_dev_write_flash(uint32_t addr, uint8_t *pData, uint32_t len);


#endif
