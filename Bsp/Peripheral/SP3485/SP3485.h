/**
 ******************************************************************************
 * @Copyright       (C) 2017 - 2018 guet-sctc-hardwarepart Team
 * @filename        sp3485.h
 * @author          门禁开发小组
 * @version         V1.0.0
 * @date            2018-02-28
 * @Description     sp3485头文件，在此文件内实现sp3485设备对外开放的API接口。
 * @Others
 * @History
 * Date           Author    version    		Notes
 * 2018-02-28     ZSY       V1.0.0          first version.
 * @verbatim  
 */
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _SP3485_H_
#define _SP3485_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "rtthread.h"

/* Public macro Definition ---------------------------------------------------*/
#define SP3485_USING_UART       "uart2"
#define SP3485_BUSY             (0)
#define SP3485_NO_BUSY          (1)

/* End public macro Definition -----------------------------------------------*/

/* UserCode start ------------------------------------------------------------*/
/* Member method APIs --------------------------------------------------------*/
rt_err_t sp3485_init(void);
rt_err_t sp3485_send_data(const uint8_t * data_buf, rt_size_t size);
rt_size_t sp3485_get_rev_status(void);
rt_err_t sp3485_rev_data(uint8_t * data_buf, rt_size_t size);
rt_err_t sp3485_open(void);
rt_err_t sp3485_close(void);
rt_err_t sp3485_busy(void);
rt_err_t sp3485_control(void);
/* End Member Method APIs ---------------------------------------------------*/
/* UserCode end -------------------------------------------------------------*/

#endif
