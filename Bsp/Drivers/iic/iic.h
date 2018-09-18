/**
 ******************************************************************************
 * @Copyright     (C) 2017 - 2018 guet-sctc-hardwarepart Team
 * @filename      iic.h
 * @author        �Ž�����С��
 * @version       V1.0.3
 * @date          2018-01-26
 * @Description   bsp_iic�ļ����ڴ��ļ��ڶ�����һЩiic�����ź궨�壬��Ҫ����iic��ʱ
                  ������ʱ�ڴ��ļ��ڽ��������޸ļ��ɣ�������ļ�����iic���⿪�ŵ�API
 * @Others
 * @History
 * Date           Author    version    		Notes
 * 2017-11-01     ZSY       V1.0.0      first version.
 * 2017-11-02     ZSY       V1.0.1      �����˺�IIC_ACK_TIMEOUT��IIC_OPER_OK��
                                        IIC_OPER_FAILT
 * 2018-01-09     ZSY       V1.0.2      �Ű��ʽ������.
 * 2018-01-26     ZSY       V1.0.3      ���˽�к͹��к궨��.
 */
	
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _IIC_H_
#define _IIC_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"	 

/* Public macro Definition ---------------------------------------------------*/

/* ���������� */
#define IIC_OPER_OK       (0)		//�����ɹ�
#define IIC_OPER_FAILT    (1)		//����ʧ��

#define IIC_NEED_ACK      (1)
#define IIC_NEEDNT_ACK    (0)
/* End public macro Definition -----------------------------------------------*/

/* UserCode start ------------------------------------------------------------*/
/* Member method APIs --------------------------------------------------------*/
/* config iic gpio */
void iic_gpio_config(void);

void iic_start(void); 
void iic_stop(void);
uint8_t iic_wait_ack(void);
void iic_send_byte(uint8_t data);
uint8_t iic_read_byte(uint8_t ack);

/* End Member Method APIs ---------------------------------------------------*/

#endif







