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
#include "lv_misc/lv_font.h"
#include "HZCharPEx.h"

#include "stm32f4xx.h" // Device header

#define FONT_ERROR           (uint32_t)(-1)
#define FONT_OK              (uint32_t)(-2)
#define FONT_NO_FIND_FONT    (uint32_t)(-3)


#if defined USE_CN_INT_LIB || defined USE_ASCII_INT_LIB
/* 字库位于内部的FLASH */
#define USE_SMALL_LIB_FONT              (1)     //1为使能内部字库
#endif

#ifdef USE_CN_EXT_LIB
/* 字库位于外部的FLASH */
#define USE_GBK_LIB_FONT                (0)     //1为使能GBK字库
/* 字库位于外部的FLASH */
#define USE_GB2312_LIB_FONT             (0)     //1为使能GB2312字库
#endif

#if USE_SMALL_LIB_FONT == 1
/* 中文字库最大容量 */
#define ChAR_NUM_MAX          200
#endif

/* 英文的字库的地址如果不存在，请将其注释掉，避免出现错误 */
#ifdef USE_ASCII_EXT_LIB
/* 英文字库基地址 */
#define FONT_ASCII16_BASE_ADDR                 0x20000000
#define FONT_ASCII24_BASE_ADDR                 0x20000000
#define FONT_ASCII32_BASE_ADDR                 0x20000000
#define FONT_ASCII40_BASE_ADDR                 0x20000000
#define FONT_ASCII48_BASE_ADDR                 0x20000000
#endif

/* 汉字的字库的地址如果不存在，请将其注释掉，避免出现错误 */
#if USE_GBK_LIB_FONT == 1

/* 中文字库基地址 */
#define GBK_FONT_CN16_BASE_ADDR                 0x20000000
#define GBK_FONT_CN24_BASE_ADDR                 0x20000000
#define GBK_FONT_CN32_BASE_ADDR                 0x20000000
#define GBK_FONT_CN40_BASE_ADDR                 0x20000000
#define GBK_FONT_CN48_BASE_ADDR                 0x20000000
#endif

#if USE_GB2312_LIB_FONT == 1
/* 中文字库基地址 */
#define GB2312_FONT_CN16_BASE_ADDR                 0x20000000
#define GB2312_FONT_CN24_BASE_ADDR                 0x20000000
#define GB2312_FONT_CN32_BASE_ADDR                 0x20000000
#define GB2312_FONT_CN40_BASE_ADDR                 0x20000000
#define GB2312_FONT_CN48_BASE_ADDR                 0x20000000
#endif

uint8_t lv_FontDataBuf[BYTES_PER_FONT];

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
 * @brief   从内存里获取点阵的数据
 * @param   GUI_CnInfo 显示文字的信息的结构体
 * @note
 * @retval  无
 */
static inline uint8_t * _GetASCII_FontData(const lv_font_t *font, uint32_t CnCode)
{
    uint8_t WordNun; 
    paCharsInfo_t * paCharsInfo = (paCharsInfo_t *)font->glyph_dsc;
    uint16_t SumBytes;
    uint16_t BytesPerFont = paCharsInfo->paAsciiInfo.Hight * paCharsInfo->paAsciiInfo.PerLinePixels;
    
    SumBytes = BytesPerFont;
    
    if (BytesPerFont > BYTES_PER_FONT)
    {
        BytesPerFont = BYTES_PER_FONT;
    }
    
#if defined USE_ASCII_EXT_LIB || defined USE_CN_EXT_LIB
    uint32_t FlashAddr = 0;
#endif
/* 屏蔽部分没有显示的ASCII码 */
    if (CnCode < 32)
    {
        goto _ERROR;
    }
    
    /* 从ASCII码的32开始有显示，所以此处减掉32 */
    WordNun = CnCode - 32;
#ifdef USE_ASCII_INT_LIB        
    /* 使用16的点阵 */
#ifdef USING_CN_16_CHAR
    if (font->h_px == 16) /* 8*16 ASCII字符 */
    {
        /* 指针直接取地址 */
        return (uint8_t *)&ASCII08x16[(uint16_t )WordNun * SumBytes];
    }
#endif /* USING_CN_16_CHAR */   
    
    /* 使用24的点阵 */
#ifdef USING_CN_24_CHAR  
    if (font->h_px == 24) /* 16*24 ASCII字符 */
    {          
        return (uint8_t *)&ASCII12x24[(uint16_t )WordNun * SumBytes];
    }
#endif /* USING_CN_24_CHAR */
    
    /* 使用32的点阵 */
#ifdef USING_CN_32_CHAR  
    if (font->h_px == 32) /* 20*32 ASCII字符 */
    {  
        return (uint8_t *)&ASCII16x32[(uint16_t )WordNun * SumBytes];
    }
#endif /* USING_CN_32_CHAR */
    
    /* 使用40的点阵 */
#ifdef USING_CN_40_CHAR  
    if (font->h_px == 40) /* 24*40 ASCII字符 */
    {  
        return (uint8_t *)&ASCII20x40[(uint16_t )WordNun * SumBytes];
    }
#endif /* USING_CN_40_CHAR */
    
    /* 使用48的点阵 */
#ifdef USING_CN_48_CHAR   
    if (font->h_px == 48) /* 28*48 ASCII字符 */
    { 
        return (uint8_t *)&ASCII24x48[(uint16_t )WordNun * SumBytes];
    }
#endif /* USING_CN_48_CHAR */
#elif defined USE_ASCII_EXT_LIB
    /* 使用16的点阵 */
#ifdef USING_CN_16_CHAR
    if (font->h_px == 16) /* 8*16 ASCII字符 */
    {
#ifdef FONT_ASCII16_BASE_ADDR
        /* 计算地址 */
        FlashAddr = FONT_ASCII16_BASE_ADDR + (uint16_t)WordNun * SumBytes;
        goto _ReadASCII_Data; 
#else
        return (uint8_t *)FONT_NO_FIND_FONT;
#endif
    }
#endif /* USING_CN_16_CHAR */   
    
    /* 使用24的点阵 */
#ifdef USING_CN_24_CHAR  
    if (font->h_px == 24) /* 16*24 ASCII字符 */
    {          
#ifdef FONT_ASCII24_BASE_ADDR
        FlashAddr = FONT_ASCII24_BASE_ADDR + (uint16_t)WordNun * SumBytes;
        goto _ReadASCII_Data;
#else
        return (uint8_t *)FONT_NO_FIND_FONT;
#endif
    }
#endif /* USING_CN_24_CHAR */
    
    /* 使用32的点阵 */
#ifdef USING_CN_32_CHAR  
    if (font->h_px == 32) /* 20*32 ASCII字符 */
    {  
#ifdef FONT_ASCII32_BASE_ADDR
        FlashAddr = FONT_ASCII32_BASE_ADDR + (uint16_t)WordNun * SumBytes;
        goto _ReadASCII_Data;
#else
        return (uint8_t *)FONT_NO_FIND_FONT;
#endif
    }
#endif /* USING_CN_32_CHAR */
    
    /* 使用40的点阵 */
#ifdef USING_CN_40_CHAR  
    if (font->h_px == 40) /* 24*40 ASCII字符 */
    {  
#ifdef FONT_ASCII40_BASE_ADDR
        FlashAddr = FONT_ASCII40_BASE_ADDR + (uint16_t)WordNun * SumBytes;
        goto _ReadASCII_Data;
#else
        return (uint8_t *)FONT_NO_FIND_FONT;
#endif
    }
#endif /* USING_CN_40_CHAR */
    
    /* 使用48的点阵 */
#ifdef USING_CN_48_CHAR   
    if (font->h_px == 48) /* 28*48 ASCII字符 */
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
    _GUI_DeviceAPI.ReadData(FlashAddr, lv_FontDataBuf, BytesPerFont);
    return _GUI_FontDataBufFromFlash;
#endif
_ERROR:
    return (uint8_t *)FONT_ERROR;
}

/**
 * @func    _GetCN_FontData
 * @brief   从内存里获取点阵的数据
 * @param   GUI_CnInfo 显示文字的信息的结构体
 * @note
 * @retval  无
 */
static inline uint8_t * _GetCN_FontDataFromMem(const lv_font_t *font, uint32_t CnCode)
{
    uint16_t i = 0;
#if defined USE_ASCII_EXT_LIB || defined USE_CN_EXT_LIB
    uint32_t FlashAddr = 0;
    uint8_t FontType = NONE_FONT;
#endif
#ifdef USE_CN_INT_LIB
#ifdef USING_CN_16_CHAR  
    if(font->h_px == 16) /* 16*16 中文字符 */
    { 
        for (i = 0; i < ChAR_NUM_MAX; i++)        //循环查询内码，查找汉字的数据
        {
            if((HanZi16Index[i].Index[0] == ((CnCode >> 8) & 0xff))		
                & (HanZi16Index[i].Index[1] == (CnCode & 0xff)))
            {
                return (uint8_t *)HanZi16Data[i].Msk;
            }
        }
        
        goto _ERROR;
    }
#endif /* USING_CN_16_CHAR */
#ifdef USING_CN_24_CHAR          
    if (font->h_px == 24) /* 24*24 中文字符 */
    {
        for (i = 0; i < ChAR_NUM_MAX; i++)
        {
            if((HanZi24Index[i].Index[0] == ((CnCode >> 8) & 0xff))		
                & (HanZi24Index[i].Index[1] == (CnCode & 0xff)))
            {
                return (uint8_t *)HanZi24Data[i].Msk;
            }
        }
        
        goto _ERROR;
    }
#endif /* USING_CN_24_CHAR */
#ifdef USING_CN_32_CHAR  
    if (font->h_px == 32) /* 32*32 中文字符 */
    {
        for (i = 0; i < ChAR_NUM_MAX; i++)
        {
            if((HanZi32Index[i].Index[0] == ((CnCode >> 8) & 0xff))		
                & (HanZi32Index[i].Index[1] == (CnCode & 0xff)))
            {
                return (uint8_t *)HanZi32Data[i].Msk;
            }
        }
        
        goto _ERROR;
    }
#endif /* USING_CN_32_CHAR */
#ifdef USING_CN_40_CHAR  
    if (font->h_px == 40) /* 40*40 中文字符 */
    {
        for (i = 0; i < ChAR_NUM_MAX; i++)
        {
            if((HanZi40Index[i].Index[0] == ((CnCode >> 8) & 0xff))		
                & (HanZi40Index[i].Index[1] == (CnCode & 0xff)))
            {
                return (uint8_t *)HanZi40Data[i].Msk;
            }
        }
        
        goto _ERROR;
    }
#endif /* USING_CN_40_CHAR */
#ifdef USING_CN_48_CHAR  
    if (font->h_px == 48) /* 48*48 中文字符 */
    {
        for (i = 0; i < ChAR_NUM_MAX; i++)
        {
            if((HanZi48Index[i].Index[0] == ((CnCode >> 8) & 0xff))		
                & (HanZi48Index[i].Index[1] == (CnCode & 0xff)))
            {
                return (uint8_t *)HanZi48Data[i].Msk;
            }
        }
        
        goto _ERROR;
    }
#endif /* USING_CN_48_CHAR */
#elif defined USE_CN_EXT_LIB
#ifdef USING_CN_16_CHAR  
    if(font->h_px == 16) /* 16*16 中文字符 */
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
    if (font->h_px == 24) /* 24*24 中文字符 */
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
    if (font->h_px == 32) /* 32*32 中文字符 */
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
    if (font->h_px == 40) /* 40*40 中文字符 */
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
    if (font->h_px == 48) /* 48*48 中文字符 */
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
        /* 根据汉字内码的计算公式锁定起始地址 */
        code2 = c >> 8;
        code1 = c & 0xFF;
        
        /* 由于字符编码是安顺序存储的，先存储到高位（区号），然后是低位（位号）。而我们用的是小端格式，
            一个汉字两个字节，获取的16位变量，正好相反，16位变量的高位是位号，低位是区号。
        */
        FlashAddr = ((code1 - 0xA1) * 94 + (code2 - 0xa1)) * GUI_CnInfo->SumBytes + FlashAddr;
    }
    else if (FontType == GB2312_FONT)
    {
        /* 根据汉字内码的计算公式锁定起始地址 */
        code2 = c >> 8;
        code1 = c & 0xFF;
        
        /* 由于字符编码是安顺序存储的，先存储到高位（区号），然后是低位（位号）。而我们用的是小端格式，
            一个汉字两个字节，获取的16位变量，正好相反，16位变量的高位是位号，低位是区号。
        */
        FlashAddr = ((code1 - 0x81) * 190 + (code2 - 0x40) - (code2 / 128)) * GUI_CnInfo->SumBytes + FlashAddr;
    }

    _GUI_DeviceAPI.ReadData(FlashAddr, _GUI_FontDataBufFromFlash, GUI_CnInfo->SumBytes);
    GUI_CnInfo->FontDataBuf = _GUI_FontDataBufFromFlash;
#endif
_ERROR:
    return (uint8_t *)FONT_ERROR;
}

const uint8_t *lv_hzPEx_font_get_bitmap_continuous(const lv_font_t *font, uint32_t CnCode)
{
    uint8_t * _pBuf = NULL;
    /* 判断是中文的还是英文的 */    
    if (CnCode < 0x80)                                                                
    {
        _pBuf = _GetASCII_FontData(font, CnCode);
    }
    /* 中文显示 */
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

    /* 判断是中文的还是英文的 */    
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

