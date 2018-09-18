/**
******************************************************************************
 * @Copyright       (C) 2017 - 2018 guet-sctc-hardwarepart Team
 * @filename        delay.h
 * @author          �Ž�����С��
 * @version         V1.0.3
 * @date            2018-01-11
 * @Description     delay�ļ����������Ƿ�ʹ��ϵͳ�ĺ궨��ѡ���Լ�ʹ��ϵͳ������
 * @Others
 * @History
 * Date           	Author    	version                 Notes
 * 2017-11-05     	ZSY       	V1.0.0              first version.
 * 2017-11-13       ZSY         V1.0.1              �����ṹ
 * 2018-01-11       ZSY         V1.0.2              �Ű��ʽ����������ǿ�ڲ�ͬ��������µĿɶ��ԣ�����HAL���֧��
 * 2018-01-11       ZSY         V1.0.3              �޸Ĳ��ֱ���������
 * @verbatim  
 */
	
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DELAY_H_
#define __DELAY_H_ 		

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx.h>	  

/* Public macro Definition ---------------------------------------------------*/
/**
 * ��delay_us/delay_ms��Ҫ֧��OS��ʱ����Ҫ������OS��صĺ궨��ͺ�����֧��		
 * ������3���궨��:
 * SYSTEM_SUPPORT_OS            �Ƿ�ʹ��ϵͳ
	
 * OS_USE_RTTHREAD              �Ƿ�ʹ��rt-thread
	
 * OS_CRITICAL_METHOD           �Ƿ�ʹ��uc/OS-II
	
 * CPU_CFG_CRITICAL_METHOD      �Ƿ�ʹ��uc/OS-III
	
	
 * DELAY_OS_RUNNING             ���ڱ�ʾOS��ǰ�Ƿ���������,�Ծ����Ƿ����ʹ����غ���
 * DELAY_OS_TICK_PERSEC         ���ڱ�ʾOS�趨��ʱ�ӽ���,delay_init�����������������ʼ��systick
 * DELAY_OS_INTNESTING          ���ڱ�ʾOS�ж�Ƕ�׼���,��Ϊ�ж����治���Ե���,delay_msʹ�øò����������������
 */

/* 0Ϊ��ʹ��ϵͳ��1Ϊʹ��ϵͳ */
#define SYSTEM_SUPPORT_OS       1					
#if SYSTEM_SUPPORT_OS == 0
#undef SYSTEM_SUPPORT_OS
#else

/* 0Ϊ��ʹ��rt-threadϵͳ��1Ϊʹ��rt-threadϵͳ */
#define OS_USE_RTTHREAD         1
#if OS_USE_RTTHREAD == 0
#undef OS_USE_RTTHREAD
#else
#define DELAY_OS_RUNNING rt_tick_get()
#define DELAY_OS_TICK_PERSEC RT_TICK_PER_SECOND         //OSʱ�ӽ���,��ÿ����ȴ���
#define DELAY_OS_INTNESTING rt_interrupt_get_nest()     //�ж�Ƕ�׼���,���ж�Ƕ�״���
#endif /* OS_USE_RTTHREAD */

/* 0Ϊ��ʹ��uc/OS-IIϵͳ��1Ϊʹ��uc/OS-IIϵͳ */
#define OS_CRITICAL_METHOD      0
#if OS_CRITICAL_METHOD == 0
#undef OS_CRITICAL_METHOD
#else
#define DELAY_OS_RUNNING OSRunning                  //OS�Ƿ����б��,0,������;1,������
#define DELAY_OS_TICK_PERSEC OS_TICKS_PER_SEC       //OSʱ�ӽ���,��ÿ����ȴ���
#define DELAY_OS_INTNESTING OSIntNesting            //�ж�Ƕ�׼���,���ж�Ƕ�״���
#endif /* OS_CRITICAL_METHOD */

/* 0Ϊ��ʹ��uc/OS-IIIϵͳ��1Ϊʹ��uc/OS-IIIϵͳ */
#define CPU_CFG_CRITICAL_METHOD     0
#if CPU_CFG_CRITICAL_METHOD == 0
#undef CPU_CFG_CRITICAL_METHOD
#else
#define DELAY_OS_RUNNING OSRunning                  //OS�Ƿ����б��,0,������;1,������
#define DELAY_OS_TICK_PERSEC OSCfg_TickRate_Hz      //OSʱ�ӽ���,��ÿ����ȴ���
#define DELAY_OS_INTNESTING OSIntNestingCtr         //�ж�Ƕ�׼���,���ж�Ƕ�״���
#endif /* CPU_CFG_CRITICAL_METHOD */
#endif /* SYSTEM_SUPPORT_OS */

/* End public macro Definition -----------------------------------------------*/

/* UserCode start ------------------------------------------------------------*/

/* ��ʱ���ԵĽṹ�� */
typedef struct delay
{
    uint16_t fac_us;            //us��ʱ������			   
    uint32_t fac_ms;            //ms��ʱ������,��os��,����ÿ�����ĵ�ms��
}delay_Typedef;

/* Member method APIs --------------------------------------------------------*/
void delay_init(void);
void delay_ms(uint16_t nms);
void delay_us(uint32_t nus);
/* End Member Method APIs ----------------------------------------------------*/

#endif /* __DELAY_H_ */





























