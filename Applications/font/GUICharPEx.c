/**
 ******************************************************************************
 * @file		GUICharPEx.c
 * @author		门禁开发小组
 * @version		V1.0.0
 * @date		2018-01-20
 * @brief       这种汉字显示方式适合使用等宽字体，非等宽的微软雅黑等字体显示效果较差。
 *              支持点阵：
 *                1. 支持12点阵，16点阵，24点阵，32点阵，40点阵以及48点阵汉字。
 *                2. 8X16点阵的ASCII，12X24点阵的ASCII，16X32点阵的ASCII，20X40点阵的ASCII，24X48点阵的ASCII。
 *              字库存储方式：
 *                1. 字库存储到SPI FLASH里面。
 * @note		
 * @History
 * Date           	Author    	version    				Notes
 * 2018-01-20    	ZSY       	V1.0.0				first version.
 */
	
/* Includes ------------------------------------------------------------------*/

#include "stm32f4xx.h"
#include "rtthread.h"
#include "string.h"
#include "SPI_flash.h" 
#include "../lv_font.h"


#include "stm32f4xx.h"                  // Device header
u8 GUI_FontDataBuf[BYTES_PER_FONT];

/* User function Declaration -------------------------------------------------*/

/* User functions ------------------------------------------------------------*/
/**
 * @func	GUI_GetDataFromMemory
 * @brief 	读取点阵数据
 * @param	pProp GUI_FONT_PROP类型结构
 * @param	c 字符
 * @note  	
 * @retval	无
 */
static void GUI_GetDataFromMemory(const lv_font_t GUI_UNI_PTR, char *c) 
{ 
    u16 BytesPerFont; 
    u32 oft = 0, BaseAdd; 
	u8 code1, code2, FontType = 0;
	
	char *font = (char *)GUI_UNI_PTR.glyph_bitmap; 

	/* 每个字模的数据字节数 */
    BytesPerFont = (u16)GUI_UNI_PTR.h_px * (u16)GUI_UNI_PTR.get_width; 
    if (BytesPerFont > BYTES_PER_FONT)
	{
		BytesPerFont = BYTES_PER_FONT;
	}

	/* 汉字和全角字符的偏移地址计算 */
		if(strncmp("H16", font, 3) == 0)  /* 16*16 字符 */
		{
			BaseAdd = 0x0020A000;
		}
		else if(strncmp("H24", font, 3) == 0)  /* 24*24 字符 */
		{
			BaseAdd = 0x002C6000;
		}
		else if(strncmp("H32", font, 3) == 0)  /* 32*32 字符 */
		{
			BaseAdd = 0x0046B000;
		}
		else if(strncmp("H40", font, 3) == 0)  /* 40*40 字符 */
		{
			BaseAdd = 0x00758000;
		}
		else if(strncmp("H48", font, 3) == 0)  /* 48*48 字符 */
		{
			BaseAdd = 0x00BE9000;
            FontType = 1;
		}
        
        if (FontType == 0)
        {
            /* 根据汉字内码的计算公式锁定起始地址 */
            code2 = *c ;
            code1 = *(++c) ;
            
            /* 由于字符编码是安顺序存储的，先存储到高位（区号），然后是低位（位号）。而我们用的是小端格式，
               一个汉字两个字节，获取的16位变量，正好相反，16位变量的高位是位号，低位是区号。
            */
            oft = ((code1 - 0x81) * 190 + (code2 - 0x40) - (code2 / 128))* BytesPerFont + BaseAdd;
        }
        else
        {
            /* 根据汉字内码的计算公式锁定起始地址 */
            code2 = *c ;
            code1 = *(++c) ;
            
            /* 由于字符编码是安顺序存储的，先存储到高位（区号），然后是低位（位号）。而我们用的是小端格式，
               一个汉字两个字节，获取的16位变量，正好相反，16位变量的高位是位号，低位是区号。
            */
            oft = ((code1 - 0xA1) * 94 + (code2 - 0xa1))* BytesPerFont + BaseAdd;
        }
        
    
	
    rt_spi_flash_device_t flash_device;
    
    flash_device = (rt_spi_flash_device_t)rt_device_find("w25q128");
    if (flash_device == RT_NULL)
    {
#ifdef RT_DEBUG
        rt_kprintf("can`t find device w25q128!");
#endif
        return ;
    }
    
	/* 读取点阵数据 */
    
    flash_device->flash_device.read(&flash_device->flash_device, oft, GUI_FontDataBuf, BytesPerFont);
    
	//sf_ReadBuffer(GUI_FontDataBuf, oft, BytesPerFont);
} 









