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
#include "HZCharPEx.h"
#include "font.h"

#include "stm32f4xx.h" // Device header

#define FONT_ERROR           (uint8_t)(-1)
#define FONT_OK              0x00
#define FONT_NO_FIND_FONT    0x01


#if defined USE_CN_INT_LIB || defined USE_ASCII_INT_LIB
/* �ֿ�λ���ڲ���FLASH */
#define USE_SMALL_LIB_FONT              (1)     //1Ϊʹ���ڲ��ֿ�
#endif

#ifdef USE_CN_EXT_LIB
/* �ֿ�λ���ⲿ��FLASH */
#define USE_GBK_LIB_FONT                (0)     //1Ϊʹ��GBK�ֿ�
/* �ֿ�λ���ⲿ��FLASH */
#define USE_GB2312_LIB_FONT             (0)     //1Ϊʹ��GB2312�ֿ�
#endif

#if USE_SMALL_LIB_FONT == 1
/* �����ֿ�������� */
#define ChAR_NUM_MAX          200
#endif

/* Ӣ�ĵ��ֿ�ĵ�ַ��������ڣ��뽫��ע�͵���������ִ��� */
#ifdef USE_ASCII_EXT_LIB
/* Ӣ���ֿ����ַ */
#define FONT_ASCII16_BASE_ADDR                 0x20000000
#define FONT_ASCII24_BASE_ADDR                 0x20000000
#define FONT_ASCII32_BASE_ADDR                 0x20000000
#define FONT_ASCII40_BASE_ADDR                 0x20000000
#define FONT_ASCII48_BASE_ADDR                 0x20000000
#endif

/* ���ֵ��ֿ�ĵ�ַ��������ڣ��뽫��ע�͵���������ִ��� */
#if USE_GBK_LIB_FONT == 1

/* �����ֿ����ַ */
#define GBK_FONT_CN16_BASE_ADDR                 0x20000000
#define GBK_FONT_CN24_BASE_ADDR                 0x20000000
#define GBK_FONT_CN32_BASE_ADDR                 0x20000000
#define GBK_FONT_CN40_BASE_ADDR                 0x20000000
#define GBK_FONT_CN48_BASE_ADDR                 0x20000000
#endif

#if USE_GB2312_LIB_FONT == 1
/* �����ֿ����ַ */
#define GB2312_FONT_CN16_BASE_ADDR                 0x20000000
#define GB2312_FONT_CN24_BASE_ADDR                 0x20000000
#define GB2312_FONT_CN32_BASE_ADDR                 0x20000000
#define GB2312_FONT_CN40_BASE_ADDR                 0x20000000
#define GB2312_FONT_CN48_BASE_ADDR                 0x20000000
#endif

uint8_t lv_FontDataBuf[BYTES_PER_FONT];

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
static void GUI_GetDataFromMemory(const lv_font_t * GUI_UNI_PTR, char *c)
{
    u16 BytesPerFont;
    u32 oft = 0, BaseAdd;
    u8 code1, code2, FontType = 0;

    char *font = c;

    /* ÿ����ģ�������ֽ��� */
    if (BytesPerFont > BYTES_PER_FONT)
    {
        BytesPerFont = BYTES_PER_FONT;
    }

    /* ���ֺ�ȫ���ַ���ƫ�Ƶ�ַ���� */
    if (strncmp("H16", font, 3) == 0) /* 16*16 �ַ� */
    {
        BaseAdd = 0x0020A000;
    }
    else if (strncmp("H24", font, 3) == 0) /* 24*24 �ַ� */
    {
        BaseAdd = 0x002C6000;
    }
    else if (strncmp("H32", font, 3) == 0) /* 32*32 �ַ� */
    {
        BaseAdd = 0x0046B000;
    }
    else if (strncmp("H40", font, 3) == 0) /* 40*40 �ַ� */
    {
        BaseAdd = 0x00758000;
    }
    else if (strncmp("H48", font, 3) == 0) /* 48*48 �ַ� */
    {
        BaseAdd = 0x00BE9000;
        FontType = 1;
    }

    if (FontType == 0)
    {
        /* ���ݺ�������ļ��㹫ʽ������ʼ��ַ */
        code2 = *c;
        code1 = *(++c);

        /* �����ַ������ǰ�˳��洢�ģ��ȴ洢����λ�����ţ���Ȼ���ǵ�λ��λ�ţ����������õ���С�˸�ʽ��
               һ�����������ֽڣ���ȡ��16λ�����������෴��16λ�����ĸ�λ��λ�ţ���λ�����š�
            */
        oft = ((code1 - 0x81) * 190 + (code2 - 0x40) - (code2 / 128)) * BytesPerFont + BaseAdd;
    }
    else
    {
        /* ���ݺ�������ļ��㹫ʽ������ʼ��ַ */
        code2 = *c;
        code1 = *(++c);

        /* �����ַ������ǰ�˳��洢�ģ��ȴ洢����λ�����ţ���Ȼ���ǵ�λ��λ�ţ����������õ���С�˸�ʽ��
               һ�����������ֽڣ���ȡ��16λ�����������෴��16λ�����ĸ�λ��λ�ţ���λ�����š�
            */
        oft = ((code1 - 0xA1) * 94 + (code2 - 0xa1)) * BytesPerFont + BaseAdd;
    }

    rt_spi_flash_device_t flash_device;

    flash_device = (rt_spi_flash_device_t)rt_device_find("w25q128");
    if (flash_device == RT_NULL)
    {
#ifdef RT_DEBUG
        rt_kprintf("can`t find device w25q128!");
#endif
        return;
    }

    /* ��ȡ�������� */

    flash_device->flash_device.read(&flash_device->flash_device, oft, lv_FontDataBuf, BytesPerFont);

    //sf_ReadBuffer(GUI_FontDataBuf, oft, BytesPerFont);
}

///**
// * @func    _GetASCII_FontData
// * @brief   ���ڴ����ȡ���������
// * @param   GUI_CnInfo ��ʾ���ֵ���Ϣ�Ľṹ��
// * @note
// * @retval  ��
// */
//static inline void _GetASCII_FontData(GUI_CnInfo_t * GUI_CnInfo)
//{
//    uint8_t WordNun; 

//    GUI_CnInfo->ERROR_CODE = GUI_OK;

//#if defined USE_ASCII_EXT_LIB || defined USE_CN_EXT_LIB
//    uint32_t FlashAddr = 0;
//#endif
///* ���β���û����ʾ��ASCII�� */
//    if (GUI_CnInfo->Cn < 32)
//    {
//        GUI_CnInfo->ERROR_CODE = GUI_ERROR;
//        return ;
//    }
//    
//    /* ��ASCII���32��ʼ����ʾ�����Դ˴�����32 */
//    WordNun = GUI_CnInfo->Cn - 32;
//#ifdef USE_ASCII_INT_LIB        
//    /* ʹ��16�ĵ��� */
//#ifdef USING_CN_16_CHAR
//    if (my_strncmp("A16", GUI_CnInfo->paCnInfo.Char, 3) == 0) /* 8*16 ASCII�ַ� */
//    {
//        /* ָ��ֱ��ȡ��ַ */
//        GUI_CnInfo->FontDataBuf = (uint8_t *)&ASCII08x16[(uint16_t )WordNun * GUI_CnInfo->SumBytes];
//        return ;
//    }
//#endif /* USING_CN_16_CHAR */   
//    
//    /* ʹ��24�ĵ��� */
//#ifdef USING_CN_24_CHAR  
//    if (my_strncmp("A24", GUI_CnInfo->paCnInfo.Char, 3) == 0) /* 16*24 ASCII�ַ� */
//    {          
//        GUI_CnInfo->FontDataBuf = (uint8_t *)&ASCII12x24[(uint16_t )WordNun * GUI_CnInfo->SumBytes];
//        return ;
//    }
//#endif /* USING_CN_24_CHAR */
//    
//    /* ʹ��32�ĵ��� */
//#ifdef USING_CN_32_CHAR  
//    if (my_strncmp("A32", GUI_CnInfo->paCnInfo.Char, 3) == 0) /* 20*32 ASCII�ַ� */
//    {  
//        GUI_CnInfo->FontDataBuf = (uint8_t *)&ASCII16x32[(uint16_t )WordNun * GUI_CnInfo->SumBytes];
//        return ;
//    }
//#endif /* USING_CN_32_CHAR */
//    
//    /* ʹ��40�ĵ��� */
//#ifdef USING_CN_40_CHAR  
//    if (my_strncmp("A40", GUI_CnInfo->paCnInfo.Char, 3) == 0) /* 24*40 ASCII�ַ� */
//    {  
//        GUI_CnInfo->FontDataBuf = (uint8_t *)&ASCII20x40[(uint16_t )WordNun * GUI_CnInfo->SumBytes];
//        return ;
//    }
//#endif /* USING_CN_40_CHAR */
//    
//    /* ʹ��48�ĵ��� */
//#ifdef USING_CN_48_CHAR   
//    if (my_strncmp("A48", GUI_CnInfo->paCnInfo.Char, 3) == 0) /* 28*48 ASCII�ַ� */
//    { 
//        GUI_CnInfo->FontDataBuf = (uint8_t *)&ASCII24x48[(uint16_t )WordNun * GUI_CnInfo->SumBytes];
//        return ;
//    }
//#endif /* USING_CN_48_CHAR */
//#elif defined USE_ASCII_EXT_LIB
//    /* ʹ��16�ĵ��� */
//#ifdef USING_CN_16_CHAR
//    if (my_strncmp("A16", GUI_CnInfo->paCnInfo.Char, 3) == 0) /* 8*16 ASCII�ַ� */
//    {
//#ifdef FONT_ASCII16_BASE_ADDR
//        /* �����ַ */
//        FlashAddr = FONT_ASCII16_BASE_ADDR + (uint16_t)WordNun * GUI_CnInfo->SumBytes;
//        goto _ReadASCII_Data; 
//#else
//        GUI_CnInfo->ERROR_CODE = GUI_NO_FIND_FONT;
//        return ;
//#endif
//    }
//#endif /* USING_CN_16_CHAR */   
//    
//    /* ʹ��24�ĵ��� */
//#ifdef USING_CN_24_CHAR  
//    if (my_strncmp("A24", GUI_CnInfo->paCnInfo.Char, 3) == 0) /* 16*24 ASCII�ַ� */
//    {          
//#ifdef FONT_ASCII24_BASE_ADDR
//        FlashAddr = FONT_ASCII24_BASE_ADDR + (uint16_t)WordNun * GUI_CnInfo->SumBytes;
//        goto _ReadASCII_Data;
//#else
//        GUI_CnInfo->ERROR_CODE = GUI_NO_FIND_FONT;
//        return ;
//#endif
//    }
//#endif /* USING_CN_24_CHAR */
//    
//    /* ʹ��32�ĵ��� */
//#ifdef USING_CN_32_CHAR  
//    if (my_strncmp("A32", GUI_CnInfo->paCnInfo.Char, 3) == 0) /* 20*32 ASCII�ַ� */
//    {  
//#ifdef FONT_ASCII32_BASE_ADDR
//        FlashAddr = FONT_ASCII32_BASE_ADDR + (uint16_t)WordNun * GUI_CnInfo->SumBytes;
//        goto _ReadASCII_Data;
//#else
//        GUI_CnInfo->ERROR_CODE = GUI_NO_FIND_FONT;
//        return ;
//#endif
//    }
//#endif /* USING_CN_32_CHAR */
//    
//    /* ʹ��40�ĵ��� */
//#ifdef USING_CN_40_CHAR  
//    if (my_strncmp("A40", GUI_CnInfo->paCnInfo.Char, 3) == 0) /* 24*40 ASCII�ַ� */
//    {  
//#ifdef FONT_ASCII40_BASE_ADDR
//        FlashAddr = FONT_ASCII40_BASE_ADDR + (uint16_t)WordNun * GUI_CnInfo->SumBytes;
//        goto _ReadASCII_Data;
//#else
//        GUI_CnInfo->ERROR_CODE = GUI_NO_FIND_FONT;
//        return ;
//#endif
//    }
//#endif /* USING_CN_40_CHAR */
//    
//    /* ʹ��48�ĵ��� */
//#ifdef USING_CN_48_CHAR   
//    if (my_strncmp("A48", GUI_CnInfo->paCnInfo.Char, 3) == 0) /* 28*48 ASCII�ַ� */
//    { 
//#ifdef FONT_ASCII48_BASE_ADDR
//        FlashAddr = FONT_ASCII48_BASE_ADDR + (uint16_t)WordNun * GUI_CnInfo->SumBytes;
//        goto _ReadASCII_Data;
//#else
//        GUI_CnInfo->ERROR_CODE = GUI_NO_FIND_FONT;
//        return ;
//#endif
//    }
//#endif /* USING_CN_48_CHAR */
//_ReadASCII_Data:
//    _GUI_DeviceAPI.ReadData(FlashAddr, _GUI_FontDataBufFromFlash, GUI_CnInfo->SumBytes);
//    GUI_CnInfo->FontDataBuf = _GUI_FontDataBufFromFlash;
//    return ;
//#endif
//    GUI_CnInfo->ERROR_CODE = GUI_ERROR;
//    return ;
//}

/**
 * @func    _GetCN_FontData
 * @brief   ���ڴ����ȡ���������
 * @param   GUI_CnInfo ��ʾ���ֵ���Ϣ�Ľṹ��
 * @note
 * @retval  ��
 */
static inline uint8_t _GetCN_FontDataFromMem(const lv_font_t *font, uint32_t CnCode)
{
    uint16_t i = 0;
    paCharsInfo_t * paCharsInfo = (paCharsInfo_t *)font->glyph_dsc;
#if defined USE_ASCII_EXT_LIB || defined USE_CN_EXT_LIB
    uint32_t FlashAddr = 0;
    uint8_t FontType = NONE_FONT;
#endif
#ifdef USE_CN_INT_LIB
#ifdef USING_CN_16_CHAR  
    if(strncmp("H16", paCharsInfo->paHanziInfo.Char, 3) == 0) /* 16*16 �����ַ� */
    { 
        for (i = 0; i < ChAR_NUM_MAX; i++)        //ѭ����ѯ���룬���Һ��ֵ�����
        {
            if((HanZi16Index[i].Index[0] == ((CnCode >> 8) & 0xff))		
                & (HanZi16Index[i].Index[1] == (CnCode & 0xff)))
            {
                font->get_bitmap = (uint8_t *)HanZi16Data[i].Msk;
                return -1;
            }
        }
        
        goto _ERROR;
    }
#endif /* USING_CN_16_CHAR */
#ifdef USING_CN_24_CHAR          
    if (strncmp("H24", paCharsInfo->paHanziInfo.Char, 3) == 0) /* 24*24 �����ַ� */
    {
        for (i = 0; i < ChAR_NUM_MAX; i++)
        {
            if((HanZi24Index[i].Index[0] == ((CnCode >> 8) & 0xff))		
                & (HanZi24Index[i].Index[1] == (CnCode & 0xff)))
            {
                font->get_bitmap = (uint8_t *)HanZi24Data[i].Msk;
                return ;
            }
        }
        
        goto _ERROR;
    }
#endif /* USING_CN_24_CHAR */
#ifdef USING_CN_32_CHAR  
    if (strncmp("H32", paCharsInfo->paHanziInfo.Char, 3) == 0) /* 32*32 �����ַ� */
    {
        for (i = 0; i < ChAR_NUM_MAX; i++)
        {
            if((HanZi32Index[i].Index[0] == ((CnCode >> 8) & 0xff))		
                & (HanZi32Index[i].Index[1] == (CnCode & 0xff)))
            {
                font->get_bitmap = (uint8_t *)HanZi32Data[i].Msk;
                return ;
            }
        }
        
        goto _ERROR;
    }
#endif /* USING_CN_32_CHAR */
#ifdef USING_CN_40_CHAR  
    if (strncmp("H40", paCharsInfo->paHanziInfo.Char, 3) == 0) /* 40*40 �����ַ� */
    {
        for (i = 0; i < ChAR_NUM_MAX; i++)
        {
            if((HanZi40Index[i].Index[0] == ((CnCode >> 8) & 0xff))		
                & (HanZi40Index[i].Index[1] == (CnCode & 0xff)))
            {
                font->get_bitmap = (uint8_t *)HanZi40Data[i].Msk;
                return ;
            }
        }
        
        goto _ERROR;
    }
#endif /* USING_CN_40_CHAR */
#ifdef USING_CN_48_CHAR  
    if (strncmp("H48", paCharsInfo->paHanziInfo.Char, 3) == 0) /* 48*48 �����ַ� */
    {
        for (i = 0; i < ChAR_NUM_MAX; i++)
        {
            if((HanZi48Index[i].Index[0] == ((CnCode >> 8) & 0xff))		
                & (HanZi48Index[i].Index[1] == (CnCode & 0xff)))
            {
                font->get_bitmap = (uint8_t *)HanZi48Data[i].Msk;
                return ;
            }
        }
        
        goto _ERROR;
    }
#endif /* USING_CN_48_CHAR */
#elif defined USE_CN_EXT_LIB
#ifdef USING_CN_16_CHAR  
    if(my_strncmp("H16", paCharsInfo->paHanziInfo.Char, 3) == 0) /* 16*16 �����ַ� */
    { 
#ifdef GBK_FONT_CN16_BASE_ADDR
        FlashAddr = GBK_FONT_CN16_BASE_ADDR;
        FontType = GBK_FONT;
        goto _ReadCN_Data;
#elif define GB2312_FONT_CN16_BASE_ADDR
        FlashAddr = GB2312_FONT_CN16_BASE_ADDR;
        FontType = GB2312_FONT;
        goto _ReadCN_Data;
#else
        goto _ERROR;
#endif
    }
#endif /* USING_CN_16_CHAR */
#ifdef USING_CN_24_CHAR          
    if (my_strncmp("H24", paCharsInfo->paHanziInfo.Char, 3) == 0) /* 24*24 �����ַ� */
    {
#ifdef GBK_FONT_CN24_BASE_ADDR
        FlashAddr = GBK_FONT_CN24_BASE_ADDR;
        FontType = GBK_FONT;
        goto _ReadCN_Data;
#elif define GB2312_FONT_CN24_BASE_ADDR
        FlashAddr = GB2312_FONT_CN24_BASE_ADDR;
        FontType = GB2312_FONT;
        goto _ReadCN_Data;
#else
        goto _ERROR;
#endif
    }
#endif /* USING_CN_24_CHAR */
#ifdef USING_CN_32_CHAR  
    if (my_strncmp("H32", paCharsInfo->paHanziInfo.Char, 3) == 0) /* 32*32 �����ַ� */
    {
#ifdef GBK_FONT_CN32_BASE_ADDR
        FlashAddr = GBK_FONT_CN32_BASE_ADDR;
        FontType = GBK_FONT;
        goto _ReadCN_Data;
#elif define GB2312_FONT_CN32_BASE_ADDR
        FlashAddr = GB2312_FONT_CN32_BASE_ADDR;
        FontType = GB2312_FONT;
        goto _ReadCN_Data;
#else
        goto _ERROR;
#endif
    }
#endif /* USING_CN_32_CHAR */
#ifdef USING_CN_40_CHAR  
    if (my_strncmp("H40", paCharsInfo->paHanziInfo.Char, 3) == 0) /* 40*40 �����ַ� */
    {
#ifdef GBK_FONT_CN40_BASE_ADDR
        FlashAddr = GBK_FONT_CN40_BASE_ADDR;
        FontType = GBK_FONT;
        goto _ReadCN_Data;
#elif define GB2312_FONT_CN40_BASE_ADDR
        FlashAddr = GB2312_FONT_CN40_BASE_ADDR;
        FontType = GB2312_FONT;
        goto _ReadCN_Data;
#else
        goto _ERROR;
#endif
    }
#endif /* USING_CN_40_CHAR */
#ifdef USING_CN_48_CHAR  
    if (my_strncmp("H48", paCharsInfo->paHanziInfo.Char, 3) == 0) /* 48*48 �����ַ� */
    {
#ifdef GBK_FONT_CN48_BASE_ADDR
        FlashAddr = GBK_FONT_CN48_BASE_ADDR;
        FontType = GBK_FONT;
        goto _ReadCN_Data;
#elif define GB2312_FONT_CN48_BASE_ADDR
        FlashAddr = GB2312_FONT_CN48_BASE_ADDR;
        FontType = GB2312_FONT;
        goto _ReadCN_Data;
#else
        goto _ERROR;
#endif
    }
#endif /* USING_CN_48_CHAR */
_ReadCN_Data:
    if (FontType == GBK_FONT)
    {
        /* ���ݺ�������ļ��㹫ʽ������ʼ��ַ */
        code2 = c >> 8;
        code1 = c & 0xFF;
        
        /* �����ַ������ǰ�˳��洢�ģ��ȴ洢����λ�����ţ���Ȼ���ǵ�λ��λ�ţ����������õ���С�˸�ʽ��
            һ�����������ֽڣ���ȡ��16λ�����������෴��16λ�����ĸ�λ��λ�ţ���λ�����š�
        */
        FlashAddr = ((code1 - 0xA1) * 94 + (code2 - 0xa1)) * GUI_CnInfo->SumBytes + FlashAddr;
    }
    else if (FontType == GB2312_FONT)
    {
        /* ���ݺ�������ļ��㹫ʽ������ʼ��ַ */
        code2 = c >> 8;
        code1 = c & 0xFF;
        
        /* �����ַ������ǰ�˳��洢�ģ��ȴ洢����λ�����ţ���Ȼ���ǵ�λ��λ�ţ����������õ���С�˸�ʽ��
            һ�����������ֽڣ���ȡ��16λ�����������෴��16λ�����ĸ�λ��λ�ţ���λ�����š�
        */
        FlashAddr = ((code1 - 0x81) * 190 + (code2 - 0x40) - (code2 / 128)) * GUI_CnInfo->SumBytes + FlashAddr;
    }

    _GUI_DeviceAPI.ReadData(FlashAddr, _GUI_FontDataBufFromFlash, GUI_CnInfo->SumBytes);
    GUI_CnInfo->FontDataBuf = _GUI_FontDataBufFromFlash;
#endif
_ERROR:
    return FONT_ERROR;
}

const uint8_t *lv_hzPEx_font_get_bitmap_continuous(const lv_font_t *font, uint32_t CnCode)
{
    /* �ж������ĵĻ���Ӣ�ĵ� */    
    if (CnCode < 0x80)                                                                
    {
        //_GetASCII_FontData(font);
    }
    /* ������ʾ */
    else
    {
        /*Check the range*/
        if(CnCode < font->unicode_first || CnCode > font->unicode_last)
        {
            return -1;
        }
        _GetCN_FontDataFromMem(font, CnCode);
    }
    
    return font->glyph_bitmap;
}


/**
 * Generic glyph width get function used in 'font->get_width' when the font contains all characters in the range
 * @param font pointer to font
 * @param unicode_letter an unicode letter which width should be get
 * @return width of the gylph or -1 if not found
 */
int16_t lv_hzPEx_font_get_width_continuous(const lv_font_t * font, uint32_t CnCode)
{
    paCharsInfo_t * paCharsInfo = (paCharsInfo_t *)font->glyph_dsc;

    /* �ж������ĵĻ���Ӣ�ĵ� */    
    if (CnCode < 0x80)                                                                
    {
        return paCharsInfo->paAsciiInfo.Width;
    }
    else
    {
        /*Check the range*/
        if(CnCode < font->unicode_first || CnCode > font->unicode_last)
        {
            return -1;
        }
        return paCharsInfo->paHanziInfo.Width;
    }

    return -1;
}

