/**
 ******************************************************************************
 * @file        pwm.c
 * @author      门禁开发小组
 * @version     V1.0.0
 * @date        2018-01-11
 * @brief       这个文件是系统PWM实现的方法，实现对背光的调节
 * @History
 * Date           	Author    	version    				Notes
 * 2018-02-23     	ZSY       	V1.0.0              first version.
 */
/* Includes ------------------------------------------------------------------*/
#include "pwm.h"
#include "lcd_fsmc.h"

/* global variable Declaration -----------------------------------------------*/


/* User function Declaration --------------------------------------------------*/



/* User functions -------------------------------------------------------------*/

/**
 * @func    TIM1_PWM_Init
 * @brief   TIM1 PWM部分初始化，PWM输出初始化
 * @param   arr 自动重装值	
 * @param   psc 时钟预分频数	
 * @note    需要开启对应的时钟和复用
 * @retval  无
 */  

void TIM1_PWM_Init(uint32_t arr, uint32_t psc)
{		 				
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);  	                //TIM1时钟使能    
    RCC_AHB1PeriphClockCmd(LCD_LIGHT_RCC, ENABLE); 	                        //使能LCD_LIGHT_RCC时钟	
    
    GPIO_PinAFConfig(LCD_LIGHT_PORT, LCD_LIGHT_PIN_SOURCE, GPIO_AF_TIM1);   //LCD_LIGHT_GPIO复用为定时器1
    
    GPIO_InitStructure.GPIO_Pin   = LCD_LIGHT_GPIO;                 //LCD_LIGHT_GPIO
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;                   //复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	            //速度100MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                  //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;                   //上拉
    GPIO_Init(LCD_LIGHT_PORT, &GPIO_InitStructure);                 //初始化LCD_LIGHT_PORT
    
    TIM_TimeBaseStructure.TIM_Prescaler     = psc;                  //定时器分频
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;   //向上计数模式
    TIM_TimeBaseStructure.TIM_Period        = arr;                  //自动重装载值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
    
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);                 //初始化定时器1
    
    //初始化TIM1 Channel4 PWM模式	 
    TIM_OCInitStructure.TIM_OCMode       = TIM_OCMode_PWM2;         //选择定时器模式:TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable;  //比较输出使能
    TIM_OCInitStructure.TIM_OutputNState = TIM_Channel_4;
    TIM_OCInitStructure.TIM_OCPolarity   = TIM_OCPolarity_Low;      //输出极性:TIM输出比较极性低
    
    TIM_OC4Init(TIM1, &TIM_OCInitStructure);                        //根据T指定的参数初始化外设TIM1 OC4
    
    TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);               //使能TIM1在CCR4上的预装载寄存器
    
    TIM_ARRPreloadConfig(TIM1, ENABLE);                             //ARPE使能 
    
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    
    TIM_Cmd(TIM1, ENABLE);                                  
}  


