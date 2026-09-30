#ifndef LV_FONT_XZL_16_H
#define LV_FONT_XZL_16_H
#include "lv_font_xzl_16_config.h"
#if USE_LVGL_MINIMAL
#include "lvgl_minimal.h"
#else
#include <lvgl.h>
#endif
#ifdef __cplusplus
extern "C"
{
#endif
    LV_FONT_DECLARE(lv_font_xzl_16)
    LV_FONT_XZL_BOOL xzl_unicode_to_bitmap(
        LV_FONT_XZL_UINT32 cp,
        LV_FONT_XZL_WORK_PTR(LV_FONT_XZL_UINT8) buf) LV_FONT_XZL_REENTRANT;
#ifdef __cplusplus
}
#endif

#endif
