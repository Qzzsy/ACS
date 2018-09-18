/**
******************************************************************************
 * @Copyright       (C) 2017 - 2018 guet-sctc-hardwarepart Team
 * @filename        delay.h
 * @author          门禁开发小组
 * @version         V1.0.3
 * @date            2018-01-11
 * @Description     delay文件，包含了是否使用系统的宏定义选择以及使用系统的类型
 * @Others
 * @History
 * Date           	Author    	version                 Notes
 * 2017-11-05     	ZSY       	V1.0.0              first version.
 * 2017-11-13       ZSY         V1.0.1              调整结构
 * 2018-01-11       ZSY         V1.0.2              排版格式化操作，增强在不同缩进情况下的可读性，增加HAL库的支持
 * 2018-01-11       ZSY         V1.0.3              修改部分变量的声明
 * @verbatim  
 */
	
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DELAY_H_
#define __DELAY_H_ 		

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx.h>	  

/* Public macro Definition ---------------------------------------------------*/
/**
 * 当delay_us/delay_ms需要支持OS的时候需要三个与OS相关的宏定义和函数来支持		
 * 首先是3个宏定义:
 * SYSTEM_SUPPORT_OS            是否使用系统
	
 * OS_USE_RTTHREAD              是否使用rt-thread
	
 * OS_CRITICAL_METHOD           是否使用uc/OS-II
	
 * CPU_CFG_CRITICAL_METHOD      是否使用uc/OS-III
	
	
 * DELAY_OS_RUNNING             用于表示OS当前是否正在运行,以决定是否可以使用相关函数
 * DELAY_OS_TICK_PERSEC         用于表示OS设定的时钟节拍,delay_init将根据这个参数来初始哈systick
 * DELAY_OS_INTNESTING          用于表示OS中断嵌套级别,因为中断里面不可以调度,delay_ms使用该参数来决定如何运行
 */

/* 0为不使用系统，1为使用系统 */
#define SYSTEM_SUPPORT_OS       1					
#if SYSTEM_SUPPORT_OS == 0
#undef SYSTEM_SUPPORT_OS
#else

/* 0为不使用rt-thread系统，1为使用rt-thread系统 */
#define OS_USE_RTTHREAD         1
#if OS_USE_RTTHREAD == 0
#undef OS_USE_RTTHREAD
#else
#define DELAY_OS_RUNNING rt_tick_get()
#define DELAY_OS_TICK_PERSEC RT_TICK_PER_SECOND         //OS时钟节拍,即每秒调度次数
#define DELAY_OS_INTNESTING rt_interrupt_get_nest()     //中断嵌套级别,即中断嵌套次数
#endif /* OS_USE_RTTHREAD */

/* 0为不使用uc/OS-II系统，1为使用uc/OS-II系统 */
#define OS_CRITICAL_METHOD      0
#if OS_CRITICAL_METHOD == 0
#undef OS_CRITICAL_METHOD
#else
#define DELAY_OS_RUNNING OSRunning                  //OS是否运行标记,0,不运行;1,在运行
#define DELAY_OS_TICK_PERSEC OS_TICKS_PER_SEC       //OS时钟节拍,即每秒调度次数
#define DELAY_OS_INTNESTING OSIntNesting            //中断嵌套级别,即中断嵌套次数
#endif /* OS_CRITICAL_METHOD */

/* 0为不使用uc/OS-III系统，1为使用uc/OS-III系统 */
#define CPU_CFG_CRITICAL_METHOD     0
#if CPU_CFG_CRITICAL_METHOD == 0
#undef CPU_CFG_CRITICAL_METHOD
#else
#define DELAY_OS_RUNNING OSRunning                  //OS是否运行标记,0,不运行;1,在运行
#define DELAY_OS_TICK_PERSEC OSCfg_TickRate_Hz      //OS时钟节拍,即每秒调度次数
#define DELAY_OS_INTNESTING OSIntNestingCtr         //中断嵌套级别,即中断嵌套次数
#endif /* CPU_CFG_CRITICAL_METHOD */
#endif /* SYSTEM_SUPPORT_OS */

/* End public macro Definition -----------------------------------------------*/

/* UserCode start ------------------------------------------------------------*/

/* 延时属性的结构体 */
typedef struct delay
{
    uint16_t fac_us;            //us延时倍乘数			   
    uint32_t fac_ms;            //ms延时倍乘数,在os下,代表每个节拍的ms数
}delay_Typedef;

/* Member method APIs --------------------------------------------------------*/
void delay_init(void);
void delay_ms(uint16_t nms);
void delay_us(uint32_t nus);
/* End Member Method APIs ----------------------------------------------------*/

#endif /* __DELAY_H_ */





























