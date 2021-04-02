
//添加到app工程
#include "iap.h"	//与bootloader中的iap.h是同一个
#include "string.h"
#include "flash.h"

//清除升级标志
//不同位置不同实现方式
//建议与app用户数据放在一起，方便操作，节约空间
uint8_t data[PAGE_SIZE];
void clear_update_flag(void)
{
//    uint8_t data[PAGE_SIZE];    //放在这里堆栈不够
    //如果升级标志地址不是页起始地址，则需要推算地址
    ReadFlash(UPDATE_FLAG_ADDRESS, data, PAGE_SIZE);
    EraseFlash(UPDATE_FLAG_ADDRESS, PAGE_SIZE);
    WriteFlash(UPDATE_FLAG_ADDRESS+4, data+4, PAGE_SIZE-4);
}

void write_update_flag(void)
{
    uint32_t flag = UPDATE_FLAG_VALUE;
    WriteFlash(UPDATE_FLAG_ADDRESS, (uint8_t *)&flag, 4);
}

//CPU软复位
void app_cpu_reset(void)
{
    SCB_AIRCR = SCB_RESET_VALUE;
}

//必须mian中第一个调用
void app_iap_init(void)
{
    #if !defined (SOC_STM32F0) && !defined (SOC_STM32F1) && !defined (SOC_STM32F4)
    #error "Please select use SOC_xxx!"
    #endif
    
    #ifdef SOC_STM32F0
    /*0x20000000是SRAM的起始地址*/
    /*0x08002800是应用程序的起址地址*/
    /*VECTOR_SIZE是指中断向量表的大小*/
    memcpy((void*)0x20000000, (void*)APP_ADDRESS, VECTOR_SIZE);
    SYSCFG_MemoryRemapConfig(SYSCFG_MemoryRemap_SRAM);
    #endif
    
    #if defined(SOC_STM32F1) || defined(SOC_STM32F4)
    SCB->VTOR = APP_ADDRESS;
    #endif
    
    __enable_irq();
    
    //清除IAP升级标志
    clear_update_flag();
}

void app_go_to_update(void)
{
    //写IAP升级标志
    write_update_flag();
    app_cpu_reset();
    while(1);
}
