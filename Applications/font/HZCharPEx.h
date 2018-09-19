#ifndef _HZ_CHAR_PEX_H_
#define _HZ_CHAR_PEX_H_

#include "lvgl/lvgl.h"

extern uint8_t lv_FontDataBuf[BYTES_PER_FONT];

const uint8_t *lv_hzPEx_font_get_bitmap_continuous(const lv_font_t *font, uint32_t CnCode);
int16_t lv_hzPEx_font_get_width_continuous(const lv_font_t * font, uint32_t CnCode);

#endif


