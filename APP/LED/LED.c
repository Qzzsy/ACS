/**
 ******************************************************************************
 * @file        LED.c
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
#include "LED.h"
#include "delay.h"

/* global variable Declaration -----------------------------------------------*/

/* User function Declaration -------------------------------------------------*/

/* User functions ------------------------------------------------------------*/
void IWDG_Init(u8 prer, u16 rlr)  
{  
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);//使能对IWDG->PR IWDG->RLR的写  
         
  IWDG_SetPrescaler(prer); //设置IWDG分频系数  
   
  IWDG_SetReload(rlr);   //设置IWDG装载值  
   
  IWDG_ReloadCounter(); //reload  
         
  IWDG_Enable();       //使能看门狗  
}  
   
void IWDG_Feed(void)  
{  
  IWDG_ReloadCounter();//reload  
} 

/**
 * @func	LED_init
 * @brief 	对LED端口进行配置
 * @note  	
 * @retval	无
 */
void LED_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;        //PC11OUT
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;      //输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  //100MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;       //上拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);              //初始化
    /* 默认输出低 */
	GPIO_ResetBits(GPIOC,GPIO_Pin_11);
   
    
    
    IWDG_Init(IWDG_Prescaler_64, 600); //与分频数为64,重载值为500,溢出时间为1s  
}
/**
 * @func	rt_led_thread_entry
 * @brief 	led线程入口
 * @param	parameter 用户数据
 * @note  	
 * @retval	无
 */
void rt_led_thread_entry(void * parameter)
{
    /* 防止警告 */
	(void)parameter;
    
    IWDG_Feed();
    
    /* 初始化端口 */
	LED_init();
	while(1)
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_11);
		delay_ms(600); //延时500ms
        IWDG_Feed();
		GPIO_SetBits(GPIOC,GPIO_Pin_11);
		delay_ms(600);
        IWDG_Feed();
	}
}