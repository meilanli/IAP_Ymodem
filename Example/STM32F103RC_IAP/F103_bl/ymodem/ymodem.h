#ifndef __YMODEM_H
#define __YMODEM_H
#include "stdint.h"

/**************************************************
*��ֲ������
    ʵ��ymodem_port.c����ӿ�
*ʹ�÷���
    1.����ymodem���յ����ļ��Ĵ����ʼ��ַYM_WRITE_START_ADDR
    2.����ṹ�����Ymodem xx
    3.����ymodem_init��ʼ��
    4.1ms��ʱ���е���ym_timer_1ms_cb
    4.ѭ������ymodem_poll
    5.ֱ��ymodem_poll����0��ʾһ��ͨ�����
***************************************************/

#define YM_STA_INIT     0
#define YM_STA_RECVING  1
#define YM_STA_DONE     2

typedef struct
{
    uint32_t flash_addr;    //�´�Ҫд��ĵ�ַ
    uint8_t status;     //��ǰͨ��״̬
    uint32_t rem_size;  //ʣ��������ļ�����
    uint32_t file_size; //�ļ���С
    char file_name[32];    //�������Ҫ�������Ż�
}Ymodem;

//��Ҫ��ʼ���ṹ��Ĳ���
void ymodem_init(Ymodem *ym);    

//�ŵ�1ms���ҵĶ�ʱ����
void ym_timer_1ms_cb(void);

//ymodem��������ѭ�����ü���
int8_t ymodem_poll(Ymodem *ym);

//ʾ������
void ymodem_thread_entry(void *Par);

#endif
