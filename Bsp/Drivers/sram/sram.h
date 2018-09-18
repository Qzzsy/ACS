/**
 ******************************************************************************
 * @Copyright       (C) 2017 - 2017 guet-sctc-hardwarepart Team
 * @filename        sram.h
 * @author          门禁开发小组
 * @version         V1.0.0
 * @date            2017-11-04
 * @Description     sram头文件，在此文件内SRAM的初始地址和对外开放的API接口。
 * @Others
 * @History
 * Date           Author    version    		Notes
 * 2017-11-04     ZSY       V1.0.0          first version.
 * @verbatim  
 */
	
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SRAM_H_
#define __SRAM_H_		

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h" 

/* UserCode start ------------------------------------------------------------*/

#define Bank1_SRAM3_ADDR    ((u32)(0x68000000))

void fsmc_sram_config(void);


/* 以下代码为测试代码 */
//void FSMC_SRAM_WriteBuffer(u8* pBuffer,u32 WriteAddr,u32 NumHalfwordToWrite);
//void FSMC_SRAM_ReadBuffer(u8* pBuffer,u32 ReadAddr,u32 NumHalfwordToRead);


//void fsmc_sram_test_write(u32 addr,u8 data);
//u8 fsmc_sram_test_read(u32 addr);
uint8_t bsp_TestExtSRAM(void);



#endif

