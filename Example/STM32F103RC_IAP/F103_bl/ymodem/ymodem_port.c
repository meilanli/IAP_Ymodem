
//---------------需要移植的接口,也可以在头文件中直接定义成宏-----------//
#include "ymodem_port.h"

//要使用的底层驱动
#include "usart.h"
#include "flash.h"

//需要一个完全不精确的延时
//用于间隔两个答复帧, 如果两次回复没有间隔时间，上位机很可能无法处理
//一般不用修改
void ymode_dev_delay(uint16_t ms)
{
    uint16_t temp=1000;
    while(ms--)
    {
        while(temp--);
    }
}

//发送一个字节
void ymode_dev_send_byte(uint8_t ch)
{
//    usart_tx(&ch, 1);
    USART_SendData(USART1, ch);
}

/******************
*读取一帧数据
*输入：
*  buf用于存放读取到的数据
*  len要读取的长度
*返回：实际接收到的数据长度
******************/
uint32_t ymode_dev_recv(uint8_t *buf, uint16_t len)
{
    return usart_rx(1, buf, len);
}

//接收使能, 不需要则留空(类似RS485流控的通信需要)
void ymode_dev_recv_en(void)
{
//    usart_rx_enable();
}

/********擦除flash*********
*输入：
*  addr    起始地址
*  len     数据长度byte
*注意计算擦除页数
***************************/
void ymode_dev_ease_flash(uint32_t addr, uint32_t len)
{
    EraseFlash(addr, len);
}

/**********写入数据*******
*输入：
*  addr：写入起始地址
*  pData：数据
*  len：    数据长度
*注意字节对齐
**************************/
void ymode_dev_write_flash(uint32_t addr, uint8_t *pData, uint32_t len)
{
    WriteFlash(addr, pData, len);
}

/**********通信外设初始化*******
*如果不需要，可以自行初始化
**************************/
void ymodem_dev_init(void)
{
    usart_init(1, 115200);
}
