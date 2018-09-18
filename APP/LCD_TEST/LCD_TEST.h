/**
******************************************************************************
 * @Copyright		(C) 2017 - 2018 guet-sctc-hardwarepart Team
 * @filename		led.h
 * @author			门禁开发小组
 * @version			V1.0.0
 * @date			2018-01-26
 * @Description		led文件，指定了led app的api
 * @Others
 * @History
 * Date           	Author    	version    				Notes
 * 2018-08-8    	Zehn       	V1.0.0				first version.
 * @verbatim  
 */
 
/* Define to prevent recursive inclu*/


#ifndef __LCD_TEST_H__
#define __LCD_TEST_H__


/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "rtthread.h"

/* Member method APIs --------------------------------------------------------*/
void rt_lcd_test_thread_entry(void * parameter);


#endif