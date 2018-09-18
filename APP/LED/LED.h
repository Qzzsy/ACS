/**
******************************************************************************
 * @Copyright		(C) 2017 - 2018 guet-sctc-hardwarepart Team
 * @filename		led.h
 * @author			�Ž�����С��
 * @version			V1.0.0
 * @date			2018-01-26
 * @Description		led�ļ���ָ����led app��api
 * @Others
 * @History
 * Date           	Author    	version    				Notes
 * 2018-08-8    	Zehn       	V1.0.0				first version.
 * @verbatim  
 */
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _LED_H_
#define _LED_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "rtthread.h"

/* Member method APIs --------------------------------------------------------*/
void rt_led_thread_entry(void * parameter);

/* End Member Method APIs ----------------------------------------------------*/
#endif