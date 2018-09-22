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
#include "string.h"
#include "lv_misc/lv_font.h"
#include "HZCharPEx.h"

#include "stm32f4xx.h" // Device header

#define FONT_ERROR           (uint32_t)(-1)
#define FONT_OK              (uint32_t)(-2)
#define FONT_NO_FIND_FONT    (uint32_t)(-3)

#define GBK_FONT            0x00
#define GB2312_FONT         0x01
#define NONE_FONT           0xff

#if USE_SMALL_LIB_FONT == 1
/* �����ֿ�������� */
#define ChAR_NUM_MAX          200
#endif

#if defined USE_CN_EXT_LIB || defined USE_ASCII_EXT_LIB
uint8_t lv_FontDataBuf[BYTES_PER_FONT];
void (*ReadDataApi)(uint32_t, uint8_t *, uint16_t) = NULL;
#endif

/* Ӣ�ĵ��ֿ�ĵ�ַ��������ڣ��뽫��ע�͵���������ִ��� */
#ifdef USE_ASCII_EXT_LIB
/* Ӣ���ֿ����ַ */
#define FONT_ASCII16_BASE_ADDR                 0x00200000
#define FONT_ASCII24_BASE_ADDR                 0x00200600
#define FONT_ASCII32_BASE_ADDR                 0x00201800
#define FONT_ASCII40_BASE_ADDR                 0x00203000
#define FONT_ASCII48_BASE_ADDR                 0x00205d00
#endif

/* ���ֵ��ֿ�ĵ�ַ��������ڣ��뽫��ע�͵���������ִ��� */
#if USE_GBK_LIB_FONT == 1

/* �����ֿ����ַ */
#define GBK_FONT_CN16_BASE_ADDR                 0x0020A000
#define GBK_FONT_CN24_BASE_ADDR                 0x002C6000
#define GBK_FONT_CN32_BASE_ADDR                 0x0046B000
#define GBK_FONT_CN40_BASE_ADDR                 0x00758000
#define GBK_FONT_CN48_BASE_ADDR                 0x00BE9000
#endif

#if USE_GB2312_LIB_FONT == 1
/* �����ֿ����ַ */
#define GB2312_FONT_CN16_BASE_ADDR                 0x20000000
#define GB2312_FONT_CN24_BASE_ADDR                 0x20000000
#define GB2312_FONT_CN32_BASE_ADDR                 0x20000000
#define GB2312_FONT_CN40_BASE_ADDR                 0x20000000
#define GB2312_FONT_CN48_BASE_ADDR                 0x20000000
#endif


#ifdef USING_CN_16_CHAR
extern Cn16Data_t HanZi16Data[];
extern CnChar_t HanZi16Index[];
extern uint8_t ASCII08x16[];
#endif

#ifdef USING_CN_24_CHAR
extern Cn24Data_t HanZi24Data[];
extern CnChar_t HanZi24Index[];
extern uint8_t ASCII12x24[];
#endif

#ifdef USING_CN_32_CHAR
extern Cn32Data_t HanZi32Data[];
extern CnChar_t HanZi32Index[];
extern uint8_t ASCII16x32[];
#endif

#ifdef USING_CN_40_CHAR
extern Cn40Data_t HanZi40Data[];
extern CnChar_t HanZi40Index[];
extern uint8_t ASCII20x40[];
#endif

#ifdef USING_CN_48_CHAR
extern Cn48Data_t HanZi48Data[];
extern CnChar_t HanZi48Index[];
extern uint8_t ASCII24x48[];
#endif
/* User function Declaration -------------------------------------------------*/

/**
 * @func    _GetASCII_FontData
 * @brief   ���ڴ����ȡ���������
 * @param   GUI_CnInfo ��ʾ���ֵ���Ϣ�Ľṹ��
 * @note
 * @retval  ��
 */
static inline uint8_t * _GetASCII_FontData(const lv_font_t *font, uint32_t CnCode)
{
    uint8_t WordNun; 
    paCharsInfo_t * paCharsInfo = (paCharsInfo_t *)font->glyph_dsc;
    uint16_t SumBytes;
    uint16_t BytesPerFont = paCharsInfo->paAsciiInfo.Hight * paCharsInfo->paAsciiInfo.PerLineBytes;
    
    SumBytes = BytesPerFont;
    
    if (BytesPerFont > BYTES_PER_FONT)
    {
        BytesPerFont = BYTES_PER_FONT;
    }
    
#if defined USE_ASCII_EXT_LIB || defined USE_CN_EXT_LIB
    uint32_t FlashAddr = 0;
#endif
/* ���β���û����ʾ��ASCII�� */
    if (CnCode < 32)
    {
        goto _ERROR;
    }
    
    /* ��ASCII���32��ʼ����ʾ�����Դ˴�����32 */
    WordNun = CnCode - 32;
#ifdef USE_ASCII_INT_LIB        
    /* ʹ��16�ĵ��� */
#ifdef USING_CN_16_CHAR
    if (font->h_px == 16) /* 8*16 ASCII�ַ� */
    {
        /* ָ��ֱ��ȡ��ַ */
        return (uint8_t *)&ASCII08x16[(uint16_t )WordNun * SumBytes];
    }
#endif /* USING_CN_16_CHAR */   
    
    /* ʹ��24�ĵ��� */
#ifdef USING_CN_24_CHAR  
    if (font->h_px == 24) /* 16*24 ASCII�ַ� */
    {          
        return (uint8_t *)&ASCII12x24[(uint16_t )WordNun * SumBytes];
    }
#endif /* USING_CN_24_CHAR */
    
    /* ʹ��32�ĵ��� */
#ifdef USING_CN_32_CHAR  
    if (font->h_px == 32) /* 20*32 ASCII�ַ� */
    {  
        return (uint8_t *)&ASCII16x32[(uint16_t )WordNun * SumBytes];
    }
#endif /* USING_CN_32_CHAR */
    
    /* ʹ��40�ĵ��� */
#ifdef USING_CN_40_CHAR  
    if (font->h_px == 40) /* 24*40 ASCII�ַ� */
    {  
        return (uint8_t *)&ASCII20x40[(uint16_t )WordNun * SumBytes];
    }
#endif /* USING_CN_40_CHAR */
    
    /* ʹ��48�ĵ��� */
#ifdef USING_CN_48_CHAR   
    if (font->h_px == 48) /* 28*48 ASCII�ַ� */
    { 
        return (uint8_t *)&ASCII24x48[(uint16_t )WordNun * SumBytes];
    }
#endif /* USING_CN_48_CHAR */
#elif defined USE_ASCII_EXT_LIB
    /* ʹ��16�ĵ��� */
#ifdef USING_CN_16_CHAR
    if (font->h_px == 16) /* 8*16 ASCII�ַ� */
    {
#ifdef FONT_ASCII16_BASE_ADDR
        /* �����ַ */
        FlashAddr = FONT_ASCII16_BASE_ADDR + (uint16_t)WordNun * SumBytes;
        goto _ReadASCII_Data; 
#else
        return (uint8_t *)FONT_NO_FIND_FONT;
#endif
    }
#endif /* USING_CN_16_CHAR */   
    
    /* ʹ��24�ĵ��� */
#ifdef USING_CN_24_CHAR  
    if (font->h_px == 24) /* 16*24 ASCII�ַ� */
    {          
#ifdef FONT_ASCII24_BASE_ADDR
        FlashAddr = FONT_ASCII24_BASE_ADDR + (uint16_t)WordNun * SumBytes;
        goto _ReadASCII_Data;
#else
        return (uint8_t *)FONT_NO_FIND_FONT;
#endif
    }
#endif /* USING_CN_24_CHAR */
    
    /* ʹ��32�ĵ��� */
#ifdef USING_CN_32_CHAR  
    if (font->h_px == 32) /* 20*32 ASCII�ַ� */
    {  
#ifdef FONT_ASCII32_BASE_ADDR
        FlashAddr = FONT_ASCII32_BASE_ADDR + (uint16_t)WordNun * SumBytes;
        goto _ReadASCII_Data;
#else
        return (uint8_t *)FONT_NO_FIND_FONT;
#endif
    }
#endif /* USING_CN_32_CHAR */
    
    /* ʹ��40�ĵ��� */
#ifdef USING_CN_40_CHAR  
    if (font->h_px == 40) /* 24*40 ASCII�ַ� */
    {  
#ifdef FONT_ASCII40_BASE_ADDR
        FlashAddr = FONT_ASCII40_BASE_ADDR + (uint16_t)WordNun * SumBytes;
        goto _ReadASCII_Data;
#else
        return (uint8_t *)FONT_NO_FIND_FONT;
#endif
    }
#endif /* USING_CN_40_CHAR */
    
    /* ʹ��48�ĵ��� */
#ifdef USING_CN_48_CHAR   
    if (font->h_px == 48) /* 28*48 ASCII�ַ� */
    { 
#ifdef FONT_ASCII48_BASE_ADDR
        FlashAddr = FONT_ASCII48_BASE_ADDR + (uint16_t)WordNun * SumBytes;
        goto _ReadASCII_Data;
#else
        return (uint8_t *)FONT_NO_FIND_FONT;
#endif
    }
#endif /* USING_CN_48_CHAR */
_ReadASCII_Data:
    if (ReadDataApi == NULL)
        return (uint8_t *)FONT_ERROR;
    ReadDataApi(FlashAddr, lv_FontDataBuf, BytesPerFont);
    return lv_FontDataBuf;
#endif
_ERROR:
    return (uint8_t *)FONT_ERROR;
}

/**
 * @func    _GetCN_FontData
 * @brief   ���ڴ����ȡ���������
 * @param   GUI_CnInfo ��ʾ���ֵ���Ϣ�Ľṹ��
 * @note
 * @retval  ��
 */
static inline uint8_t * _GetCN_FontDataFromMem(const lv_font_t *font, uint32_t CnCode)
{
    uint16_t i = 0;
#if defined USE_CN_EXT_LIB
    uint32_t FlashAddr = 0;
    uint8_t FontType = NONE_FONT;
    paCharsInfo_t * paCharsInfo = (paCharsInfo_t *)font->glyph_dsc;
    uint16_t SumBytes;
    uint16_t BytesPerFont = paCharsInfo->paHanziInfo.Hight * paCharsInfo->paHanziInfo.PerLineBytes;
    
    SumBytes = BytesPerFont;
    
    if (BytesPerFont > BYTES_PER_FONT)
    {
        BytesPerFont = BYTES_PER_FONT;
    }
#endif
#ifdef USE_CN_INT_LIB
#ifdef USING_CN_16_CHAR  
    if(font->h_px == 16) /* 16*16 �����ַ� */
    { 
        for (i = 0; i < ChAR_NUM_MAX; i++)        //ѭ����ѯ���룬���Һ��ֵ�����
        {
            if((HanZi16Index[i].Index[1] == ((CnCode >> 8) & 0xff))		
                & (HanZi16Index[i].Index[0] == (CnCode & 0xff)))
            {
                return (uint8_t *)HanZi16Data[i].Msk;
            }
        }
        
        goto _ERROR;
    }
#endif /* USING_CN_16_CHAR */
#ifdef USING_CN_24_CHAR          
    if (font->h_px == 24) /* 24*24 �����ַ� */
    {
        for (i = 0; i < ChAR_NUM_MAX; i++)
        {
            if((HanZi24Index[i].Index[1] == ((CnCode >> 8) & 0xff))		
                & (HanZi24Index[i].Index[0] == (CnCode & 0xff)))
            {
                return (uint8_t *)HanZi24Data[i].Msk;
            }
        }
        
        goto _ERROR;
    }
#endif /* USING_CN_24_CHAR */
#ifdef USING_CN_32_CHAR  
    if (font->h_px == 32) /* 32*32 �����ַ� */
    {
        for (i = 0; i < ChAR_NUM_MAX; i++)
        {
            if((HanZi32Index[i].Index[1] == ((CnCode >> 8) & 0xff))		
                & (HanZi32Index[i].Index[0] == (CnCode & 0xff)))
            {
                return (uint8_t *)HanZi32Data[i].Msk;
            }
        }
        
        goto _ERROR;
    }
#endif /* USING_CN_32_CHAR */
#ifdef USING_CN_40_CHAR  
    if (font->h_px == 40) /* 40*40 �����ַ� */
    {
        for (i = 0; i < ChAR_NUM_MAX; i++)
        {
            if((HanZi40Index[i].Index[1] == ((CnCode >> 8) & 0xff))		
                & (HanZi40Index[i].Index[0] == (CnCode & 0xff)))
            {
                return (uint8_t *)HanZi40Data[i].Msk;
            }
        }
        
        goto _ERROR;
    }
#endif /* USING_CN_40_CHAR */
#ifdef USING_CN_48_CHAR  
    if (font->h_px == 48) /* 48*48 �����ַ� */
    {
        for (i = 0; i < ChAR_NUM_MAX; i++)
        {
            if((HanZi48Index[i].Index[1] == ((CnCode >> 8) & 0xff))		
                & (HanZi48Index[i].Index[0] == (CnCode & 0xff)))
            {
                return (uint8_t *)HanZi48Data[i].Msk;
            }
        }
        
        goto _ERROR;
    }
#endif /* USING_CN_48_CHAR */
#elif defined USE_CN_EXT_LIB
#ifdef USING_CN_16_CHAR  
    if(font->h_px == 16) /* 16*16 �����ַ� */
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
    if (font->h_px == 24) /* 24*24 �����ַ� */
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
    if (font->h_px == 32) /* 32*32 �����ַ� */
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
    if (font->h_px == 40) /* 40*40 �����ַ� */
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
    if (font->h_px == 48) /* 48*48 �����ַ� */
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
        uint8_t code1, code2;
        /* ���ݺ�������ļ��㹫ʽ������ʼ��ַ */
        code2 = CnCode >> 8;        /*!< λ�� */
        code1 = CnCode & 0xFF;      /*!< ���� */
        
        /* �����ַ������ǰ�˳��洢�ģ��ȴ洢����λ�����ţ���Ȼ���ǵ�λ��λ�ţ����������õ���С�˸�ʽ��
            һ�����������ֽڣ���ȡ��16λ�����������෴��16λ�����ĸ�λ��λ�ţ���λ�����š�        */
        FlashAddr = ((code1 - 0x81) * 190 + (code2 - 0x40) - (code2 / 128)) * SumBytes + FlashAddr;
    }
    else if (FontType == GB2312_FONT)
    {
        uint8_t code1, code2;
        /* ���ݺ�������ļ��㹫ʽ������ʼ��ַ */
        code2 = CnCode >> 8;
        code1 = CnCode & 0xFF;
        
        /* �����ַ������ǰ�˳��洢�ģ��ȴ洢����λ�����ţ���Ȼ���ǵ�λ��λ�ţ����������õ���С�˸�ʽ��
            һ�����������ֽڣ���ȡ��16λ�����������෴��16λ�����ĸ�λ��λ�ţ���λ�����š�        */
        FlashAddr = ((code1 - 0xA1) * 94 + (code2 - 0xa1)) * SumBytes + FlashAddr;
    }
    if (ReadDataApi == NULL)
        return (uint8_t *)FONT_ERROR;
    ReadDataApi(FlashAddr, lv_FontDataBuf, BytesPerFont);
    return lv_FontDataBuf;
#endif
_ERROR:
    return (uint8_t *)FONT_ERROR;
}

const uint8_t *lv_hzPEx_font_get_bitmap_continuous(const lv_font_t *font, uint32_t CnCode)
{
    uint8_t * _pBuf = NULL;
    /* �ж������ĵĻ���Ӣ�ĵ� */    
    if (CnCode < 0x80)                                                                
    {
        _pBuf = _GetASCII_FontData(font, CnCode);
    }
    /* ������ʾ */
    else
    {
        /*Check the range*/
        if(CnCode < font->unicode_first || CnCode > font->unicode_last)
        {
            return (uint8_t *)FONT_ERROR;
        }
        _pBuf = _GetCN_FontDataFromMem(font, CnCode);
    }
    
    if (_pBuf == (uint8_t *)FONT_ERROR || _pBuf == (uint8_t *)FONT_NO_FIND_FONT)
    {
        return NULL;
    }
    
    return _pBuf;
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
}

#ifdef USE_CN_EXT_LIB
void FontSetReadDataApi(void (*pfunc)(uint32_t, uint8_t *, uint16_t))
{
    ReadDataApi = pfunc;
}
#endif
