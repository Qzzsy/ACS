/**
 ******************************************************************************
 * @file        delay.c
 * @author      �Ž�����С��
 * @version     V1.0.4
 * @date        2018-01-11
 * @brief       ����ļ���ϵͳ��ʱʵ�ֵķ�����ʵ�ֵ���ʱ��us��ʱ��ms��ʱ������systick
                �ķ�ʽ����ʱ��֧�������uc/OS-III��uc/OS-II��rt-threadʵʱϵͳ		
 * @note        ʹ��ǰ��Ҫ����delay_init();  �����޷�ʹ�ô˷���
 * @History
 * Date           	Author    	version    				Notes
 * 2017-11-05     	ZSY       	V1.0.0              first version.
 * 2017-11-07     	ZSY       	V1.0.1              �޸��ڲ���ϵͳ��us��ʱ��׼��BUG����߼�����
 * 2017-11-13       ZSY         V1.0.2              �޸�������������us����ʱ�����BUG�������ṹ���޸���uc/OS��ʹ��ʱms����ʱ��BUG
 * 2018-01-11     	ZSY       	V1.0.3              �Ű��ʽ����������ǿ�ɶ��ԣ���Ӷ�HAL���֧��
 * 2018-01-11       ZSY         V1.0.4              �޸Ĳ��ֱ���������
 */
	
/* Includes ------------------------------------------------------------------*/
#include "delay.h"

/* ���֧��ϵͳ */
#ifdef SYSTEM_SUPPORT_OS

/* ���ʹ��uc/OS������������ͷ�ļ� */
#if defined (OS_CRITICAL_METHOD) || defined (CPU_CFG_CRITICAL_METHOD)
#include "includes.h"

/* ���ʹ��rt-thread������������ͷ�ļ� */
#elif defined (OS_USE_RTTHREAD)	
#include "rtthread.h"					
#endif	
#endif /* SYSTEM_SUPPORT_OS */

/* global variable Declaration -----------------------------------------------*/

/* ��ʱ������Ҫ��������ֵ����װ�ڽṹ��delay_Typedef�� */
/* fac_us	ÿһ��us�������ʱ���� */
/* fac_ms	ÿһ��ms���������ʱ���� */
/* �ýṹ�嶨���ȫ�ֱ���������delay.c����ʹ�� */
delay_Typedef sys_delay;

/* User function Declaration --------------------------------------------------*/


/* User functions -------------------------------------------------------------*/

/* ���SYSTEM_SUPPORT_OS�����ˣ�˵��Ҫ֧��OS */
#ifdef SYSTEM_SUPPORT_OS							

/**
 * @func    delay_osschedlock
 * @brief   us����ʱʱ���ر��������(��ֹϵͳ���ȴ��us���ӳ�)
 * @note    ��֧��ϵͳʹ��
 * @retval  ��
 */
void delay_osschedlock(void)
{
/* ʹ��UCOSIII */
#if defined (CPU_CFG_CRITICAL_METHOD)    			
    OS_ERR err; 
    
    /* UCOSIII�ķ�ʽ����ֹ���ȣ���ֹ���us��ʱ */
    OSSchedLock(&err);
    
/* ʹ��UCOSII */
#elif defined (OS_CRITICAL_METHOD)		

    /* UCOSII�ķ�ʽ����ֹ���� */
    OSSchedLock();
    
/* ʹ��rt-thread */	
#elif defined (OS_USE_RTTHREAD)
	
    /* rt-thread�ķ�ʽ����ֹ���� */
    rt_enter_critical();
#endif
}

/**
 * @func    delay_osschedunlock
 * @brief   us����ʱʱ,�ָ��������
 * @note    ��֧��ϵͳʹ��
 * @retval  ��
 */
void delay_osschedunlock(void)
{	
#if defined (CPU_CFG_CRITICAL_METHOD)    			
    OS_ERR err; 
	
    /* UCOSIII�ķ�ʽ,�ָ����� */
    OSSchedUnlock(&err);	
	
#elif defined (OS_CRITICAL_METHOD)
	
    /* UCOSII�ķ�ʽ,�ָ����� */
    OSSchedUnlock();	
	
#elif defined (OS_USE_RTTHREAD)
	
    /* rt-thread�ķ�ʽ,�ָ����� */
    rt_exit_critical();
#endif
}

/**
 * @func    delay_ostimedly
 * @brief   ����OS�Դ�����ʱ������ʱ
 * @param   ticks ��ʱ�Ľ�����
 * @note    ��֧��ϵͳʹ��
 * @retval  ��
 */
void delay_ostimedly(uint32_t ticks)
{
#if defined (CPU_CFG_CRITICAL_METHOD) 
    OS_ERR err; 
	
    /* UCOSIII��ʱ��������ģʽ */
    OSTimeDly(ticks,OS_OPT_TIME_PERIODIC, &err);
	
#elif defined (OS_CRITICAL_METHOD)
	
    /* UCOSII��ʱ */
    OSTimeDly(ticks);		
	
#elif defined (OS_USE_RTTHREAD)
	
    /* rt-thread��ʱ */
    rt_thread_delay(ticks);
#endif 
}
/* ��Ϊrt-thread�����Ѿ�������SysTick_Handler�����Խ���uc/OS��Ҫ�õ��˺��� */
#if defined (OS_CRITICAL_METHOD) || defined (CPU_CFG_CRITICAL_METHOD)

/**
 * @func    SysTick_Handler
 * @brief   systick�жϷ�����,ʹ��OSʱ�õ�
 * @note    ��ǰ��֧��uc/OSϵͳʹ��
 * @retval  ��
 */
void SysTick_Handler(void)
{	
#ifdef USE_HAL_DRIVER
    HAL_IncTick();
#endif
    /* OS��ʼ����,��ִ�������ĵ��ȴ��� */
    if (DELAY_OS_RUNNING == 1)					
    {
        /* �����ж� */
        OSIntEnter();						
        
        /* ����ucos��ʱ�ӷ������ */
        OSTimeTick();      
        
        /* ���������л����ж� */    
        OSIntExit();       	 				
    }
}
#endif
#endif /* SYSTEM_SUPPORT_OS */

/**
 * @func    delay_init
 * @brief   ��ʼ���ӳٺ���
 * @note    ��ǰ��֧��uc/OSϵͳʹ��
            ��ʹ��uc/OS��ʱ��,�˺������ʼ��uc/OS��ʱ�ӽ���
            SYSTICK��ʱ�ӹ̶�ΪAHBʱ�ӵ�1/8
 * @retval  ��
 */			   
void delay_init(void)
{
    uint32_t sys_tick_freq;
	
	/* ����RCC_ClocksTypeDef���͵ı��������ڻ�ȡϵͳ��ʱ�� */
#if defined (USE_HAL_DRIVER) 
    uint32_t hclk_frequency;
    hclk_frequency = HAL_RCC_GetHCLKFreq();
#elif defined (USE_STDPERIPH_DRIVER)
    RCC_ClocksTypeDef  rcc_clocks;
    
    /* ��ȡоƬ��ǰ�����е�ʱ�� */
    RCC_GetClocksFreq(&rcc_clocks);
#endif
	
/* ��Ҫ֧��OS. */
#ifdef SYSTEM_SUPPORT_OS 

/* ʹ��uc/OS */	
#if defined (OS_CRITICAL_METHOD) || defined (CPU_CFG_CRITICAL_METHOD)
    uint32_t cnts;
#if defined (USE_HAL_DRIVER)
    cnts = (uint32_t)hclk_frequency / DELAY_OS_TICK_PERSEC;
#elif defined (USE_STDPERIPH_DRIVER)
    cnts = (uint32_t)rcc_clocks.HCLK_Frequency / DELAY_OS_TICK_PERSEC;
#endif

    /* Ĭ��ʹ��8��Ƶ */
    cnts = cnts / 8;
    
    /* systick��ʱ������ֵ������ */
    SysTick_Config(cnts);
#endif
#endif /* SYSTEM_SUPPORT_OS */
	
/* ��ʹ��rt-thread */
#ifndef OS_USE_RTTHREAD

    /* ����systick��ʱ��Դ������ϵͳʱ�ӵ�8��Ƶ */
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); 
#endif /* OS_USE_RTTHREAD */
    
    if (((SysTick->CTRL) >> 2) & 0x01)
    {
#if defined (USE_HAL_DRIVER)
        
        /* ��ȡsystick��ʱ�ӣ�SysTickֱ��ȡHCLK����Ƶ */
        sys_tick_freq = (uint32_t)hclk_frequency;
#elif defined (USE_STDPERIPH_DRIVER)
        
        /* ��ȡsystick��ʱ�ӣ�SysTickֱ��ȡHCLK����Ƶ */
        sys_tick_freq = (uint32_t)rcc_clocks.HCLK_Frequency;
#endif
    }
    else
    {
#if defined (USE_HAL_DRIVER)
        
        /* ��ȡsystick��ʱ�ӣ�Ĭ����ϵͳʱ�ӵ�8��Ƶ */
        sys_tick_freq = (uint32_t)hclk_frequency / 8;
#elif defined (USE_STDPERIPH_DRIVER)
        
        /* ��ȡsystick��ʱ�ӣ�Ĭ����ϵͳʱ�ӵ�8��Ƶ */
        sys_tick_freq = (uint32_t)rcc_clocks.HCLK_Frequency / 8;
#endif
    }
    
    /* ����ÿ��us������ʱ���� */
    sys_delay.fac_us = sys_tick_freq / 1000000;
    
#ifdef SYSTEM_SUPPORT_OS
#if defined (OS_USE_RTTHREAD) || defined (OS_CRITICAL_METHOD) || defined (CPU_CFG_CRITICAL_METHOD)
    sys_delay.fac_ms = 1000 / DELAY_OS_TICK_PERSEC;
#endif
#else

    /* ��OS��,����ÿ��ms��Ҫ��systickʱ���� */
    sys_delay.fac_ms = (uint16_t)sys_delay.fac_us * 1000;				
#endif /* SYSTEM_SUPPORT_OS */
}		

/* �����Ҫ֧��OS. */
#if SYSTEM_SUPPORT_OS 

/**
 * @func    delay_us
 * @brief   ��ʱnus
 * @param   nus Ҫ��ʱ��us��	
 * @note    nus�ķ�Χ0~204522252(���ֵ��2^32/fac_us@fac_us=21)
 * @retval  ��
 */	  								   
void delay_us(uint32_t nus)
{		
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload;
    
    /* ��ȡ��ǰLOAD��ֵ */
    reload = SysTick->LOAD;				
    
    /* ������Ҫ�Ľ����� */
    ticks = nus * sys_delay.fac_us; 	
    
    /* ��ֹOS���ȣ���ֹ���us��ʱ */
    delay_osschedlock();	
    
    /* ��¼��ǰ�ļ�������ֵ */
    told = SysTick->VAL;    
    
    /* ���� ��ʱ */
    while (1)
    {
        tnow = SysTick->VAL;	
        
        if (tnow != told)
        {	    
            /* ����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ����� */
            if (tnow < told)
                tcnt += told - tnow;	
            else 
                tcnt += reload - tnow + told;	    
            told = tnow;
            
            /* ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳� */
            if (tcnt >= ticks)
                break;			
        }  
    };
    
    /* �ָ�OS���� */			
    delay_osschedunlock();												    
}  

/**
 * @func    delay_ms
 * @brief   ��ʱnms
 * @param   nms Ҫ��ʱ��ms��	
 * @note    nms��0-65535
 * @retval  ��
 */	
void delay_ms(uint16_t nms)
{	
    uint16_t i;
#if defined (OS_CRITICAL_METHOD) || defined (CPU_CFG_CRITICAL_METHOD)
    
    /* ���OS�Ѿ�������,���Ҳ������ж�����(�ж����治���������) */
    if (DELAY_OS_RUNNING && (DELAY_OS_INTNESTING == 0))  
    {	
        /* ��ʱ��ʱ�����OS������ʱ������ */
        if (nms >= sys_delay.fac_ms)						
        { 
            /* OS��ʱ */
            delay_ostimedly(nms / sys_delay.fac_ms);	
        }
        
        /* OS�Ѿ��޷��ṩ��ôС����ʱ��,������ͨ��ʽ��ʱ */
        nms %= sys_delay.fac_ms;						 
    }
    else
    {
        for (i = 0; i < nms; i++)
        {
            /* ��ͨ��ʽ��ʱ */
            delay_us(1000);	
        }
        return ;
    }
    
/* ���ʹ��rt-thread(�ж����治���������) */
#elif defined (OS_USE_RTTHREAD)
    {
        static rt_uint8_t delay_ms_lock = False;
        if((DELAY_OS_RUNNING | delay_ms_lock) && (DELAY_OS_INTNESTING == 0))
        {
            
            /* ���ڱ�־ϵͳ�Ƿ��Ѿ������� */
            if (delay_ms_lock == False)
            {
                delay_ms_lock = True;
            }
            
            if (nms >= sys_delay.fac_ms)						
            { 
                delay_ostimedly(nms / sys_delay.fac_ms);	
            }
            
            nms %= sys_delay.fac_ms;	
        }	
        else
        {
            for (i = 0; i < nms; i++)
            {
                /* ��ͨ��ʽ��ʱ */
                delay_us(1000);	
            }
            return ;
        }	
    }
#endif
    
    /* ��ͨ��ʽ��ʱ */
    delay_us((uint32_t)(nms * 1000));				
}

/* ����ucosʱ */
#else  

/**
 * @func    delay_us
 * @brief   ��ʱnus
 * @param   nus ΪҪ��ʱ��us��.	
 * @note    nus��ֵ,��Ҫ����798915us(���ֵ��2^24/fac_us@fac_us=21)
 * @retval  ��
 */	
void delay_us(uint32_t nus)
{		
    uint32_t temp;	    	 
    
    /* ʱ����� */
    SysTick->LOAD = nus * sys_delay.fac_us; 		
    
    /* ��ռ����� */
    SysTick->VAL = 0x00;        				
    
    /* ��ʼ���� */
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ; 	 
    
    do
    {
        temp = SysTick->CTRL;
    }while((temp & 0x01 ) && !(temp & (1 << 16)));	//�ȴ�ʱ�䵽��   
    
    /* �رռ����� */
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; 
    
    /* ��ռ����� */
    SysTick->VAL = 0X00;       				
}

/**
 * @func    delay_xms
 * @brief   ��ʱnms
 * @param   nms ΪҪ��ʱ��ms��.	
 * @note    ע��nms�ķ�Χ
            SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
            nms<=0xffffff*8*1000/SYSCLK
            SYSCLK��λΪHz,nms��λΪms
            ��168M������,nms<=798ms 
 * @retval  ��
 */	
void delay_xms(uint16_t nms)
{	 		  	  
    uint32_t temp;		   
    
    /* ʱ�����(SysTick->LOADΪ24bit) */
    SysTick->LOAD = (uint32_t)nms * sys_delay.fac_ms;		
    
    /* ��ռ����� */
    SysTick->VAL = 0x00;           
    
    /* ��ʼ���� */
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ;    
    
    do
    {
        temp = SysTick->CTRL;
    }while((temp & 0x01) && !(temp & (1 << 16)));	//�ȴ�ʱ�䵽��   
    
    /* �رռ����� */
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;     
    
    /* ��ռ����� */
    SysTick->VAL = 0X00;     		  		  	    
} 

/**
 * @func    delay_ms
 * @brief   ��ʱnms
 * @param   nms ΪҪ��ʱ��ms��.	
 * @note    nms:0~65535
 * @retval  ��
 */	
void delay_ms(uint16_t nms)
{	
    /* ������540,�ǿ��ǵ�ĳЩ�ͻ����ܳ�Ƶʹ�� */
    uint16_t repeat = nms / 540;	
    
    /* ���糬Ƶ��248M��ʱ��,delay_xms���ֻ����ʱ541ms���� */									
    uint16_t remain = nms % 540;
    
    while (repeat)
    {
        delay_xms(540);
        repeat--;
    }
    
    if (remain)
        delay_xms(remain);
} 
#endif
			 



































