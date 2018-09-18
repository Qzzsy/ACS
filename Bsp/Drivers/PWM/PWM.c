/**
 ******************************************************************************
 * @file        pwm.c
 * @author      �Ž�����С��
 * @version     V1.0.0
 * @date        2018-01-11
 * @brief       ����ļ���ϵͳPWMʵ�ֵķ�����ʵ�ֶԱ���ĵ���
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
 * @brief   TIM1 PWM���ֳ�ʼ����PWM�����ʼ��
 * @param   arr �Զ���װֵ	
 * @param   psc ʱ��Ԥ��Ƶ��	
 * @note    ��Ҫ������Ӧ��ʱ�Ӻ͸���
 * @retval  ��
 */  

void TIM1_PWM_Init(uint32_t arr, uint32_t psc)
{		 				
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);  	                //TIM1ʱ��ʹ��    
    RCC_AHB1PeriphClockCmd(LCD_LIGHT_RCC, ENABLE); 	                        //ʹ��LCD_LIGHT_RCCʱ��	
    
    GPIO_PinAFConfig(LCD_LIGHT_PORT, LCD_LIGHT_PIN_SOURCE, GPIO_AF_TIM1);   //LCD_LIGHT_GPIO����Ϊ��ʱ��1
    
    GPIO_InitStructure.GPIO_Pin   = LCD_LIGHT_GPIO;                 //LCD_LIGHT_GPIO
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;                   //���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	            //�ٶ�100MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                  //���츴�����
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;                   //����
    GPIO_Init(LCD_LIGHT_PORT, &GPIO_InitStructure);                 //��ʼ��LCD_LIGHT_PORT
    
    TIM_TimeBaseStructure.TIM_Prescaler     = psc;                  //��ʱ����Ƶ
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;   //���ϼ���ģʽ
    TIM_TimeBaseStructure.TIM_Period        = arr;                  //�Զ���װ��ֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
    
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);                 //��ʼ����ʱ��1
    
    //��ʼ��TIM1 Channel4 PWMģʽ	 
    TIM_OCInitStructure.TIM_OCMode       = TIM_OCMode_PWM2;         //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
    TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable;  //�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_OutputNState = TIM_Channel_4;
    TIM_OCInitStructure.TIM_OCPolarity   = TIM_OCPolarity_Low;      //�������:TIM����Ƚϼ��Ե�
    
    TIM_OC4Init(TIM1, &TIM_OCInitStructure);                        //����Tָ���Ĳ�����ʼ������TIM1 OC4
    
    TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);               //ʹ��TIM1��CCR4�ϵ�Ԥװ�ؼĴ���
    
    TIM_ARRPreloadConfig(TIM1, ENABLE);                             //ARPEʹ�� 
    
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    
    TIM_Cmd(TIM1, ENABLE);                                  
}  


