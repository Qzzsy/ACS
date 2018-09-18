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
 * @brief 	��ȡ��������
 * @param	pProp GUI_FONT_PROP���ͽṹ
 * @param	c �ַ�
 * @note  	
 * @retval	��
 */
static void GUI_GetDataFromMemory(const lv_font_t GUI_UNI_PTR, char *c) 
{ 
    u16 BytesPerFont; 
    u32 oft = 0, BaseAdd; 
	u8 code1, code2, FontType = 0;
	
	char *font = (char *)GUI_UNI_PTR.glyph_bitmap; 

	/* ÿ����ģ�������ֽ��� */
    BytesPerFont = (u16)GUI_UNI_PTR.h_px * (u16)GUI_UNI_PTR.get_width; 
    if (BytesPerFont > BYTES_PER_FONT)
	{
		BytesPerFont = BYTES_PER_FONT;
	}

	/* ���ֺ�ȫ���ַ���ƫ�Ƶ�ַ���� */
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
            FontType = 1;
		}
        
        if (FontType == 0)
        {
            /* ���ݺ�������ļ��㹫ʽ������ʼ��ַ */
            code2 = *c ;
            code1 = *(++c) ;
            
            /* �����ַ������ǰ�˳��洢�ģ��ȴ洢����λ�����ţ���Ȼ���ǵ�λ��λ�ţ����������õ���С�˸�ʽ��
               һ�����������ֽڣ���ȡ��16λ�����������෴��16λ�����ĸ�λ��λ�ţ���λ�����š�
            */
            oft = ((code1 - 0x81) * 190 + (code2 - 0x40) - (code2 / 128))* BytesPerFont + BaseAdd;
        }
        else
        {
            /* ���ݺ�������ļ��㹫ʽ������ʼ��ַ */
            code2 = *c ;
            code1 = *(++c) ;
            
            /* �����ַ������ǰ�˳��洢�ģ��ȴ洢����λ�����ţ���Ȼ���ǵ�λ��λ�ţ����������õ���С�˸�ʽ��
               һ�����������ֽڣ���ȡ��16λ�����������෴��16λ�����ĸ�λ��λ�ţ���λ�����š�
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
    
	/* ��ȡ�������� */
    
    flash_device->flash_device.read(&flash_device->flash_device, oft, GUI_FontDataBuf, BytesPerFont);
    
	//sf_ReadBuffer(GUI_FontDataBuf, oft, BytesPerFont);
} 









