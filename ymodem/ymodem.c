/***********************************************
2021-2-21 by meilanli
1.Ymodem文件传输协议实现, 支持Ymodem(旧版128)、Ymodem-1K
2.hypertrm超级终端Win7、Tera Term已验证
3.应用在BootLoader中，对接flash擦写，实现IAP升级
***********************************************/
#include "ymodem.h"
#include "ymodem_port.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

//----------宏定义-------------//
#define SOH                     (0x01)  /* start of 128-byte data packet */
#define STX                     (0x02)  /* start of 1024-byte data packet */
#define EOT                     (0x04)  /* end of transmission */
#define ACK                     (0x06)  /* acknowledge */
#define NAK                     (0x15)  /* negative acknowledge */
#define CA                      (0x18)  /* two of these in succession aborts transfer */
#define CRC16                   (0x43)  /* 'C' == 0x43, request 16-bit CRC */

#define SOH_PACKET_SIZE     133
#define STX_PACKET_SIZE     1029

typedef enum
{
    EM_YPT_NONE,    //未知类型
    EM_YPT_ERR,    //错误包
    EM_YPT_HEAD,
    EM_YPT_STX_DATA,
    EM_YPT_SOH_DATA,
    EM_YPT_EOT,
    EM_YPT_BREAK,
    EM_YPT_END,
}EM_YmPackType;
//-------------------------------//

//-------------------主体实现-------------------//
//定时回调
uint16_t timeout_cnt=0;
void ym_timer_1ms_cb(void)
{
    timeout_cnt ++;
}

//Ymodem crc单字节效验
static unsigned short YModemCrc(char *pData, unsigned short sLen)
{  
   unsigned short crc16 = 0;    //
   unsigned short i = 0;
   
   while (sLen--)  //len是所要计算的长度
   {
       crc16 = crc16^(int)(*pData++) << 8; //    
       for (i=8; i!=0; i--) 
       {
           if (crc16 & 0x8000)   
               crc16 = crc16 << 1 ^ 0x1021;    
           else
               crc16 = crc16 << 1;
       }    
   }
   return crc16;
}

//初始化
void ymodem_init(Ymodem *ym)
{
    ymodem_dev_init();
    ym->status = YM_STA_INIT;
    ym->flash_addr = YM_WRITE_START_ADDR;
    timeout_cnt = 0;
}

//解析头个SOH帧里面的文件名、长度信息
void soh_parse_file(Ymodem *ym, uint8_t *buf, uint8_t len)
{
    uint8_t *pFile, *pSize;
    pFile = buf +3;
    pSize = pFile + strlen((char *)(pFile)) +1;
    strcpy(ym->file_name, (char *)pFile);
    ym->file_size = atol((char *)pSize);
}

//输入完整一包ymodem数据
//返回此包类型
EM_YmPackType ym_packet_type(uint8_t *pData, uint16_t Len, uint8_t ym_sta)
{
    EM_YmPackType ret = EM_YPT_NONE;
    uint16_t RxCrc;
    if( Len == SOH_PACKET_SIZE || Len == STX_PACKET_SIZE )
    {
        RxCrc = YModemCrc((char *)(pData+3), Len-5);
        if( ((RxCrc>>8&0xff) == pData[Len-2]) &&    //CRC正确
            ((RxCrc&0xff) == pData[Len-1]) &&   //CRC正确
            ( pData[1] == ((~pData[2])&0xff) ) )  //补码正确
        {
            //收到SOH或者STX包
            switch(ym_sta)
            {
                case YM_STA_INIT:   //如果是初始状态
                    ret = EM_YPT_HEAD;     //则此为头包
                    break;
                case YM_STA_RECVING:    //接收到头包后的状态
                    if( Len == STX_PACKET_SIZE )
                    {
                        ret = EM_YPT_STX_DATA;     //则此为1024数据包
                    }
                    else if(Len == SOH_PACKET_SIZE)
                    {
                        ret = EM_YPT_SOH_DATA;  //则此为128数据包
                    }
                    break;
                case YM_STA_DONE:   //如果是接收了EOT的状态
                    ret = EM_YPT_END;   //则此为空的结束包
                    break;
                default:
                    break;
            }
        }
        else    //校验错误
        {
            return EM_YPT_ERR;  //错误包
        }
    }
    else 
    {
        switch(pData[0])
        {
            case EOT:
                ret = EM_YPT_EOT;   //结束符
                break;
            case CA:
                ret = EM_YPT_BREAK; //用户终止
            default:
                break;
        }
    }

    return ret;
}

//可以放在ymodem_poll函数中，但是堆栈可能不够
uint8_t pData[1030];    
//主函数
int8_t ymodem_poll(Ymodem *ym)
{
    int8_t ret = -1;
    uint16_t RxLen;
    
    RxLen = ymode_dev_recv(pData, sizeof(pData));   //从底层设备获取一包数据
    if(RxLen > 0)
    {
        int type = ym_packet_type(pData, RxLen, ym->status);    //校验并解析此包类型
        switch(type)
        {
            case EM_YPT_HEAD:   //收到头包，包含文件信息的包
                ymode_dev_send_byte(ACK);
                soh_parse_file(ym, pData, RxLen);   //计算文件大小
                ym->flash_addr = YM_WRITE_START_ADDR;
                ymode_dev_ease_flash(ym->flash_addr, ym->file_size);   //擦除flash
                ym->rem_size = ym->file_size;   //待写入的数据大小
                ymode_dev_send_byte('C');
                ym->status = YM_STA_RECVING;  //进入准备接收状态
                break;
            case EM_YPT_STX_DATA:   //收到STX 1024数据包
                if(ym->rem_size>=1024)
                {
                    ymode_dev_write_flash(ym->flash_addr, pData+3, 1024);
                    ym->rem_size -= 1024;
                    ym->flash_addr += 1024;
                }
                else    //最后一包
                {
                    ymode_dev_write_flash(ym->flash_addr, pData+3, ym->rem_size);
                    ym->rem_size = 0;
                    ym->flash_addr = YM_WRITE_START_ADDR;
                }
                ymode_dev_send_byte(ACK);
                ym->status = YM_STA_RECVING;    //正在接收状态
                break;
            case EM_YPT_SOH_DATA:   //收到SOH 128数据包
                if(ym->rem_size>=128)
                {
                    ymode_dev_write_flash(ym->flash_addr, pData+3, 128);
                    ym->rem_size -= 128;
                    ym->flash_addr += 128;
                }
                else    //最后一包
                {
                    ymode_dev_write_flash(ym->flash_addr, pData+3, ym->rem_size);
                    ym->rem_size = 0;
                    ym->flash_addr = YM_WRITE_START_ADDR;
                }
                ymode_dev_send_byte(ACK);
                ym->status = YM_STA_RECVING;    //正在接收状态
                break;
            case EM_YPT_EOT:    //收到EOT
                //很多人说收到第一个EOT，先发NAK，再收到EOT才回ACK+'C'
                //实际上，收到EOT直接回复ACk+'C'接收最后一个空包即可
                ymode_dev_send_byte(ACK);
                ymode_dev_delay(10);    //需要一小段间隔时间
                ymode_dev_send_byte('C');
                ym->status = YM_STA_DONE;   //数据接收完成，收尾状态
                break;
            case EM_YPT_BREAK:  //收到用户中断指令
                ymodem_init(ym);     //恢复初始化，准备下次通信
                break;
            case EM_YPT_END:    //收到最后一个空包
                ymode_dev_send_byte(ACK);
                ymodem_init(ym);     //恢复初始化，准备下次通信
                ret = 0;    //彻底完成一次传输，返回0
                break;
            case EM_YPT_ERR:    //数据校验错误
                ymode_dev_send_byte(NAK);   //请求重发
                break;
            case EM_YPT_NONE:   //未知指令
                break;
            default:
                break;
        }
        timeout_cnt = 0;
    }
    else    //通信底层没有收到数据
    {
        if( ym->status == YM_STA_INIT ) //如果是初始状态
        {
            if( timeout_cnt > 1000) //定时发C，准备接收
            {
                ymode_dev_send_byte('C');
                timeout_cnt = 0;
            }
        }
        else    //如果是其它状态下
        {
            if( timeout_cnt > 10000) //超过10s没任何数据过来, 认为传输终止
            {
                ymodem_init(ym);     //恢复初始化，准备下次通信
                timeout_cnt = 0;
            }
        }
    }
    
    ymode_dev_recv_en();
    return ret;  
}


////-----------使用示例-----------//
//#include "rtthread.h"
//void ymodem_thread_entry(void *Par)
//{
//    uint8_t rate;
//    Ymodem ymode;
//    
//    ymodem_init(&ymode);
//    
//    while(1)
//    {
//        if( ymodem_poll(&ymode) == 0)
//        {
//            printf("\r\nYmodem receive OK!\r\n");
//            printf("file name:\"%s\"\r\n", ymode.file_name);
//            printf("file size:%dbyte\r\n", ymode.file_size);
//            bl_go_to_app();
//        }
//        else
//        {
//            if( ymode.status == YM_STA_RECVING )
//            {
//                if( rate != (uint8_t)(100 - ymode.rem_size*100.0/ymode.file_size) )
//                {
//                    rate = (uint8_t)(100 - ymode.rem_size*100.0/ymode.file_size);
//                    printf("Receive: %d%%\r\n", rate);
//                }
//            }
//        }
//        ym_timer_1ms_cb();  //有定时器可以放到定时器中
//        rt_thread_mdelay(1);
//    }
//}
