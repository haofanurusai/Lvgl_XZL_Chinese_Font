#ifndef LVGL_MINIMAL_H
#define LVGL_MINIMAL_H

/* Minimal XZL font interface for standalone builds, not the full LVGL ABI. */
#include <stddef.h>
#include "lv_font_xzl_16_config.h"

#if !defined(__cplusplus) && !defined(bool) && \
    (!defined(__STDC_VERSION__) || __STDC_VERSION__ < 202311L)
typedef LV_FONT_XZL_BOOL bool;
#endif

#define LV_FONT_GLYPH_FORMAT_A1 1
#define LV_FONT_SUBPX_NONE 0
#define LV_FONT_KERNING_NONE 0
#define LV_FONT_DECLARE(name) extern const lv_font_t name;

typedef struct {
    LV_FONT_XZL_UINT32 index;
} lv_font_glyph_id_t;

typedef struct {
    LV_FONT_XZL_UINT16 adv_w;
    LV_FONT_XZL_UINT16 box_w;
    LV_FONT_XZL_UINT16 box_h;
    short ofs_x;
    short ofs_y;
    LV_FONT_XZL_UINT16 stride;
    LV_FONT_XZL_UINT8 format;
    LV_FONT_XZL_UINT8 is_placeholder;
    lv_font_glyph_id_t gid;
} lv_font_glyph_dsc_t;

typedef struct {
    LV_FONT_XZL_UINT32 stride;
} lv_draw_buf_header_t;

typedef struct _lv_draw_buf_t {
    lv_draw_buf_header_t header;
#if defined(LV_FONT_XZL_PLATFORM_KEIL_C51) || defined(LV_FONT_XZL_PLATFORM_KEIL_C251)
    LV_FONT_XZL_WORK_PTR(LV_FONT_XZL_UINT8) dat_;
#else
    LV_FONT_XZL_WORK_PTR(LV_FONT_XZL_UINT8) data;
#endif
} lv_draw_buf_t;

typedef struct _lv_font_t lv_font_t;
struct _lv_font_t {
    bool (*get_glyph_dsc)(const lv_font_t *, lv_font_glyph_dsc_t *,
                         LV_FONT_XZL_UINT32, LV_FONT_XZL_UINT32)
        LV_FONT_XZL_CALLBACK_REENTRANT;
    const void *(*get_glyph_bitmap)(lv_font_glyph_dsc_t *, lv_draw_buf_t *)
        LV_FONT_XZL_CALLBACK_REENTRANT;
    void (*release_glyph)(const lv_font_t *, lv_font_glyph_dsc_t *)
        LV_FONT_XZL_CALLBACK_REENTRANT;
    LV_FONT_XZL_UINT16 line_height;
    short base_line;
    LV_FONT_XZL_UINT8 subpx;
    LV_FONT_XZL_UINT8 kerning;
    LV_FONT_XZL_UINT8 static_bitmap;
    LV_FONT_XZL_UINT8 underline_position;
    LV_FONT_XZL_UINT8 underline_thickness;
    const void *dsc;
    const lv_font_t *fallback;
    void *user_data;
};

/* Standalone buffers have no LVGL-managed cache to flush. */
#define lv_draw_buf_flush_cache(draw_buf, area) \
    do { (void)(draw_buf); (void)(area); } while(0)

#endif /* LVGL_MINIMAL_H */
