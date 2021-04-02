
//��ӵ�app����
#include "iap.h"	//��bootloader�е�iap.h��ͬһ��
#include "string.h"
#include "flash.h"

//���������־
//��ͬλ�ò�ͬʵ�ַ�ʽ
//������app�û����ݷ���һ�𣬷����������Լ�ռ�
uint8_t data[PAGE_SIZE];
void clear_update_flag(void)
{
//    uint8_t data[PAGE_SIZE];    //���������ջ����
    //���������־��ַ����ҳ��ʼ��ַ������Ҫ�����ַ
    ReadFlash(UPDATE_FLAG_ADDRESS, data, PAGE_SIZE);
    EraseFlash(UPDATE_FLAG_ADDRESS, PAGE_SIZE);
    WriteFlash(UPDATE_FLAG_ADDRESS+4, data+4, PAGE_SIZE-4);
}

void write_update_flag(void)
{
    uint32_t flag = UPDATE_FLAG_VALUE;
    WriteFlash(UPDATE_FLAG_ADDRESS, (uint8_t *)&flag, 4);
}

//CPU��λ
void app_cpu_reset(void)
{
    SCB_AIRCR = SCB_RESET_VALUE;
}

//����mian�е�һ������
void app_iap_init(void)
{
    #if !defined (SOC_STM32F0) && !defined (SOC_STM32F1) && !defined (SOC_STM32F4)
    #error "Please select use SOC_xxx!"
    #endif
    
    #ifdef SOC_STM32F0
    /*0x20000000��SRAM����ʼ��ַ*/
    /*0x08002800��Ӧ�ó������ַ��ַ*/
    /*VECTOR_SIZE��ָ�ж�������Ĵ�С*/
    memcpy((void*)0x20000000, (void*)APP_ADDRESS, VECTOR_SIZE);
    SYSCFG_MemoryRemapConfig(SYSCFG_MemoryRemap_SRAM);
    #endif
    
    #if defined(SOC_STM32F1) || defined(SOC_STM32F4)
    SCB->VTOR = APP_ADDRESS;
    #endif
    
    __enable_irq();
    
    //���IAP������־
    clear_update_flag();
}

void app_go_to_update(void)
{
    //дIAP������־
    write_update_flag();
    app_cpu_reset();
    while(1);
}
