#include "../lvgl/lv_misc/lv_font.h"
#include "HZCharPEx.h"
#include "font.h"

#ifdef USE_CN_EXT_LIB
/* 字体信息参数，不需要更改 */
const paCharsInfo_t FontCn16 = 
{
    {8,  16, 8,  "A16"},
    {16, 16, 16, "H16"},
};
#endif

lv_font_t HZChar_16 =
    {
        .unicode_first = 0x8140,                     /*First Unicode letter in this font*/
        .unicode_last = 0xfefe,                      /*Last Unicode letter in this font*/
        .h_px = 16,                                  /*Font height in pixels*/
        .glyph_bitmap = lv_FontDataBuf,             /*Bitmap of glyphs*/
        .glyph_dsc = (void *)&FontCn16,             /*Description of glyphs*/
        .unicode_list = NULL,                        /*List of unicode characters*/
        .get_bitmap = lv_hzPEx_font_get_bitmap_continuous, /*Function pointer to get glyph's bitmap*/
        .get_width = lv_hzPEx_font_get_width_continuous,   /*Function pointer to get glyph's width*/
        .bpp = 1, /*Bit per pixel*/
        .next_page = 0, /*Pointer to a font extension*/
};