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
#include "GUI_private.h"
#include "stm32f4xx.h"
#include "rtthread.h"
#include "gui.h"
#include "string.h"
#include "SPI_flash.h" 


/* Private macro Definition --------------------------------------------------*/
/* 点阵数据缓存, 必须大于等于单个字模需要的存储空间*/ 
#define BYTES_PER_FONT      512 

/* global variable Declaration -----------------------------------------------*/

static U8 GUI_FontDataBuf[BYTES_PER_FONT];

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
static void GUI_GetDataFromMemory(const GUI_FONT_PROP GUI_UNI_PTR *pProp, U16P c) 
{ 
    U16 BytesPerFont; 
    U32 oft = 0, BaseAdd; 
	U8 code1, code2, FontType = false;
	
	char *font = (char *)pProp->paCharInfo->pData; 

	/* 每个字模的数据字节数 */
    BytesPerFont = GUI_pContext->pAFont->YSize * pProp->paCharInfo->BytesPerLine; 
    if (BytesPerFont > BYTES_PER_FONT)
	{
		BytesPerFont = BYTES_PER_FONT;
	}

	/* 英文字符地址偏移计算 */ 
    if (c < 0x80)                                                                
    { 
		if(strncmp("A16", font, 3) == 0) /* 8*16 ASCII字符 */
		{
			BaseAdd = 0x00200000;
		}
		else if(strncmp("A24", font, 3) == 0) /* 12*24 ASCII字符 */
		{
			BaseAdd = 0x00200600;
		}
		else if(strncmp("A32", font, 3) == 0) /* 16*32 ASCII字符 */
		{
			BaseAdd = 0x00201800;
		}
		else if(strncmp("A40", font, 3) == 0) /* 20*40 ASCII字符 */
		{
			BaseAdd = 0x00203000;
		}
		else if(strncmp("A48", font, 3) == 0) /* 24*48 ASCII字符 */
		{
			BaseAdd = 0x00205d00;
		}
		if (c >= 32)
        {
            oft = (c - 32) * BytesPerFont + BaseAdd; 
        }
        else
        {
            return ;
        }
    } 
	/* 汉字和全角字符的偏移地址计算 */
    else                                                                           
    { 
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
            FontType = true;
		}
        
        if (FontType == false)
        {
            /* 根据汉字内码的计算公式锁定起始地址 */
            code2 = c >> 8;
            code1 = c & 0xFF;
            
            /* 由于字符编码是安顺序存储的，先存储到高位（区号），然后是低位（位号）。而我们用的是小端格式，
               一个汉字两个字节，获取的16位变量，正好相反，16位变量的高位是位号，低位是区号。
            */
            oft = ((code1 - 0x81) * 190 + (code2 - 0x40) - (code2 / 128))* BytesPerFont + BaseAdd;
        }
        else
        {
            /* 根据汉字内码的计算公式锁定起始地址 */
            code2 = c >> 8;
            code1 = c & 0xFF;
            
            /* 由于字符编码是安顺序存储的，先存储到高位（区号），然后是低位（位号）。而我们用的是小端格式，
               一个汉字两个字节，获取的16位变量，正好相反，16位变量的高位是位号，低位是区号。
            */
            oft = ((code1 - 0xA1) * 94 + (code2 - 0xa1))* BytesPerFont + BaseAdd;
        }
        
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

/**
 * @func	GUIUSER_DispChar
 * @brief 	显示字符
 * @param	c 显示的字符
 * @note  	
 * @retval	无
 */
void GUIUSER_DispChar(U16P c)  
{ 
    int BytesPerLine; 
    GUI_DRAWMODE DrawMode = GUI_pContext->TextMode; 
    const GUI_FONT_PROP GUI_UNI_PTR *pProp = GUI_pContext->pAFont->p.pProp; 
	
    /* 确定显示的字符是否在有效范围内 */  
    for (; pProp; pProp = pProp->pNext)                                          
    { 
        if ((c >= pProp->First) && (c <= pProp->Last))break; 
    } 
	
	/* 字符有效，进行显示 */
    if (pProp) 
    { 
        GUI_DRAWMODE OldDrawMode;
        const GUI_CHARINFO GUI_UNI_PTR * pCharInfo = pProp->paCharInfo;
        GUI_GetDataFromMemory(pProp, c);
        BytesPerLine = pCharInfo->BytesPerLine;                
        OldDrawMode  = LCD_SetDrawMode(DrawMode);
        LCD_DrawBitmap(GUI_pContext->DispPosX, GUI_pContext->DispPosY,
                       pCharInfo->XSize, GUI_pContext->pAFont->YSize,
                       GUI_pContext->pAFont->XMag, GUI_pContext->pAFont->YMag,
                       1,    
                       BytesPerLine,
                       &GUI_FontDataBuf[0],
                       &LCD_BKCOLORINDEX
                       );
		
        /* 填充背景 */
        if (GUI_pContext->pAFont->YDist > GUI_pContext->pAFont->YSize) 
        {
            int YMag = GUI_pContext->pAFont->YMag;
            int YDist = GUI_pContext->pAFont->YDist * YMag;
            int YSize = GUI_pContext->pAFont->YSize * YMag;
            if (DrawMode != LCD_DRAWMODE_TRANS) 
            {
                LCD_COLOR OldColor = GUI_GetColor();
                GUI_SetColor(GUI_GetBkColor());
                LCD_FillRect(GUI_pContext->DispPosX, GUI_pContext->DispPosY + YSize, 
                             GUI_pContext->DispPosX + pCharInfo->XSize, 
                             GUI_pContext->DispPosY + YDist);
                GUI_SetColor(OldColor);
            }
        }
		
        LCD_SetDrawMode(OldDrawMode);
//      if (!GUI_MoveRTL)
        GUI_pContext->DispPosX += pCharInfo->XDist * GUI_pContext->pAFont->XMag;
    }
} 

/**
 * @func	GUIUSER_GetCharDistX
 * @brief 	获取字符的X轴间距
 * @param	c 字符
 * @note  	
 * @retval	无
 */
int GUIUSER_GetCharDistX(U16P c, int * pSizeX) 
{
    const GUI_FONT_PROP GUI_UNI_PTR * pProp = GUI_pContext->pAFont->p.pProp;  
    for (; pProp; pProp = pProp->pNext)                                         
    {
        if ((c >= pProp->First) && (c <= pProp->Last))break;
    }
    return (pProp) ? (pProp->paCharInfo)->XSize * GUI_pContext->pAFont->XMag : 0;
}







