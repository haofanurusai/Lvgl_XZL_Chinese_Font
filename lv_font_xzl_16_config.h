#ifndef LV_FONT_XZL_16_CONFIG_H
#define LV_FONT_XZL_16_CONFIG_H

#include <limits.h>

/*
 * Minimal LVGL data structure.
 * If use full LVGL environment, set USE_LVGL_MINIMAL to 0
 */
#if !defined(USE_LVGL_MINIMAL)
#define USE_LVGL_MINIMAL 1
#endif

/*
 * Platform detection.
 * Define one LV_FONT_XZL_PLATFORM_* macro before including this file to
 * override automatic detection.
 */
#if !defined(LV_FONT_XZL_PLATFORM_KEIL_C51) &&   \
    !defined(LV_FONT_XZL_PLATFORM_KEIL_C251) &&  \
    !defined(LV_FONT_XZL_PLATFORM_SDCC_MCS51) && \
    !defined(LV_FONT_XZL_PLATFORM_AVR) &&        \
    !defined(LV_FONT_XZL_PLATFORM_GENERIC)
#if defined(__C251__)
#define LV_FONT_XZL_PLATFORM_KEIL_C251 1
#elif defined(__C51__) || defined(__CX51__)
#define LV_FONT_XZL_PLATFORM_KEIL_C51 1
#elif defined(__SDCC_mcs51) || defined(__SDCC_MCS51)
#define LV_FONT_XZL_PLATFORM_SDCC_MCS51 1
#elif defined(__AVR__)
#define LV_FONT_XZL_PLATFORM_AVR 1
#else
#define LV_FONT_XZL_PLATFORM_GENERIC 1
#endif
#endif

#if defined(LV_FONT_XZL_PLATFORM_AVR)
#include <avr/pgmspace.h>
#endif

/* Keep the full LVGL member name; avoid Keil keywords in the minimal stub. */
#ifndef LV_FONT_XZL_DRAW_BUF_DATA
#if USE_LVGL_MINIMAL && (defined(LV_FONT_XZL_PLATFORM_KEIL_C51) || \
                         defined(LV_FONT_XZL_PLATFORM_KEIL_C251))
#define LV_FONT_XZL_DRAW_BUF_DATA(buf) ((buf)->dat_)
#else
#define LV_FONT_XZL_DRAW_BUF_DATA(buf) ((buf)->data)
#endif
#endif

/* Internal boolean type.  May be overridden with e.g. Keil C51 bit. */
#ifndef LV_FONT_XZL_BOOL
#define LV_FONT_XZL_BOOL unsigned char
#endif

/* Fixed-width unsigned integer types without <stdint.h>. */
#ifndef LV_FONT_XZL_UINT8
#if UCHAR_MAX == 0xFFU
#define LV_FONT_XZL_UINT8 unsigned char
#else
#error "LV_FONT_XZL requires an 8-bit unsigned integer type"
#endif
#endif

#ifndef LV_FONT_XZL_UINT16
#if USHRT_MAX == 0xFFFFU
#define LV_FONT_XZL_UINT16 unsigned short
#elif UINT_MAX == 0xFFFFU
#define LV_FONT_XZL_UINT16 unsigned int
#else
#error "Define LV_FONT_XZL_UINT16 as an exact 16-bit unsigned type"
#endif
#endif

#ifndef LV_FONT_XZL_UINT32
#if UINT_MAX == 0xFFFFFFFFUL
#define LV_FONT_XZL_UINT32 unsigned int
#elif ULONG_MAX == 0xFFFFFFFFUL
#define LV_FONT_XZL_UINT32 unsigned long
#else
#error "Define LV_FONT_XZL_UINT32 as an exact 32-bit unsigned type"
#endif
#endif

/*
 * Function calling convention / reentrancy abstraction.
 *
 * LV_FONT_XZL_REENTRANT_KEYWORD is the native compiler keyword.
 * LV_FONT_XZL_USE_REENTRANT selects whether it is applied.  The default is 0
 * so an unmodified LVGL lv_font_t callback type remains type-compatible.
 *
 * If enabled on C51/C251/SDCC, the same LV_FONT_XZL_REENTRANT suffix must also
 * be present on the corresponding function-pointer type in the platform's
 * lv_font_t declaration.  This keeps one real function ABI instead of adding
 * wrapper/thunk functions.
 */
#ifndef LV_FONT_XZL_REENTRANT_KEYWORD
#if defined(LV_FONT_XZL_PLATFORM_KEIL_C51) || \
    defined(LV_FONT_XZL_PLATFORM_KEIL_C251)
#define LV_FONT_XZL_REENTRANT_KEYWORD reentrant
#elif defined(LV_FONT_XZL_PLATFORM_SDCC_MCS51)
#define LV_FONT_XZL_REENTRANT_KEYWORD __reentrant
#else
#define LV_FONT_XZL_REENTRANT_KEYWORD
#endif
#endif
#ifndef LV_FONT_XZL_USE_REENTRANT
#define LV_FONT_XZL_USE_REENTRANT 0
#endif
#ifndef LV_FONT_XZL_REENTRANT
#if LV_FONT_XZL_USE_REENTRANT
#define LV_FONT_XZL_REENTRANT LV_FONT_XZL_REENTRANT_KEYWORD
#else
#define LV_FONT_XZL_REENTRANT
#endif
#endif
#ifndef LV_FONT_XZL_CALLBACK_REENTRANT
#define LV_FONT_XZL_CALLBACK_REENTRANT LV_FONT_XZL_REENTRANT
#endif

/*
 * Decoder stack capacity.  The bundled 6763-glyph database was exhaustively
 * measured at ops=6, conts=3, geoms=4 maximum depth.  8 is therefore enough
 * for classic 8051-class targets; other targets keep the historical 64.
 */
#ifndef LV_FONT_XZL_STACK_MAX
#if defined(LV_FONT_XZL_PLATFORM_KEIL_C51) || \
    defined(LV_FONT_XZL_PLATFORM_SDCC_MCS51)
#define LV_FONT_XZL_STACK_MAX 8
#else
#define LV_FONT_XZL_STACK_MAX 64
#endif
#endif

/*
 * Memory-space abstraction.
 *
 * Object placement and pointee address space are intentionally separate:
 *
 *   LV_FONT_XZL_RODATA          placement of read-only database objects
 *   LV_FONT_XZL_RO_PTR_SPACE     address space a pointer to RODATA points into
 *   LV_FONT_XZL_WORKDATA        placement of writable decoder work objects
 *   LV_FONT_XZL_WORK_PTR_SPACE   address space a pointer to WORKDATA points into
 *   LV_FONT_XZL_SCRATCHDATA       placement of small temporary decode arrays
 *   LV_FONT_XZL_SCRATCH_PTR_SPACE address space of pointers to those arrays
 *   LV_FONT_XZL_LOCALDATA         placement of ordinary function temporaries
 *   LV_FONT_XZL_LOCAL_PTR_SPACE   address space of pointers to LOCALDATA
 *   LV_FONT_XZL_RECORDDATA        placement of address-taken decode records
 *   LV_FONT_XZL_RECORD_PTR_SPACE  address space of pointers to RECORDDATA
 *
 * On flat-memory compilers the *_PTR_QUAL macros expand to nothing, so the
 * abstraction has no generated-code cost.  On 8051-family compilers they map
 * directly to the compiler's native address-space qualifier.
 */

#if defined(LV_FONT_XZL_PLATFORM_KEIL_C51)
#ifndef LV_FONT_XZL_RODATA
#define LV_FONT_XZL_RODATA code
#endif
#ifndef LV_FONT_XZL_RO_PTR_SPACE
#define LV_FONT_XZL_RO_PTR_SPACE code
#endif
#ifndef LV_FONT_XZL_WORKDATA
#define LV_FONT_XZL_WORKDATA xdata
#endif
#ifndef LV_FONT_XZL_WORK_PTR_SPACE
#define LV_FONT_XZL_WORK_PTR_SPACE xdata
#endif
#ifndef LV_FONT_XZL_LOCALDATA
#if LV_FONT_XZL_USE_REENTRANT
#define LV_FONT_XZL_LOCALDATA
#else
#define LV_FONT_XZL_LOCALDATA xdata
#endif
#endif
#ifndef LV_FONT_XZL_LOCAL_PTR_SPACE
#if LV_FONT_XZL_USE_REENTRANT
#define LV_FONT_XZL_LOCAL_PTR_SPACE
#else
#define LV_FONT_XZL_LOCAL_PTR_SPACE xdata
#endif
#endif
#ifndef LV_FONT_XZL_RECORDDATA
#define LV_FONT_XZL_RECORDDATA LV_FONT_XZL_LOCALDATA
#endif
#ifndef LV_FONT_XZL_RECORD_PTR_SPACE
#define LV_FONT_XZL_RECORD_PTR_SPACE LV_FONT_XZL_LOCAL_PTR_SPACE
#endif

#elif defined(LV_FONT_XZL_PLATFORM_KEIL_C251)
#ifndef LV_FONT_XZL_RODATA
#define LV_FONT_XZL_RODATA code
#endif
#ifndef LV_FONT_XZL_RO_PTR_SPACE
#define LV_FONT_XZL_RO_PTR_SPACE code
#endif
#ifndef LV_FONT_XZL_WORKDATA
#define LV_FONT_XZL_WORKDATA near
#endif
#ifndef LV_FONT_XZL_WORK_PTR_SPACE
#define LV_FONT_XZL_WORK_PTR_SPACE near
#endif
#ifndef LV_FONT_XZL_LOCALDATA
#if LV_FONT_XZL_USE_REENTRANT
#define LV_FONT_XZL_LOCALDATA
#else
#define LV_FONT_XZL_LOCALDATA near
#endif
#endif
#ifndef LV_FONT_XZL_LOCAL_PTR_SPACE
#if LV_FONT_XZL_USE_REENTRANT
#define LV_FONT_XZL_LOCAL_PTR_SPACE
#else
#define LV_FONT_XZL_LOCAL_PTR_SPACE near
#endif
#endif
#ifndef LV_FONT_XZL_RECORDDATA
#define LV_FONT_XZL_RECORDDATA LV_FONT_XZL_LOCALDATA
#endif
#ifndef LV_FONT_XZL_RECORD_PTR_SPACE
#define LV_FONT_XZL_RECORD_PTR_SPACE LV_FONT_XZL_LOCAL_PTR_SPACE
#endif

#elif defined(LV_FONT_XZL_PLATFORM_SDCC_MCS51)
#ifndef LV_FONT_XZL_CUSTOM_ZERO
#define LV_FONT_XZL_CUSTOM_ZERO 1
#endif
#ifndef LV_FONT_XZL_RODATA
#define LV_FONT_XZL_RODATA __code
#endif
#ifndef LV_FONT_XZL_RO_PTR_SPACE
#define LV_FONT_XZL_RO_PTR_SPACE __code
#endif
/* SDCC MCS51 defaults the explicit decoder workspace to XDATA. */
#ifndef LV_FONT_XZL_WORKDATA
#define LV_FONT_XZL_WORKDATA __xdata
#endif
#ifndef LV_FONT_XZL_WORK_PTR_SPACE
#define LV_FONT_XZL_WORK_PTR_SPACE __xdata
#endif
/*
 * Make ordinary local storage explicit without changing SDCC's selected
 * automatic-memory model.  This mirrors asm/mcs51/features.h::_AUTOMEM.
 */
#ifndef LV_FONT_XZL_LOCALDATA
#if LV_FONT_XZL_USE_REENTRANT || defined(__SDCC_STACK_AUTO)
/* Named address-space automatic locals are rejected for reentrant/stack-auto. */
#define LV_FONT_XZL_LOCALDATA
#elif defined(__SDCC_MODEL_MEDIUM)
#define LV_FONT_XZL_LOCALDATA __pdata
#elif defined(__SDCC_MODEL_LARGE)
#define LV_FONT_XZL_LOCALDATA __xdata
#else
#define LV_FONT_XZL_LOCALDATA __data
#endif
#endif
#ifndef LV_FONT_XZL_LOCAL_PTR_SPACE
#if LV_FONT_XZL_USE_REENTRANT || defined(__SDCC_STACK_AUTO)
#define LV_FONT_XZL_LOCAL_PTR_SPACE
#else
#define LV_FONT_XZL_LOCAL_PTR_SPACE LV_FONT_XZL_LOCALDATA
#endif
#endif
#ifndef LV_FONT_XZL_RECORDDATA
#define LV_FONT_XZL_RECORDDATA LV_FONT_XZL_LOCALDATA
#endif
#ifndef LV_FONT_XZL_RECORD_PTR_SPACE
#define LV_FONT_XZL_RECORD_PTR_SPACE LV_FONT_XZL_LOCAL_PTR_SPACE
#endif

#elif defined(LV_FONT_XZL_PLATFORM_AVR)
#ifndef LV_FONT_XZL_RODATA
#define LV_FONT_XZL_RODATA PROGMEM
#endif
/* PROGMEM is an object-placement attribute; avr-libc read macros carry access. */
#ifndef LV_FONT_XZL_RO_PTR_SPACE
#define LV_FONT_XZL_RO_PTR_SPACE
#endif
#ifndef LV_FONT_XZL_WORKDATA
#define LV_FONT_XZL_WORKDATA
#endif
#ifndef LV_FONT_XZL_WORK_PTR_SPACE
#define LV_FONT_XZL_WORK_PTR_SPACE
#endif
#ifndef LV_FONT_XZL_LOCALDATA
#define LV_FONT_XZL_LOCALDATA
#endif
#ifndef LV_FONT_XZL_LOCAL_PTR_SPACE
#define LV_FONT_XZL_LOCAL_PTR_SPACE
#endif
#ifndef LV_FONT_XZL_RECORDDATA
#define LV_FONT_XZL_RECORDDATA LV_FONT_XZL_LOCALDATA
#endif
#ifndef LV_FONT_XZL_RECORD_PTR_SPACE
#define LV_FONT_XZL_RECORD_PTR_SPACE LV_FONT_XZL_LOCAL_PTR_SPACE
#endif

#else
#ifndef LV_FONT_XZL_RODATA
#define LV_FONT_XZL_RODATA
#endif
#ifndef LV_FONT_XZL_RO_PTR_SPACE
#define LV_FONT_XZL_RO_PTR_SPACE
#endif
#ifndef LV_FONT_XZL_WORKDATA
#define LV_FONT_XZL_WORKDATA
#endif
#ifndef LV_FONT_XZL_WORK_PTR_SPACE
#define LV_FONT_XZL_WORK_PTR_SPACE
#endif
#ifndef LV_FONT_XZL_LOCALDATA
#define LV_FONT_XZL_LOCALDATA
#endif
#ifndef LV_FONT_XZL_LOCAL_PTR_SPACE
#define LV_FONT_XZL_LOCAL_PTR_SPACE
#endif
#ifndef LV_FONT_XZL_RECORDDATA
#define LV_FONT_XZL_RECORDDATA LV_FONT_XZL_LOCALDATA
#endif
#ifndef LV_FONT_XZL_RECORD_PTR_SPACE
#define LV_FONT_XZL_RECORD_PTR_SPACE LV_FONT_XZL_LOCAL_PTR_SPACE
#endif
#endif

/* Scratch defaults to the same memory space as the persistent decode workset. */
#ifndef LV_FONT_XZL_SCRATCHDATA
#define LV_FONT_XZL_SCRATCHDATA LV_FONT_XZL_WORKDATA
#endif
#ifndef LV_FONT_XZL_SCRATCH_PTR_SPACE
#define LV_FONT_XZL_SCRATCH_PTR_SPACE LV_FONT_XZL_WORK_PTR_SPACE
#endif

/* Typed pointer constructors: address-space qualification is on the pointee. */
#ifndef LV_FONT_XZL_RO_PTR
#define LV_FONT_XZL_RO_PTR(type) const type LV_FONT_XZL_RO_PTR_SPACE *
#endif
#ifndef LV_FONT_XZL_WORK_PTR
#define LV_FONT_XZL_WORK_PTR(type) type LV_FONT_XZL_WORK_PTR_SPACE *
#endif
#ifndef LV_FONT_XZL_WORK_CONST_PTR
#define LV_FONT_XZL_WORK_CONST_PTR(type) const type LV_FONT_XZL_WORK_PTR_SPACE *
#endif
#ifndef LV_FONT_XZL_SCRATCH_PTR
#define LV_FONT_XZL_SCRATCH_PTR(type) type LV_FONT_XZL_SCRATCH_PTR_SPACE *
#endif
#ifndef LV_FONT_XZL_LOCAL_PTR
#define LV_FONT_XZL_LOCAL_PTR(type) type LV_FONT_XZL_LOCAL_PTR_SPACE *
#endif
#ifndef LV_FONT_XZL_LOCAL_CONST_PTR
#define LV_FONT_XZL_LOCAL_CONST_PTR(type) const type LV_FONT_XZL_LOCAL_PTR_SPACE *
#endif
#ifndef LV_FONT_XZL_RECORD_PTR
#define LV_FONT_XZL_RECORD_PTR(type) type LV_FONT_XZL_RECORD_PTR_SPACE *
#endif
#ifndef LV_FONT_XZL_RECORD_CONST_PTR
#define LV_FONT_XZL_RECORD_CONST_PTR(type) const type LV_FONT_XZL_RECORD_PTR_SPACE *
#endif

/* Object declarations.  AVR needs PROGMEM after the declarator. */
#ifndef LV_FONT_XZL_RO_ARRAY
#if defined(LV_FONT_XZL_PLATFORM_AVR)
#define LV_FONT_XZL_RO_ARRAY(type, name, size) \
      static const type name[size] LV_FONT_XZL_RODATA
#else
#define LV_FONT_XZL_RO_ARRAY(type, name, size) \
      static const type LV_FONT_XZL_RODATA name[size]
#endif
#endif
#ifndef LV_FONT_XZL_WORK_VAR
#if defined(LV_FONT_XZL_PLATFORM_SDCC_MCS51)
/* SDCC requires named-address-space locals in __reentrant functions to be static. */
#define LV_FONT_XZL_WORK_VAR(type, name) static type LV_FONT_XZL_WORKDATA name
#else
#define LV_FONT_XZL_WORK_VAR(type, name) type LV_FONT_XZL_WORKDATA name
#endif
#endif
#ifndef LV_FONT_XZL_WORK_ARRAY
#if defined(LV_FONT_XZL_PLATFORM_SDCC_MCS51)
#define LV_FONT_XZL_WORK_ARRAY(type, name, size) \
      static type LV_FONT_XZL_WORKDATA name[size]
#else
#define LV_FONT_XZL_WORK_ARRAY(type, name, size) \
      type LV_FONT_XZL_WORKDATA name[size]
#endif
#endif
#ifndef LV_FONT_XZL_SCRATCH_ARRAY
#if defined(LV_FONT_XZL_PLATFORM_SDCC_MCS51)
#define LV_FONT_XZL_SCRATCH_ARRAY(type, name, size) \
      static type LV_FONT_XZL_SCRATCHDATA name[size]
#else
#define LV_FONT_XZL_SCRATCH_ARRAY(type, name, size) \
      type LV_FONT_XZL_SCRATCHDATA name[size]
#endif
#endif
#ifndef LV_FONT_XZL_LOCAL_VAR
#define LV_FONT_XZL_LOCAL_VAR(type, name) type LV_FONT_XZL_LOCALDATA name
#endif
#ifndef LV_FONT_XZL_LOCAL_ARRAY
#define LV_FONT_XZL_LOCAL_ARRAY(type, name, size) \
      type LV_FONT_XZL_LOCALDATA name[size]
#endif
#ifndef LV_FONT_XZL_RECORD_VAR
#define LV_FONT_XZL_RECORD_VAR(type, name) type LV_FONT_XZL_RECORDDATA name
#endif
#ifndef LV_FONT_XZL_RECORD_ARRAY
#define LV_FONT_XZL_RECORD_ARRAY(type, name, size) \
      type LV_FONT_XZL_RECORDDATA name[size]
#endif

/*
 * Local pointer-object declarations.  These specify both dimensions:
 *   - what address space the pointer points into, and
 *   - where the pointer variable itself is stored.
 * Function parameters cannot have an independent static storage area, so the
 * *_PTR(type) constructors are used directly in parameter lists.
 */
#ifndef LV_FONT_XZL_LOCAL_RO_PTR_VAR
#define LV_FONT_XZL_LOCAL_RO_PTR_VAR(type, name) \
      LV_FONT_XZL_RO_PTR(type)                   \
      LV_FONT_XZL_LOCALDATA name
#endif
#ifndef LV_FONT_XZL_LOCAL_WORK_PTR_VAR
#define LV_FONT_XZL_LOCAL_WORK_PTR_VAR(type, name) \
      LV_FONT_XZL_WORK_PTR(type)                   \
      LV_FONT_XZL_LOCALDATA name
#endif
#ifndef LV_FONT_XZL_LOCAL_RECORD_PTR_VAR
#define LV_FONT_XZL_LOCAL_RECORD_PTR_VAR(type, name) \
      LV_FONT_XZL_RECORD_PTR(type)                   \
      LV_FONT_XZL_LOCALDATA name
#endif
#ifndef LV_FONT_XZL_LOCAL_PTR_VAR
#define LV_FONT_XZL_LOCAL_PTR_VAR(type, name) \
      LV_FONT_XZL_LOCAL_PTR(type)             \
      LV_FONT_XZL_LOCALDATA name
#endif

/* Read-only program-space access. */
#ifndef LV_FONT_XZL_RO_READ8
#if defined(LV_FONT_XZL_PLATFORM_AVR)
#define LV_FONT_XZL_RO_READ8(p) ((LV_FONT_XZL_UINT8)pgm_read_byte(p))
#else
#define LV_FONT_XZL_RO_READ8(p) (*(p))
#endif
#endif
#ifndef LV_FONT_XZL_RO_READ16
#if defined(LV_FONT_XZL_PLATFORM_AVR)
#define LV_FONT_XZL_RO_READ16(p) ((LV_FONT_XZL_UINT16)pgm_read_word(p))
#else
#define LV_FONT_XZL_RO_READ16(p) (*(p))
#endif
#endif
#ifndef LV_FONT_XZL_RO_ADDR
#define LV_FONT_XZL_RO_ADDR(a, i) (&(a)[(i)])
#endif
#ifndef LV_FONT_XZL_RO_READ8_AT
#define LV_FONT_XZL_RO_READ8_AT(a, i) \
      LV_FONT_XZL_RO_READ8(LV_FONT_XZL_RO_ADDR((a), (i)))
#endif
#ifndef LV_FONT_XZL_RO_READ16_AT
#define LV_FONT_XZL_RO_READ16_AT(a, i) \
      LV_FONT_XZL_RO_READ16(LV_FONT_XZL_RO_ADDR((a), (i)))
#endif

/* Writable work-buffer access. */
#ifndef LV_FONT_XZL_WORK_READ8
#define LV_FONT_XZL_WORK_READ8(p) (*(p))
#endif
#ifndef LV_FONT_XZL_WORK_WRITE8
#define LV_FONT_XZL_WORK_WRITE8(p, v) (*(p) = (LV_FONT_XZL_UINT8)(v))
#endif
#ifndef LV_FONT_XZL_WORK_OR8
#define LV_FONT_XZL_WORK_OR8(p, v) (*(p) |= (LV_FONT_XZL_UINT8)(v))
#endif
#ifndef LV_FONT_XZL_WORK_READ8_AT
#define LV_FONT_XZL_WORK_READ8_AT(a, i) ((a)[(i)])
#endif
#ifndef LV_FONT_XZL_WORK_OR8_AT
#define LV_FONT_XZL_WORK_OR8_AT(a, i, v) \
      ((a)[(i)] |= (LV_FONT_XZL_UINT8)(v))
#endif
#ifndef LV_FONT_XZL_WORK_ZERO
#define LV_FONT_XZL_WORK_ZERO(p, n) memset((p), 0, (n))
#endif

#endif /* LV_FONT_XZL_16_CONFIG_H */
