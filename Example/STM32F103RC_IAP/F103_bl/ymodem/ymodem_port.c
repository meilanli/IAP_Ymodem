
//---------------��Ҫ��ֲ�Ľӿ�,Ҳ������ͷ�ļ���ֱ�Ӷ���ɺ�-----------//
#include "ymodem_port.h"

//Ҫʹ�õĵײ�����
#include "usart.h"
#include "flash.h"

//��Ҫһ����ȫ����ȷ����ʱ
//���ڼ��������֡, ������λظ�û�м��ʱ�䣬��λ���ܿ����޷�����
//һ�㲻���޸�
void ymode_dev_delay(uint16_t ms)
{
    uint16_t temp=1000;
    while(ms--)
    {
        while(temp--);
    }
}

//����һ���ֽ�
void ymode_dev_send_byte(uint8_t ch)
{
//    usart_tx(&ch, 1);
    USART_SendData(USART1, ch);
}

/******************
*��ȡһ֡����
*���룺
*  buf���ڴ�Ŷ�ȡ��������
*  lenҪ��ȡ�ĳ���
*���أ�ʵ�ʽ��յ������ݳ���
******************/
uint32_t ymode_dev_recv(uint8_t *buf, uint16_t len)
{
    return usart_rx(1, buf, len);
}

//����ʹ��, ����Ҫ������(����RS485���ص�ͨ����Ҫ)
void ymode_dev_recv_en(void)
{
//    usart_rx_enable();
}

/********����flash*********
*���룺
*  addr    ��ʼ��ַ
*  len     ���ݳ���byte
*ע��������ҳ��
***************************/
void ymode_dev_ease_flash(uint32_t addr, uint32_t len)
{
    EraseFlash(addr, len);
}

/**********д������*******
*���룺
*  addr��д����ʼ��ַ
*  pData������
*  len��    ���ݳ���
*ע���ֽڶ���
**************************/
void ymode_dev_write_flash(uint32_t addr, uint8_t *pData, uint32_t len)
{
    WriteFlash(addr, pData, len);
}

/**********ͨ�������ʼ��*******
*�������Ҫ���������г�ʼ��
**************************/
void ymodem_dev_init(void)
{
    usart_init(1, 115200);
}
