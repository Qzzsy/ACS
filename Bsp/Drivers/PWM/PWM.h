/**
******************************************************************************
 * @Copyright		(C) 2017 - 2018 guet-sctc-hardwarepart Team
 * @filename		pwm.h
 * @author			�Ž�����С��
 * @version			V1.0.0
 * @date			2018-02-23
 * @Description		pwm�ļ���ָ����pwm app��api
 * @Others
 * @History
 * Date           	Author    	version    				Notes
 * 2018-02-23    	ZSY       	V1.0.0				first version.
 * @verbatim  
 */
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PWM_H_
#define _PWM_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* UserCode start ------------------------------------------------------------*/

/* Member method APIs --------------------------------------------------------*/
void TIM1_PWM_Init(uint32_t arr, uint32_t psc);

/* End Member Method APIs ----------------------------------------------------*/


#endif



