/**
 ******************************************************************************
 * @file        LCD_TEST.c
 * @author      门禁开发小组
 * @version     V1.0.0
 * @date        2018-01-26
 * @brief       系统运行指示灯，可以通过RT_USING_RUN_LED控制是否需要
 * @note        
 * @History
 * Date             Author      version                 Notes
 * 2018-08-8       Zehn        V1.0.0             first version.
 */
	
/* Includes ------------------------------------------------------------------*/
#include "LCD_TEST.h"
#include "delay.h"
#include "lcd_fsmc.h"
#include "lvgl/lvgl.h"
#include "sram.h"	    
#include "demo.h"	 
#include "main_task.h"   

/**
 * @func	rt_LCD_TEST_thread_entry`
 * @brief 	LCD_TEST线程入口
 * @param	parameter 用户数据
 * @note  	
 * @retval	无
 */
void rt_lcd_test_thread_entry(void * parameter)
{
	(void)parameter;
    TIM_SetCompare4(TIM1,400);
    if(bsp_TestExtSRAM())
    {
      lcd.draw_colorbox(0,0,300,300,0xff00);    
    }
    main_task_creat();
	while(1)
	{
        lv_tick_inc(1);		
		lv_task_handler();
		delay_ms(1);
	}
}