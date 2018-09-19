/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 * 2014-04-27     Bernard      make code cleanup. 
 */

#include <board.h>
#include <rtthread.h>
#include "lvgl.h"
#include "tft.h"
#include "touchpad.h"
#include "touch_ft6306.h"
#include "bsp_basic_tim.h"



#include "pwm.h"


#ifdef RT_USING_LCD
#include "lcd_fsmc.h"
#endif /* RT_USING_LCD */

#ifdef RT_USING_EXT_SRAM
#include "sram.h"
#endif /* RT_USING_EXT_SRAM */

#ifdef RT_USING_I2C
#include "iic.h"
#endif /* RT_USING_I2C */

#ifdef RT_USING_SPI
#include "spi_hard.h"
#ifdef RT_USING_W25QXX
#include "w25qxx.h"
#include "spi_flash.h"
#endif
#endif

#ifdef RT_USING_LED
#include "LED.h"
#endif /* RT_USING_LED */


#ifdef RT_USING_LCDTEST
#include "LCD_TEST.h"
#endif /* RT_USING_LCDTEST */


#include "delay.h"
#include "SEGGER_SYSVIEW_RTThread.h"

/*  变量分配4字节对齐 */
ALIGN(RT_ALIGN_SIZE)

/**********led线程*********/
static struct rt_thread led_thread;
static rt_uint8_t led_stack[256];
static struct rt_thread led_thread;
/**************************/


/**********lcd测试线程*********/
static struct rt_thread lcd_test_thread;
static rt_uint8_t lcd_test_stack[4096];
static struct rt_thread lcd_test_thread;
/**************************/



void rt_init_thread_entry(void* parameter)
{
    rt_err_t result;
    rt_thread_t tid;
    
//    rt_mp_t rt_mp_create(const char* name, rt_size_t block_count, rt_size_t block_size);
    
    /*******************************************/
	delay_init();
    /* GDB STUB */
    
    /************LCD测试*****************/
    /* 创建静态线程 ： 优先级 17 ，时间片 5 个系统滴答 */
    
    lcd_func_init();      //lcd初始化
	lcd.init();
 	lv_init();
	tft_init();
    touch_init();
	touchpad_init();
    fsmc_sram_config();
    TIM1_PWM_Init(899,0);
    
//    rt_trace_init();
    
    result = rt_thread_init(&lcd_test_thread,
    "lcd_test",
    rt_lcd_test_thread_entry, RT_NULL,
    (rt_uint8_t*)&lcd_test_stack[0], sizeof(lcd_test_stack), 2, 5);
    if (result == RT_EOK)
    {
    rt_thread_startup(&lcd_test_thread);
    }
    
    /************LED*****************/
    /* 创建静态线程 ： 优先级 16 ，时间片 5 个系统滴答 */
    result = rt_thread_init(&led_thread,
    "led",
    rt_led_thread_entry, RT_NULL,
    (rt_uint8_t*)&led_stack[0], sizeof(led_stack), 10, 5);
    if (result == RT_EOK)
    {
    rt_thread_startup(&led_thread);
    }

    rt_hw_spi_init();
    result = w25qxx_init("w25q128", "FLASH");    
    
    if (result == RT_EOK)
    {
        rt_spi_flash_device_t flash_device;
        uint8_t tmp = 0;
        
        flash_device = (rt_spi_flash_device_t)rt_device_find("w25q128");
        if (flash_device == RT_NULL)
        {
            rt_kprintf("can`t find device w25q128!\r\n");

            return ;
        }
        
        flash_device->flash_device.init(&flash_device->flash_device);    
        
        flash_device->flash_device.read(&flash_device->flash_device, 4096, &tmp, 1);
//        if (tmp == 0xff)
//        {
//            sys_word_mode = MODE_BOTH_FINGER_RFID;
//        }
//        else
//        {
//            sys_word_mode = tmp;
//        }
    }
    

}

int rt_application_init()
{
    rt_thread_t tid;

     tid = rt_thread_create("init",
        rt_init_thread_entry, RT_NULL,
        2048, RT_THREAD_PRIORITY_MAX/3, 20);

    if (tid != RT_NULL)
        rt_thread_startup(tid);
    return 0;
}
