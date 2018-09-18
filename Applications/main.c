#include "main.h"
#include "rtthread.h"
#include "LED.h"
#include "lcd.h"
#include "iic.h"
#include "touch_ft6206.h"
#include "stdbool.h"
#include "delay.h"

void LED2_init()
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PD0,1,4,5,8,9,10,14,15 AF OUT
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用输出
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化
	
	GPIO_ResetBits(GPIOF,GPIO_Pin_10);
}

void rt_touch_thread_entry(void * paramer)
{
	(void)paramer;
	uint8_t touch_xy_val[4] = {'\0'}, touch_point_num;
	iic_gpio_config();
	ct_init_val();
	while(true)
	{
		get_touch_xy(touch_xy_val, touch_point_num);
		//GUI_TOUCH_X_MeasureX(); 
		rt_thread_delay(10);
	}
}

//主函数尽量不要设置为死循环，这里无法挂起主函数
int main(void)
{
	uint8_t i = 0;
	rt_thread_t tid;
	LED2_init();
	tid = rt_thread_create("LED", &rt_led_thread_entry, RT_NULL, 128, 8, 1);
	if(tid != RT_NULL)
	{
		rt_thread_startup(tid);
	}
	
	lcd_init();
	lcd_draw_colorbox(0, 0, 480, 320, BLACK);
	
	tid = rt_thread_create("touch", &rt_touch_thread_entry, RT_NULL, 1024, 7, 1);
	if(tid != RT_NULL)
	{
		rt_thread_startup(tid);
	}
	delay_init();
	while (1)
	{
		if (i == 15)
		{
			i = 0;
		}
		switch (i)
		{
			case 0 : lcd_draw_colorbox(0, 0, 480, 320, BLUE); break;
			case 1 : lcd_draw_colorbox(0, 0, 480, 320, DGRAY); break;
			case 2 : lcd_draw_colorbox(0, 0, 480, 320, GREEN); break;
			case 3 : lcd_draw_colorbox(0, 0, 480, 320, CYAN); break;
			case 4 : lcd_draw_colorbox(0, 0, 480, 320, RED); break;
			case 5 : lcd_draw_colorbox(0, 0, 480, 320, MAGENTA); break;
			case 6 : lcd_draw_colorbox(0, 0, 480, 320, YELLOW); break;
			case 7 : lcd_draw_colorbox(0, 0, 480, 320, WHITE); break;
			case 8 : lcd_draw_colorbox(0, 0, 480, 320, NAVY); break;
			case 9 : lcd_draw_colorbox(0, 0, 480, 320, DGREEN); break;
			case 10 : lcd_draw_colorbox(0, 0, 480, 320, DCYAN); break;
			case 11 : lcd_draw_colorbox(0, 0, 480, 320, MAROON); break;
			case 12 : lcd_draw_colorbox(0, 0, 480, 320, PURPLE); break;
			case 13 : lcd_draw_colorbox(0, 0, 480, 320, OLIVE); break;
			case 14 : lcd_draw_colorbox(0, 0, 480, 320, LGRAY); break;
			default : break;                                            
		}                                                             
		rt_thread_delay(50);                                         
		i++;                                                         
	}                                                              
}                                                                
                                                                 
                                                                 
       


