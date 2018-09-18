/**
 ******************************************************************************
 * @file        LED.c
 * @author      �Ž�����С��
 * @version     V1.0.0
 * @date        2018-01-26
 * @brief       ϵͳ����ָʾ�ƣ�����ͨ��RT_USING_RUN_LED�����Ƿ���Ҫ
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
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);//ʹ�ܶ�IWDG->PR IWDG->RLR��д  
         
  IWDG_SetPrescaler(prer); //����IWDG��Ƶϵ��  
   
  IWDG_SetReload(rlr);   //����IWDGװ��ֵ  
   
  IWDG_ReloadCounter(); //reload  
         
  IWDG_Enable();       //ʹ�ܿ��Ź�  
}  
   
void IWDG_Feed(void)  
{  
  IWDG_ReloadCounter();//reload  
} 

/**
 * @func	LED_init
 * @brief 	��LED�˿ڽ�������
 * @note  	
 * @retval	��
 */
void LED_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;        //PC11OUT
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;      //���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  //100MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;       //����
	GPIO_Init(GPIOC, &GPIO_InitStructure);              //��ʼ��
    /* Ĭ������� */
	GPIO_ResetBits(GPIOC,GPIO_Pin_11);
   
    
    
    IWDG_Init(IWDG_Prescaler_64, 600); //���Ƶ��Ϊ64,����ֵΪ500,���ʱ��Ϊ1s  
}
/**
 * @func	rt_led_thread_entry
 * @brief 	led�߳����
 * @param	parameter �û�����
 * @note  	
 * @retval	��
 */
void rt_led_thread_entry(void * parameter)
{
    /* ��ֹ���� */
	(void)parameter;
    
    IWDG_Feed();
    
    /* ��ʼ���˿� */
	LED_init();
	while(1)
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_11);
		delay_ms(600); //��ʱ500ms
        IWDG_Feed();
		GPIO_SetBits(GPIOC,GPIO_Pin_11);
		delay_ms(600);
        IWDG_Feed();
	}
}