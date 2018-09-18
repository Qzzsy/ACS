/**
 ******************************************************************************
 * @Copyright     (C) 2017 - 2018 guet-sctc-hardwarepart Team
 * @filename      iic.h
 * @author        门禁开发小组
 * @version       V1.0.3
 * @date          2018-01-26
 * @Description   bsp_iic文件，在此文件内定义了一些iic的引脚宏定义，需要更换iic的时
                  序引脚时在此文件内进行引脚修改即可，此外此文件包含iic对外开放的API
 * @Others
 * @History
 * Date           Author    version    		Notes
 * 2017-11-01     ZSY       V1.0.0      first version.
 * 2017-11-02     ZSY       V1.0.1      增加了宏IIC_ACK_TIMEOUT、IIC_OPER_OK和
                                        IIC_OPER_FAILT
 * 2018-01-09     ZSY       V1.0.2      排版格式化操作.
 * 2018-01-26     ZSY       V1.0.3      添加私有和公有宏定义.
 */
	
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _IIC_H_
#define _IIC_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"	 

/* Public macro Definition ---------------------------------------------------*/

/* 定义错误代码 */
#define IIC_OPER_OK       (0)		//操作成功
#define IIC_OPER_FAILT    (1)		//操作失败

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







