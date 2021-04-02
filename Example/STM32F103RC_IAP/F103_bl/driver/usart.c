//=============================================================================
//�ļ����ƣ�usart1.c
//���ܸ�Ҫ������1�����ļ�
//����ʱ�䣺2014-01-04
//=============================================================================
#include "stm32f10x.h"
#include "usart.h"
#include "stdio.h"

void usart1_Init(uint32_t baud);
void usart3_Init(uint32_t baud);
void Usart_DMA2_TX_EN(uint8_t *buffer , uint16_t BufferSize);
void Usart_DMA3_RX_EN(void);
void Usart_DMA4_TX_EN(uint8_t *buffer , uint16_t BufferSize);
void Usart_DMA5_RX_EN(void);

struct UsartType
{
  uint8_t RxBuffer[FIFO_NUM];
  uint32_t RxLen;
//  uint8_t dma_fifo[FIFO_NUM];
  uint8_t RxFlag;
};

static struct UsartType usart[2];

uint32_t usart_rx(uint8_t port, uint8_t *rx_buf, uint32_t rx_len)
{
  uint32_t len=0;
  uint32_t i;
  if(port)
  {
      port--;
      if(usart[port].RxFlag)
      {
        len = rx_len < usart[port].RxLen? rx_len:usart[port].RxLen;
        for(i=0; i<len; i++)
        {
          rx_buf[i] = usart[port].RxBuffer[i];
          usart[port].RxBuffer[i] = 0;
        }
        usart[port].RxFlag=0;
      }
  }
  return len;
}

//uint16_t tx1_last_len=0;
//uint16_t tx2_last_len=0;
void usart_tx(uint8_t port, uint8_t *tx_buf, uint16_t len)
{
    uint16_t i;
    switch(port)
    {
        case 2:
            for(i=0; i<len; i++)
            {
                USART_SendData(USART3, tx_buf[i]);
                while(RESET == USART_GetFlagStatus(USART3, USART_FLAG_TXE));
            }
            break;
        case 1:
            for(i=0; i<len; i++)
            {
                USART_SendData(USART1, tx_buf[i]);
                while(RESET == USART_GetFlagStatus(USART1, USART_FLAG_TXE));
            }
            break;
        default:break;
    }

}

void usart_rx_enable(uint8_t port)
{
    switch(port)
    {
        case 2:
            Usart_DMA3_RX_EN();
            break;
        case 1:
            Usart_DMA5_RX_EN();
            break;
        default:break;
    }
}

void usart_init(uint8_t port, uint32_t baud)
{
    switch(port)
    {
        case 1:
            usart1_Init(baud);
            usart[0].RxFlag=0;
            usart[0].RxLen = 0;
            break;
        case 2:
            usart3_Init(baud);
            usart[1].RxFlag=0;
            usart[1].RxLen = 0;
            break;
        default:break;
    }
}

void USART1_IRQHandler(void)						//����1����RS485�Ĵ���ͽ����ж�
{
    uint16_t CurrDataCount;
	if(USART_GetITStatus(USART1,USART_IT_IDLE)!=RESET)
	{
        DMA_Cmd(DMA1_Channel5,DISABLE);   //�ر�DMA���գ���ֹ��仹�����ݹ���
        USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);  //�ٴο��ж�
		USART_ReceiveData(USART1);
		USART_ClearFlag(USART1, USART_FLAG_IDLE);
		USART_ClearITPendingBit(USART1, USART_IT_IDLE);
        CurrDataCount = DMA_GetCurrDataCounter(DMA1_Channel5);
		usart[0].RxLen=FIFO_NUM - CurrDataCount;
		if(usart[0].RxLen>0)
		{
//            printf("USART1 RX=%d\n", usart[0].RxLen);
//			//��ȡ���ڻ�����������
//          uint8_t i;
//			for(i=0;i<usart[0].RxLen;i++)
//			{
//				usart[0].RxBuffer[i]=usart[0].dma_fifo[i];
//			}
			usart[0].RxFlag=1;
		}
        DMA_SetCurrDataCounter(DMA1_Channel5,FIFO_NUM);
		DMA_Cmd(DMA1_Channel5,ENABLE);  //��DMA����
        USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);  //�ٴο��ж�
	}
}

void USART3_IRQHandler(void)						//����1����RS485�Ĵ���ͽ����ж�
{
	if(USART_GetITStatus(USART3,USART_IT_IDLE)!=RESET)
	{
		USART_ReceiveData(USART3);
		USART_ClearFlag(USART3, USART_FLAG_IDLE);
		USART_ClearITPendingBit(USART3, USART_IT_IDLE);
		usart[1].RxLen=FIFO_NUM-DMA_GetCurrDataCounter(DMA1_Channel3);
        
		if(usart[1].RxLen>0)
		{
            
//			//��ȡ���ڻ�����������
//          uint8_t i;
//			for(i=0;i<usart[1].RxLen;i++)
//			{
//				usart[1].RxBuffer[i]=usart[1].dma_fifo[i];
//			}
			usart[1].RxFlag=1;
		}
	}
}

DMA_InitTypeDef DMA2_InitStructure;
DMA_InitTypeDef DMA3_InitStructure;
/* USART��ʼ�� */
void usart3_Init(uint32_t baud)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
					
    
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE );      
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE );      

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);      

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;                 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);    
	
    GPIO_PinRemapConfig(GPIO_PartialRemap_USART3 , ENABLE); 
    

		USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//
	USART_InitStructure.USART_Parity = USART_Parity_No;//
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//
	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure); //
	
	//ʹ��DMA����
	USART_DMACmd(USART3,USART_DMAReq_Rx|USART_DMAReq_Tx,ENABLE);
	USART_ITConfig(USART3,USART_IT_TC,DISABLE);         
    USART_ITConfig(USART3,USART_IT_RXNE,DISABLE);
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);//���տ����ж�ʹ��
	USART_Cmd(USART3, ENABLE);//ʹ�ܴ���

	/* �����ж� */
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;                  //IRQͨ��:����1
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority  = 1;          //���ȼ� :1��
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                    //ʹ��IRQͨ��
  NVIC_Init(&NVIC_InitStructure);


  DMA2_InitStructure.DMA_PeripheralBaseAddr =(uint32_t)(&(USART3->DR));         //�����ַ
  DMA2_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                 //���䷽��:�ڴ� -> ����
  DMA2_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;   //�������:NO
  DMA2_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;            //�ڴ����:YES
  DMA2_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA2_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;    //���ݿ��:Byte
  DMA2_InitStructure.DMA_Mode = DMA_Mode_Normal;                      //ѭ��ģʽ:NO
  DMA2_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;            //���ȼ�:��
  DMA2_InitStructure.DMA_M2M = DMA_M2M_Disable;                       //�ڴ�-�ڴ�:NO
  
  DMA3_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART3->DR));         //�����ַ
  DMA3_InitStructure.DMA_MemoryBaseAddr = (uint32_t)usart[1].RxBuffer;                 //�ڴ��ַ
  DMA3_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                 //���䷽��:���� -> �ڴ�
  DMA3_InitStructure.DMA_BufferSize = FIFO_NUM;                     //���䳤��
  DMA3_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;   //�������:NO
  DMA3_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;            //�ڴ����:YES
  DMA3_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA3_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;    //���ݿ��:Byte
  DMA3_InitStructure.DMA_Mode = DMA_Mode_Normal;                      //ѭ��ģʽ:NO
  DMA3_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;            //���ȼ�:��
  DMA3_InitStructure.DMA_M2M = DMA_M2M_Disable;                       //�ڴ�-�ڴ�:NO
}

void Usart_DMA2_TX_EN(uint8_t *buffer , uint16_t BufferSize)
{
  DMA_DeInit(DMA1_Channel2);                                         //��λDMA1_Channel2
  DMA2_InitStructure.DMA_MemoryBaseAddr = (uint32_t)buffer;                 //�ڴ��ַ
  DMA2_InitStructure.DMA_BufferSize = BufferSize;                     //���䳤��
  DMA_Init(DMA1_Channel2, &DMA2_InitStructure);
  DMA_Cmd(DMA1_Channel2, ENABLE);                                    //ʹ��DMA(��ʼ����)
}

void Usart_DMA3_RX_EN(void)
{
  DMA_DeInit(DMA1_Channel3);                                         //��λDMA1_Channel5
  DMA_Init(DMA1_Channel3, &DMA3_InitStructure);
  DMA_Cmd(DMA1_Channel3, ENABLE);                                    //ʹ��DMA(��ʼ����)
}


DMA_InitTypeDef DMA4_InitStructure;
DMA_InitTypeDef DMA5_InitStructure;
/* USART��ʼ�� */
void usart1_Init(uint32_t baud)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
					
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE );
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE );       

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;                 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);    

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	

		USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//
	USART_InitStructure.USART_Parity = USART_Parity_No;//
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//
	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure); //
	
	//ʹ��DMA����
	USART_DMACmd(USART1,USART_DMAReq_Rx|USART_DMAReq_Tx,ENABLE);
	USART_ITConfig(USART1,USART_IT_TC,DISABLE);         
    USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);
	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);//���տ����ж�ʹ��
	USART_Cmd(USART1, ENABLE);//ʹ�ܴ���

    USART_ClearFlag(USART1, USART_FLAG_TC);
    
	/* �����ж� */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;                  //IRQͨ��:����1
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority  = 1;          //���ȼ� :1��
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                    //ʹ��IRQͨ��
  NVIC_Init(&NVIC_InitStructure);
    
  //DMA TX
  DMA4_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART1->DR));         //�����ַ
  DMA4_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                 //���䷽��:�ڴ� -> ����
  DMA4_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;   //�������:NO
  DMA4_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;            //�ڴ����:YES
  DMA4_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA4_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;    //���ݿ��:Byte
  DMA4_InitStructure.DMA_Mode = DMA_Mode_Normal;                      //ѭ��ģʽ:NO
  DMA4_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;            //���ȼ�:��
  DMA4_InitStructure.DMA_M2M = DMA_M2M_Disable;                       //�ڴ�-�ڴ�:NO
  //DMA RX
  DMA5_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART1->DR));         //�����ַ
  DMA5_InitStructure.DMA_MemoryBaseAddr = (uint32_t)(usart[0].RxBuffer);                 //�ڴ��ַ
  DMA5_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                 //���䷽��:���� -> �ڴ�
  DMA5_InitStructure.DMA_BufferSize = FIFO_NUM;                     //���䳤��
  DMA5_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;   //�������:NO
  DMA5_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;            //�ڴ����:YES
  DMA5_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA5_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;    //���ݿ��:Byte
  DMA5_InitStructure.DMA_Mode = DMA_Mode_Circular;                      //ѭ��ģʽ:NO
  DMA5_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;            //���ȼ�:��
  DMA5_InitStructure.DMA_M2M = DMA_M2M_Disable;                       //�ڴ�-�ڴ�:NO
  
  DMA_Init(DMA1_Channel5, &DMA5_InitStructure);
  DMA_Cmd(DMA1_Channel5, ENABLE);                                    //ʹ��DMA(��ʼ����)
  
}

void Usart_DMA4_TX_EN(uint8_t *buffer , uint16_t BufferSize)
{
  DMA_DeInit(DMA1_Channel4);                                         //��λDMA1_Channel4
  DMA4_InitStructure.DMA_MemoryBaseAddr = (uint32_t)buffer;                 //�ڴ��ַ
  DMA4_InitStructure.DMA_BufferSize = BufferSize;                     //���䳤��
  DMA_Init(DMA1_Channel4, &DMA4_InitStructure);
  DMA_Cmd(DMA1_Channel4, ENABLE);                                    //ʹ��DMA(��ʼ����)
}

void Usart_DMA5_RX_EN(void)
{
  DMA_Cmd(DMA1_Channel5, DISABLE);
//  USART_ClearFlag(USART1, USART_FLAG_IDLE);
//  DMA_SetCurrDataCounter(DMA1_Channel5,FIFO_NUM);
  DMA_DeInit(DMA1_Channel5);                                         //��λDMA1_Channel5
  DMA_Init(DMA1_Channel5, &DMA5_InitStructure);
  DMA_Cmd(DMA1_Channel5, ENABLE);                                    //ʹ��DMA(��ʼ����)
}

#include <stdio.h>
/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    USART_SendData(USART3, (uint8_t) ch);
    while(RESET == USART_GetFlagStatus(USART3, USART_FLAG_TXE));
    return ch;
}
