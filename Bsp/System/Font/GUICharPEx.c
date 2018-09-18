/**
 ******************************************************************************
 * @file		GUICharPEx.c
 * @author		�Ž�����С��
 * @version		V1.0.0
 * @date		2018-01-20
 * @brief       ���ֺ�����ʾ��ʽ�ʺ�ʹ�õȿ����壬�ǵȿ��΢���źڵ�������ʾЧ���ϲ
 *              ֧�ֵ���
 *                1. ֧��12����16����24����32����40�����Լ�48�����֡�
 *                2. 8X16�����ASCII��12X24�����ASCII��16X32�����ASCII��20X40�����ASCII��24X48�����ASCII��
 *              �ֿ�洢��ʽ��
 *                1. �ֿ�洢��SPI FLASH���档
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
/* �������ݻ���, ������ڵ��ڵ�����ģ��Ҫ�Ĵ洢�ռ�*/ 
#define BYTES_PER_FONT      512 

/* global variable Declaration -----------------------------------------------*/

static U8 GUI_FontDataBuf[BYTES_PER_FONT];

/* User function Declaration -------------------------------------------------*/

/* User functions ------------------------------------------------------------*/
/**
 * @func	GUI_GetDataFromMemory
 * @brief 	��ȡ��������
 * @param	pProp GUI_FONT_PROP���ͽṹ
 * @param	c �ַ�
 * @note  	
 * @retval	��
 */
static void GUI_GetDataFromMemory(const GUI_FONT_PROP GUI_UNI_PTR *pProp, U16P c) 
{ 
    U16 BytesPerFont; 
    U32 oft = 0, BaseAdd; 
	U8 code1, code2, FontType = false;
	
	char *font = (char *)pProp->paCharInfo->pData; 

	/* ÿ����ģ�������ֽ��� */
    BytesPerFont = GUI_pContext->pAFont->YSize * pProp->paCharInfo->BytesPerLine; 
    if (BytesPerFont > BYTES_PER_FONT)
	{
		BytesPerFont = BYTES_PER_FONT;
	}

	/* Ӣ���ַ���ַƫ�Ƽ��� */ 
    if (c < 0x80)                                                                
    { 
		if(strncmp("A16", font, 3) == 0) /* 8*16 ASCII�ַ� */
		{
			BaseAdd = 0x00200000;
		}
		else if(strncmp("A24", font, 3) == 0) /* 12*24 ASCII�ַ� */
		{
			BaseAdd = 0x00200600;
		}
		else if(strncmp("A32", font, 3) == 0) /* 16*32 ASCII�ַ� */
		{
			BaseAdd = 0x00201800;
		}
		else if(strncmp("A40", font, 3) == 0) /* 20*40 ASCII�ַ� */
		{
			BaseAdd = 0x00203000;
		}
		else if(strncmp("A48", font, 3) == 0) /* 24*48 ASCII�ַ� */
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
	/* ���ֺ�ȫ���ַ���ƫ�Ƶ�ַ���� */
    else                                                                           
    { 
		if(strncmp("H16", font, 3) == 0)  /* 16*16 �ַ� */
		{
			BaseAdd = 0x0020A000;
		}
		else if(strncmp("H24", font, 3) == 0)  /* 24*24 �ַ� */
		{
			BaseAdd = 0x002C6000;
		}
		else if(strncmp("H32", font, 3) == 0)  /* 32*32 �ַ� */
		{
			BaseAdd = 0x0046B000;
		}
		else if(strncmp("H40", font, 3) == 0)  /* 40*40 �ַ� */
		{
			BaseAdd = 0x00758000;
		}
		else if(strncmp("H48", font, 3) == 0)  /* 48*48 �ַ� */
		{
			BaseAdd = 0x00BE9000;
            FontType = true;
		}
        
        if (FontType == false)
        {
            /* ���ݺ�������ļ��㹫ʽ������ʼ��ַ */
            code2 = c >> 8;
            code1 = c & 0xFF;
            
            /* �����ַ������ǰ�˳��洢�ģ��ȴ洢����λ�����ţ���Ȼ���ǵ�λ��λ�ţ����������õ���С�˸�ʽ��
               һ�����������ֽڣ���ȡ��16λ�����������෴��16λ�����ĸ�λ��λ�ţ���λ�����š�
            */
            oft = ((code1 - 0x81) * 190 + (code2 - 0x40) - (code2 / 128))* BytesPerFont + BaseAdd;
        }
        else
        {
            /* ���ݺ�������ļ��㹫ʽ������ʼ��ַ */
            code2 = c >> 8;
            code1 = c & 0xFF;
            
            /* �����ַ������ǰ�˳��洢�ģ��ȴ洢����λ�����ţ���Ȼ���ǵ�λ��λ�ţ����������õ���С�˸�ʽ��
               һ�����������ֽڣ���ȡ��16λ�����������෴��16λ�����ĸ�λ��λ�ţ���λ�����š�
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
    
	/* ��ȡ�������� */
    
    flash_device->flash_device.read(&flash_device->flash_device, oft, GUI_FontDataBuf, BytesPerFont);
    
	//sf_ReadBuffer(GUI_FontDataBuf, oft, BytesPerFont);
} 

/**
 * @func	GUIUSER_DispChar
 * @brief 	��ʾ�ַ�
 * @param	c ��ʾ���ַ�
 * @note  	
 * @retval	��
 */
void GUIUSER_DispChar(U16P c)  
{ 
    int BytesPerLine; 
    GUI_DRAWMODE DrawMode = GUI_pContext->TextMode; 
    const GUI_FONT_PROP GUI_UNI_PTR *pProp = GUI_pContext->pAFont->p.pProp; 
	
    /* ȷ����ʾ���ַ��Ƿ�����Ч��Χ�� */  
    for (; pProp; pProp = pProp->pNext)                                          
    { 
        if ((c >= pProp->First) && (c <= pProp->Last))break; 
    } 
	
	/* �ַ���Ч��������ʾ */
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
		
        /* ��䱳�� */
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
 * @brief 	��ȡ�ַ���X����
 * @param	c �ַ�
 * @note  	
 * @retval	��
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







