/**
 ******************************************************************************
 * @file        delay.c
 * @author      门禁开发小组
 * @version     V1.0.4
 * @date        2018-01-11
 * @brief       这个文件是系统延时实现的方法，实现的延时有us延时和ms延时，采用systick
                的方式来延时，支持裸机、uc/OS-III、uc/OS-II和rt-thread实时系统		
 * @note        使用前需要调用delay_init();  否则无法使用此方法
 * @History
 * Date           	Author    	version    				Notes
 * 2017-11-05     	ZSY       	V1.0.0              first version.
 * 2017-11-07     	ZSY       	V1.0.1              修复在部分系统上us延时不准的BUG，提高兼容性
 * 2017-11-13       ZSY         V1.0.2              修复在裸机的情况下us级延时错误的BUG，调整结构，修复在uc/OS中使用时ms级延时的BUG
 * 2018-01-11     	ZSY       	V1.0.3              排版格式化操作，增强可读性，添加对HAL库的支持
 * 2018-01-11       ZSY         V1.0.4              修改部分变量的声明
 */
	
/* Includes ------------------------------------------------------------------*/
#include "delay.h"

/* 如果支持系统 */
#ifdef SYSTEM_SUPPORT_OS

/* 如果使用uc/OS，则包括下面的头文件 */
#if defined (OS_CRITICAL_METHOD) || defined (CPU_CFG_CRITICAL_METHOD)
#include "includes.h"

/* 如果使用rt-thread，则包括下面的头文件 */
#elif defined (OS_USE_RTTHREAD)	
#include "rtthread.h"					
#endif	
#endif /* SYSTEM_SUPPORT_OS */

/* global variable Declaration -----------------------------------------------*/

/* 延时方法需要的两个数值，封装在结构体delay_Typedef内 */
/* fac_us	每一个us里包含的时钟数 */
/* fac_ms	每一个ms里面包含的时钟数 */
/* 该结构体定义的全局变量仅限于delay.c里面使用 */
delay_Typedef sys_delay;

/* User function Declaration --------------------------------------------------*/


/* User functions -------------------------------------------------------------*/

/* 如果SYSTEM_SUPPORT_OS定义了，说明要支持OS */
#ifdef SYSTEM_SUPPORT_OS							

/**
 * @func    delay_osschedlock
 * @brief   us级延时时，关闭任务调度(防止系统调度打断us级延迟)
 * @note    仅支持系统使用
 * @retval  无
 */
void delay_osschedlock(void)
{
/* 使用UCOSIII */
#if defined (CPU_CFG_CRITICAL_METHOD)    			
    OS_ERR err; 
    
    /* UCOSIII的方式，禁止调度，防止打断us延时 */
    OSSchedLock(&err);
    
/* 使用UCOSII */
#elif defined (OS_CRITICAL_METHOD)		

    /* UCOSII的方式，禁止调度 */
    OSSchedLock();
    
/* 使用rt-thread */	
#elif defined (OS_USE_RTTHREAD)
	
    /* rt-thread的方式，禁止调度 */
    rt_enter_critical();
#endif
}

/**
 * @func    delay_osschedunlock
 * @brief   us级延时时,恢复任务调度
 * @note    仅支持系统使用
 * @retval  无
 */
void delay_osschedunlock(void)
{	
#if defined (CPU_CFG_CRITICAL_METHOD)    			
    OS_ERR err; 
	
    /* UCOSIII的方式,恢复调度 */
    OSSchedUnlock(&err);	
	
#elif defined (OS_CRITICAL_METHOD)
	
    /* UCOSII的方式,恢复调度 */
    OSSchedUnlock();	
	
#elif defined (OS_USE_RTTHREAD)
	
    /* rt-thread的方式,恢复调度 */
    rt_exit_critical();
#endif
}

/**
 * @func    delay_ostimedly
 * @brief   调用OS自带的延时函数延时
 * @param   ticks 延时的节拍数
 * @note    仅支持系统使用
 * @retval  无
 */
void delay_ostimedly(uint32_t ticks)
{
#if defined (CPU_CFG_CRITICAL_METHOD) 
    OS_ERR err; 
	
    /* UCOSIII延时采用周期模式 */
    OSTimeDly(ticks,OS_OPT_TIME_PERIODIC, &err);
	
#elif defined (OS_CRITICAL_METHOD)
	
    /* UCOSII延时 */
    OSTimeDly(ticks);		
	
#elif defined (OS_USE_RTTHREAD)
	
    /* rt-thread延时 */
    rt_thread_delay(ticks);
#endif 
}
/* 因为rt-thread本身已经包含了SysTick_Handler，所以仅有uc/OS需要用到此函数 */
#if defined (OS_CRITICAL_METHOD) || defined (CPU_CFG_CRITICAL_METHOD)

/**
 * @func    SysTick_Handler
 * @brief   systick中断服务函数,使用OS时用到
 * @note    当前仅支持uc/OS系统使用
 * @retval  无
 */
void SysTick_Handler(void)
{	
#ifdef USE_HAL_DRIVER
    HAL_IncTick();
#endif
    /* OS开始跑了,才执行正常的调度处理 */
    if (DELAY_OS_RUNNING == 1)					
    {
        /* 进入中断 */
        OSIntEnter();						
        
        /* 调用ucos的时钟服务程序 */
        OSTimeTick();      
        
        /* 触发任务切换软中断 */    
        OSIntExit();       	 				
    }
}
#endif
#endif /* SYSTEM_SUPPORT_OS */

/**
 * @func    delay_init
 * @brief   初始化延迟函数
 * @note    当前仅支持uc/OS系统使用
            当使用uc/OS的时候,此函数会初始化uc/OS的时钟节拍
            SYSTICK的时钟固定为AHB时钟的1/8
 * @retval  无
 */			   
void delay_init(void)
{
    uint32_t sys_tick_freq;
	
	/* 定义RCC_ClocksTypeDef类型的变量，用于获取系统的时钟 */
#if defined (USE_HAL_DRIVER) 
    uint32_t hclk_frequency;
    hclk_frequency = HAL_RCC_GetHCLKFreq();
#elif defined (USE_STDPERIPH_DRIVER)
    RCC_ClocksTypeDef  rcc_clocks;
    
    /* 获取芯片当前的运行的时钟 */
    RCC_GetClocksFreq(&rcc_clocks);
#endif
	
/* 需要支持OS. */
#ifdef SYSTEM_SUPPORT_OS 

/* 使用uc/OS */	
#if defined (OS_CRITICAL_METHOD) || defined (CPU_CFG_CRITICAL_METHOD)
    uint32_t cnts;
#if defined (USE_HAL_DRIVER)
    cnts = (uint32_t)hclk_frequency / DELAY_OS_TICK_PERSEC;
#elif defined (USE_STDPERIPH_DRIVER)
    cnts = (uint32_t)rcc_clocks.HCLK_Frequency / DELAY_OS_TICK_PERSEC;
#endif

    /* 默认使用8分频 */
    cnts = cnts / 8;
    
    /* systick的时钟重载值的配置 */
    SysTick_Config(cnts);
#endif
#endif /* SYSTEM_SUPPORT_OS */
	
/* 不使用rt-thread */
#ifndef OS_USE_RTTHREAD

    /* 配置systick的时钟源，等于系统时钟的8分频 */
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); 
#endif /* OS_USE_RTTHREAD */
    
    if (((SysTick->CTRL) >> 2) & 0x01)
    {
#if defined (USE_HAL_DRIVER)
        
        /* 获取systick的时钟，SysTick直接取HCLK不分频 */
        sys_tick_freq = (uint32_t)hclk_frequency;
#elif defined (USE_STDPERIPH_DRIVER)
        
        /* 获取systick的时钟，SysTick直接取HCLK不分频 */
        sys_tick_freq = (uint32_t)rcc_clocks.HCLK_Frequency;
#endif
    }
    else
    {
#if defined (USE_HAL_DRIVER)
        
        /* 获取systick的时钟，默认是系统时钟的8分频 */
        sys_tick_freq = (uint32_t)hclk_frequency / 8;
#elif defined (USE_STDPERIPH_DRIVER)
        
        /* 获取systick的时钟，默认是系统时钟的8分频 */
        sys_tick_freq = (uint32_t)rcc_clocks.HCLK_Frequency / 8;
#endif
    }
    
    /* 计算每个us包含的时钟数 */
    sys_delay.fac_us = sys_tick_freq / 1000000;
    
#ifdef SYSTEM_SUPPORT_OS
#if defined (OS_USE_RTTHREAD) || defined (OS_CRITICAL_METHOD) || defined (CPU_CFG_CRITICAL_METHOD)
    sys_delay.fac_ms = 1000 / DELAY_OS_TICK_PERSEC;
#endif
#else

    /* 非OS下,代表每个ms需要的systick时钟数 */
    sys_delay.fac_ms = (uint16_t)sys_delay.fac_us * 1000;				
#endif /* SYSTEM_SUPPORT_OS */
}		

/* 如果需要支持OS. */
#if SYSTEM_SUPPORT_OS 

/**
 * @func    delay_us
 * @brief   延时nus
 * @param   nus 要延时的us数	
 * @note    nus的范围0~204522252(最大值即2^32/fac_us@fac_us=21)
 * @retval  无
 */	  								   
void delay_us(uint32_t nus)
{		
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload;
    
    /* 获取当前LOAD的值 */
    reload = SysTick->LOAD;				
    
    /* 计算需要的节拍数 */
    ticks = nus * sys_delay.fac_us; 	
    
    /* 阻止OS调度，防止打断us延时 */
    delay_osschedlock();	
    
    /* 记录当前的计数器的值 */
    told = SysTick->VAL;    
    
    /* 进行 延时 */
    while (1)
    {
        tnow = SysTick->VAL;	
        
        if (tnow != told)
        {	    
            /* 这里注意一下SYSTICK是一个递减的计数器就可以了 */
            if (tnow < told)
                tcnt += told - tnow;	
            else 
                tcnt += reload - tnow + told;	    
            told = tnow;
            
            /* 时间超过/等于要延迟的时间,则退出 */
            if (tcnt >= ticks)
                break;			
        }  
    };
    
    /* 恢复OS调度 */			
    delay_osschedunlock();												    
}  

/**
 * @func    delay_ms
 * @brief   延时nms
 * @param   nms 要延时的ms数	
 * @note    nms的0-65535
 * @retval  无
 */	
void delay_ms(uint16_t nms)
{	
    uint16_t i;
#if defined (OS_CRITICAL_METHOD) || defined (CPU_CFG_CRITICAL_METHOD)
    
    /* 如果OS已经在跑了,并且不是在中断里面(中断里面不能任务调度) */
    if (DELAY_OS_RUNNING && (DELAY_OS_INTNESTING == 0))  
    {	
        /* 延时的时间大于OS的最少时间周期 */
        if (nms >= sys_delay.fac_ms)						
        { 
            /* OS延时 */
            delay_ostimedly(nms / sys_delay.fac_ms);	
        }
        
        /* OS已经无法提供这么小的延时了,采用普通方式延时 */
        nms %= sys_delay.fac_ms;						 
    }
    else
    {
        for (i = 0; i < nms; i++)
        {
            /* 普通方式延时 */
            delay_us(1000);	
        }
        return ;
    }
    
/* 如果使用rt-thread(中断里面不能任务调度) */
#elif defined (OS_USE_RTTHREAD)
    {
        static rt_uint8_t delay_ms_lock = False;
        if((DELAY_OS_RUNNING | delay_ms_lock) && (DELAY_OS_INTNESTING == 0))
        {
            
            /* 用于标志系统是否已经在运行 */
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
                /* 普通方式延时 */
                delay_us(1000);	
            }
            return ;
        }	
    }
#endif
    
    /* 普通方式延时 */
    delay_us((uint32_t)(nms * 1000));				
}

/* 不用ucos时 */
#else  

/**
 * @func    delay_us
 * @brief   延时nus
 * @param   nus 为要延时的us数.	
 * @note    nus的值,不要大于798915us(最大值即2^24/fac_us@fac_us=21)
 * @retval  无
 */	
void delay_us(uint32_t nus)
{		
    uint32_t temp;	    	 
    
    /* 时间加载 */
    SysTick->LOAD = nus * sys_delay.fac_us; 		
    
    /* 清空计数器 */
    SysTick->VAL = 0x00;        				
    
    /* 开始倒数 */
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ; 	 
    
    do
    {
        temp = SysTick->CTRL;
    }while((temp & 0x01 ) && !(temp & (1 << 16)));	//等待时间到达   
    
    /* 关闭计数器 */
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; 
    
    /* 清空计数器 */
    SysTick->VAL = 0X00;       				
}

/**
 * @func    delay_xms
 * @brief   延时nms
 * @param   nms 为要延时的ms数.	
 * @note    注意nms的范围
            SysTick->LOAD为24位寄存器,所以,最大延时为:
            nms<=0xffffff*8*1000/SYSCLK
            SYSCLK单位为Hz,nms单位为ms
            对168M条件下,nms<=798ms 
 * @retval  无
 */	
void delay_xms(uint16_t nms)
{	 		  	  
    uint32_t temp;		   
    
    /* 时间加载(SysTick->LOAD为24bit) */
    SysTick->LOAD = (uint32_t)nms * sys_delay.fac_ms;		
    
    /* 清空计数器 */
    SysTick->VAL = 0x00;           
    
    /* 开始倒数 */
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ;    
    
    do
    {
        temp = SysTick->CTRL;
    }while((temp & 0x01) && !(temp & (1 << 16)));	//等待时间到达   
    
    /* 关闭计数器 */
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;     
    
    /* 清空计数器 */
    SysTick->VAL = 0X00;     		  		  	    
} 

/**
 * @func    delay_ms
 * @brief   延时nms
 * @param   nms 为要延时的ms数.	
 * @note    nms:0~65535
 * @retval  无
 */	
void delay_ms(uint16_t nms)
{	
    /* 这里用540,是考虑到某些客户可能超频使用 */
    uint16_t repeat = nms / 540;	
    
    /* 比如超频到248M的时候,delay_xms最大只能延时541ms左右 */									
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
			 



































