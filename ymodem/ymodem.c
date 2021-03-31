/***********************************************
2021-2-21 by meilanli
1.Ymodem�ļ�����Э��ʵ��, ֧��Ymodem(�ɰ�128)��Ymodem-1K
2.hypertrm�����ն�Win7��Tera Term����֤
3.Ӧ����BootLoader�У��Խ�flash��д��ʵ��IAP����
***********************************************/
#include "ymodem.h"
#include "ymodem_port.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

//----------�궨��-------------//
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
    EM_YPT_NONE,    //δ֪����
    EM_YPT_ERR,    //�����
    EM_YPT_HEAD,
    EM_YPT_STX_DATA,
    EM_YPT_SOH_DATA,
    EM_YPT_EOT,
    EM_YPT_BREAK,
    EM_YPT_END,
}EM_YmPackType;
//-------------------------------//

//-------------------����ʵ��-------------------//
//��ʱ�ص�
uint16_t timeout_cnt=0;
void ym_timer_1ms_cb(void)
{
    timeout_cnt ++;
}

//Ymodem crc���ֽ�Ч��
static unsigned short YModemCrc(char *pData, unsigned short sLen)
{  
   unsigned short crc16 = 0;    //
   unsigned short i = 0;
   
   while (sLen--)  //len����Ҫ����ĳ���
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

//��ʼ��
void ymodem_init(Ymodem *ym)
{
    ymodem_dev_init();
    ym->status = YM_STA_INIT;
    ym->flash_addr = YM_WRITE_START_ADDR;
    timeout_cnt = 0;
}

//����ͷ��SOH֡������ļ�����������Ϣ
void soh_parse_file(Ymodem *ym, uint8_t *buf, uint8_t len)
{
    uint8_t *pFile, *pSize;
    pFile = buf +3;
    pSize = pFile + strlen((char *)(pFile)) +1;
    strcpy(ym->file_name, (char *)pFile);
    ym->file_size = atol((char *)pSize);
}

//��������һ��ymodem����
//���ش˰�����
EM_YmPackType ym_packet_type(uint8_t *pData, uint16_t Len, uint8_t ym_sta)
{
    EM_YmPackType ret = EM_YPT_NONE;
    uint16_t RxCrc;
    if( Len == SOH_PACKET_SIZE || Len == STX_PACKET_SIZE )
    {
        RxCrc = YModemCrc((char *)(pData+3), Len-5);
        if( ((RxCrc>>8&0xff) == pData[Len-2]) &&    //CRC��ȷ
            ((RxCrc&0xff) == pData[Len-1]) &&   //CRC��ȷ
            ( pData[1] == ((~pData[2])&0xff) ) )  //������ȷ
        {
            //�յ�SOH����STX��
            switch(ym_sta)
            {
                case YM_STA_INIT:   //����ǳ�ʼ״̬
                    ret = EM_YPT_HEAD;     //���Ϊͷ��
                    break;
                case YM_STA_RECVING:    //���յ�ͷ�����״̬
                    if( Len == STX_PACKET_SIZE )
                    {
                        ret = EM_YPT_STX_DATA;     //���Ϊ1024���ݰ�
                    }
                    else if(Len == SOH_PACKET_SIZE)
                    {
                        ret = EM_YPT_SOH_DATA;  //���Ϊ128���ݰ�
                    }
                    break;
                case YM_STA_DONE:   //����ǽ�����EOT��״̬
                    ret = EM_YPT_END;   //���Ϊ�յĽ�����
                    break;
                default:
                    break;
            }
        }
        else    //У�����
        {
            return EM_YPT_ERR;  //�����
        }
    }
    else 
    {
        switch(pData[0])
        {
            case EOT:
                ret = EM_YPT_EOT;   //������
                break;
            case CA:
                ret = EM_YPT_BREAK; //�û���ֹ
            default:
                break;
        }
    }

    return ret;
}

//���Է���ymodem_poll�����У����Ƕ�ջ���ܲ���
uint8_t pData[1030];    
//������
int8_t ymodem_poll(Ymodem *ym)
{
    int8_t ret = -1;
    uint16_t RxLen;
    
    RxLen = ymode_dev_recv(pData, sizeof(pData));   //�ӵײ��豸��ȡһ������
    if(RxLen > 0)
    {
        int type = ym_packet_type(pData, RxLen, ym->status);    //У�鲢�����˰�����
        switch(type)
        {
            case EM_YPT_HEAD:   //�յ�ͷ���������ļ���Ϣ�İ�
                ymode_dev_send_byte(ACK);
                soh_parse_file(ym, pData, RxLen);   //�����ļ���С
                ym->flash_addr = YM_WRITE_START_ADDR;
                ymode_dev_ease_flash(ym->flash_addr, ym->file_size);   //����flash
                ym->rem_size = ym->file_size;   //��д������ݴ�С
                ymode_dev_send_byte('C');
                ym->status = YM_STA_RECVING;  //����׼������״̬
                break;
            case EM_YPT_STX_DATA:   //�յ�STX 1024���ݰ�
                if(ym->rem_size>=1024)
                {
                    ymode_dev_write_flash(ym->flash_addr, pData+3, 1024);
                    ym->rem_size -= 1024;
                    ym->flash_addr += 1024;
                }
                else    //���һ��
                {
                    ymode_dev_write_flash(ym->flash_addr, pData+3, ym->rem_size);
                    ym->rem_size = 0;
                    ym->flash_addr = YM_WRITE_START_ADDR;
                }
                ymode_dev_send_byte(ACK);
                ym->status = YM_STA_RECVING;    //���ڽ���״̬
                break;
            case EM_YPT_SOH_DATA:   //�յ�SOH 128���ݰ�
                if(ym->rem_size>=128)
                {
                    ymode_dev_write_flash(ym->flash_addr, pData+3, 128);
                    ym->rem_size -= 128;
                    ym->flash_addr += 128;
                }
                else    //���һ��
                {
                    ymode_dev_write_flash(ym->flash_addr, pData+3, ym->rem_size);
                    ym->rem_size = 0;
                    ym->flash_addr = YM_WRITE_START_ADDR;
                }
                ymode_dev_send_byte(ACK);
                ym->status = YM_STA_RECVING;    //���ڽ���״̬
                break;
            case EM_YPT_EOT:    //�յ�EOT
                //�ܶ���˵�յ���һ��EOT���ȷ�NAK�����յ�EOT�Ż�ACK+'C'
                //ʵ���ϣ��յ�EOTֱ�ӻظ�ACk+'C'�������һ���հ�����
                ymode_dev_send_byte(ACK);
                ymode_dev_delay(10);    //��ҪһС�μ��ʱ��
                ymode_dev_send_byte('C');
                ym->status = YM_STA_DONE;   //���ݽ�����ɣ���β״̬
                break;
            case EM_YPT_BREAK:  //�յ��û��ж�ָ��
                ymodem_init(ym);     //�ָ���ʼ����׼���´�ͨ��
                break;
            case EM_YPT_END:    //�յ����һ���հ�
                ymode_dev_send_byte(ACK);
                ymodem_init(ym);     //�ָ���ʼ����׼���´�ͨ��
                ret = 0;    //�������һ�δ��䣬����0
                break;
            case EM_YPT_ERR:    //����У�����
                ymode_dev_send_byte(NAK);   //�����ط�
                break;
            case EM_YPT_NONE:   //δָ֪��
                break;
            default:
                break;
        }
        timeout_cnt = 0;
    }
    else    //ͨ�ŵײ�û���յ�����
    {
        if( ym->status == YM_STA_INIT ) //����ǳ�ʼ״̬
        {
            if( timeout_cnt > 1000) //��ʱ��C��׼������
            {
                ymode_dev_send_byte('C');
                timeout_cnt = 0;
            }
        }
        else    //���������״̬��
        {
            if( timeout_cnt > 10000) //����10sû�κ����ݹ���, ��Ϊ������ֹ
            {
                ymodem_init(ym);     //�ָ���ʼ����׼���´�ͨ��
                timeout_cnt = 0;
            }
        }
    }
    
    ymode_dev_recv_en();
    return ret;  
}


////-----------ʹ��ʾ��-----------//
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
//        ym_timer_1ms_cb();  //�ж�ʱ�����Էŵ���ʱ����
//        rt_thread_mdelay(1);
//    }
//}
