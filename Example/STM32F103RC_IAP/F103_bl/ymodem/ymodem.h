#ifndef __YMODEM_H
#define __YMODEM_H
#include "stdint.h"

/**************************************************
*移植方法：
    实现ymodem_port.c里面接口
*使用方法
    1.定义ymodem接收到的文件的存放起始地址YM_WRITE_START_ADDR
    2.定义结构体变量Ymodem xx
    3.调用ymodem_init初始化
    4.1ms定时器中调用ym_timer_1ms_cb
    4.循环调用ymodem_poll
    5.直到ymodem_poll返回0表示一次通信完成
***************************************************/

#define YM_STA_INIT     0
#define YM_STA_RECVING  1
#define YM_STA_DONE     2

typedef struct
{
    uint32_t flash_addr;    //下次要写入的地址
    uint8_t status;     //当前通信状态
    uint32_t rem_size;  //剩余待接收文件长度
    uint32_t file_size; //文件大小
    char file_name[32];    //如果不需要，可以优化
}Ymodem;

//主要初始化结构体的参数
void ymodem_init(Ymodem *ym);    

//放到1ms左右的定时器中
void ym_timer_1ms_cb(void);

//ymodem主函数，循环调用即可
int8_t ymodem_poll(Ymodem *ym);

//示例函数
void ymodem_thread_entry(void *Par);

#endif
