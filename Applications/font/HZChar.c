
#include "../lv_font.h"

#define USE_WRJZY_20 2

#if USE_WRJZY_20 != 0	/*Can be enabled in lv_conf.h*/

/***********************************************************************************
 * font.TTF 20 px Font in U+4e00 (一) .. U+9fff (?)  range with all bpp
 * Sparse font with only these characters: 视频音乐设置闹钟蓝牙无线电量
***********************************************************************************/

/*Store the image of the letters (glyph)*/
static const uint8_t HZ_16_glyph_bitmap[] = 
{
};


/*Store the glyph descriptions*/
static const lv_font_glyph_dsc_t HZ16_glyph_dsc[] = 
{
  {.w_px = 19,   .glyph_index = 0},  
  {.w_px = 19,   .glyph_index = 100},
  {.w_px = 19,   .glyph_index = 200},
  {.w_px = 19,   .glyph_index = 300},  
  {.w_px = 19,   .glyph_index = 400},
  {.w_px = 19,   .glyph_index = 500},
  {.w_px = 19,   .glyph_index = 600},  
  {.w_px = 19,   .glyph_index = 700},
  {.w_px = 19,   .glyph_index = 800},
  {.w_px = 19,   .glyph_index = 900},  
  {.w_px = 19,   .glyph_index = 1000},
  {.w_px = 19,   .glyph_index = 1100},
  {.w_px = 19,   .glyph_index = 1200},  
  {.w_px = 19,   .glyph_index = 1300},
  {.w_px = 19,   .glyph_index = 1400},
  {.w_px = 19,   .glyph_index = 1500},  
  {.w_px = 19,   .glyph_index = 1600},
  {.w_px = 19,   .glyph_index = 1700},
};

lv_font_t HZ_16 = 
{    
    .unicode_first = 32,	/*First Unicode letter in this font*/
    .unicode_last = 126,	/*Last Unicode letter in this font*/
    .h_px = 20,				/*Font height in pixels*/
    .glyph_bitmap = GUI_FontDataBuf,	/*Bitmap of glyphs*/
    .glyph_dsc = HZ16_glyph_dsc,		/*Description of glyphs*/
    .unicode_list = NULL,	/*List of unicode characters*/
    .get_bitmap = lv_font_get_bitmap_continuous,	/*Function pointer to get glyph's bitmap*/
    .get_width = lv_font_get_width_continuous,	/*Function pointer to get glyph's width*/
#if USE_WRJZY_20 == 1
    .bpp = 1,				/*Bit per pixel*/
 #elif USE_WRJZY_20 == 2
    .bpp = 2,				/*Bit per pixel*/
 #elif USE_WRJZY_20 == 4
    .bpp = 4,				/*Bit per pixel*/
 #elif USE_WRJZY_20 == 8
    .bpp = 8,				/*Bit per pixel*/
#endif
    .next_page = NULL,		/*Pointer to a font extension*/
};

#endif /*USE_WRJZY_20*/
