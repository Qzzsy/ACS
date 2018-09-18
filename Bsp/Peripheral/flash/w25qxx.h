/**
******************************************************************************
 * @Copyright		(C) 2017 - 2017 guet-sctc-hardwarepart Team
 * @filename		w25qxx.h
 * @author			门禁开发小组
 * @version			V1.0.0
 * @date			2017-12-19
 * @Description		w25qxx文件，包含了w25qxx操作的一些常规的命令
 * @Others
 * @History
 * Date           	Author    	version    				Notes
 * 2017-12-19     	ZSY       	V1.0.0				first version.
 * 2017-12-21		ZSY			V1.0.1				添加公有和私有宏定义
 * @verbatim  
 */
	
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _W25QXX_H_
#define _W25QXX_H_

/* Includes ------------------------------------------------------------------*/
#include <rtthread.h>

/* Public macro Definition ---------------------------------------------------*/

/* command list */
#define FLASH_CMD_WRSR                  (0x01)  /* Write Status Register */
#define FLASH_CMD_PP                    (0x02)  /* Page Program */
#define FLASH_CMD_READ                  (0x03)  /* Read Data */
#define FLASH_CMD_WRDI                  (0x04)  /* Write Disable */
#define FLASH_CMD_RDSR1                 (0x05)  /* Read Status Register-1 */
#define FLASH_CMD_WREN                  (0x06)  /* Write Enable */
#define FLASH_CMD_FAST_READ             (0x0B)  /* Fast Read */
#define FLASH_CMD_ERASE_4K              (0x20)  /* Sector Erase:4K */
#define FLASH_CMD_RDSR2                 (0x35)  /* Read Status Register-2 */
#define FLASH_CMD_ERASE_32K             (0x52)  /* 32KB Block Erase */
#define FLASH_CMD_JEDEC_ID              (0x9F)  /* Read JEDEC ID */
#define FLASH_CMD_ERASE_FULL            (0xC7)  /* Chip Erase */
#define FLASH_CMD_ERASE_64K             (0xD8)  /* 64KB Block Erase */
#define FLASH_CMD_POWER_DOWN			(0xB9)	/* 进入睡眠模式 */
#define FLASH_CMD_POWER_RELEASE			(0xAB)	/* 退出睡眠模式 */

#ifdef FLASH_W25Q256
#define FLASH_CMD_ENTER_4_BYTE_MODE		(0XB7)	/* 进入4字节地址模式 */
#define FLASH_CMD_EXIT_4_BYTE_MODE		(0XE9)	/* 退出4字节地址模式 */
#define FLASH_CMD_READ_32B_ADDR			(0X13)	/* Read Data 32bit address */
#endif /* FLASH_W25Q256 */
#define DUMMY                       	(0xFF)
/* End public macro Definition -----------------------------------------------*/
/* UserCode start ------------------------------------------------------------*/

/* Member method APIs --------------------------------------------------------*/
extern rt_err_t w25qxx_init(const char * flash_device_name,
                            const char * spi_device_name);

/* End Member Method APIs ---------------------------------------------------*/

#endif /* _W25QXX_H_ */
