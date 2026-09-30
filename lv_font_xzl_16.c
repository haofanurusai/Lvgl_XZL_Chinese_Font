/*
 * Xiuzhenlong GB2312 6763 Hanzi Unicode-root font for LVGL 9.5.x.
 * 64 glyph/page; Unicode gaps are Elias-gamma coded; page-local root descriptors
 * use a 7-entry MTF node selector. Shared component DAG remains in original XZL order.
 */
#include "lv_font_xzl_16.h"
#include "xzl_data.h"
#if !defined(LV_FONT_XZL_CUSTOM_ZERO)
#include <string.h>
#endif

#if defined(LV_FONT_XZL_CUSTOM_ZERO)
static void xzl_zero_record(LV_FONT_XZL_RECORD_PTR(unsigned char) p, unsigned int n) LV_FONT_XZL_REENTRANT
{
    while (n--)
        *p++ = 0;
}

static void xzl_zero_work(LV_FONT_XZL_WORK_PTR(unsigned char) p, unsigned int n) LV_FONT_XZL_REENTRANT
{
    while (n--)
        *p++ = 0;
}

#define XZL_RECORD_ZERO(p, n) xzl_zero_record((LV_FONT_XZL_RECORD_PTR(unsigned char))(p), (unsigned int)(n))
#define XZL_WORK_ZERO(p, n) xzl_zero_work((p), (unsigned int)(n))
#else
#define XZL_RECORD_ZERO(p, n) memset((p), 0, (n))
#define XZL_WORK_ZERO(p, n) LV_FONT_XZL_WORK_ZERO((p), (n))
#endif

typedef struct
{
    LV_FONT_XZL_UINT8 x;
    LV_FONT_XZL_UINT8 y;
    LV_FONT_XZL_UINT8 w;
    LV_FONT_XZL_UINT8 h;
} xzl_geom_t;

typedef struct
{
    LV_FONT_XZL_UINT8 width;
    LV_FONT_XZL_UINT16 node_index;
    LV_FONT_XZL_UINT16 cont;
    LV_FONT_XZL_UINT16 nodeval;
    LV_FONT_XZL_UINT8 special;
    /* Keep this byte-addressable: some old compilers do not allow bit in structs. */
    LV_FONT_XZL_UINT8 node_active;
} xzl_rec_t;

typedef struct
{
    LV_FONT_XZL_UINT8 op;
    LV_FONT_XZL_UINT8 flags;
    LV_FONT_XZL_UINT8 p3;
    LV_FONT_XZL_UINT8 p4;
    LV_FONT_XZL_UINT16 child;
} xzl_node_t;

typedef struct
{
    xzl_geom_t g;
    LV_FONT_XZL_UINT8 ops[LV_FONT_XZL_STACK_MAX];
    LV_FONT_XZL_UINT8 ops_n;
    LV_FONT_XZL_UINT16 conts[LV_FONT_XZL_STACK_MAX];
    LV_FONT_XZL_UINT8 conts_n;
    xzl_geom_t geoms[LV_FONT_XZL_STACK_MAX];
    LV_FONT_XZL_UINT8 geoms_n;
    LV_FONT_XZL_UINT16 current;
    LV_FONT_XZL_UINT16 cont;
    LV_FONT_XZL_UINT8 flags;
    LV_FONT_XZL_UINT8 p2;
    LV_FONT_XZL_UINT8 p3;
    LV_FONT_XZL_UINT8 p4;
    LV_FONT_XZL_WORK_PTR(LV_FONT_XZL_UINT8)
    bm;
} xzl_dec_t;

typedef struct
{
    LV_FONT_XZL_RO_PTR(LV_FONT_XZL_UINT8)
    p;
    LV_FONT_XZL_UINT32 bitpos;
} bitcur_t;

static LV_FONT_XZL_UINT32 bc_get(LV_FONT_XZL_LOCAL_PTR(bitcur_t) c, LV_FONT_XZL_UINT8 n) LV_FONT_XZL_REENTRANT
{
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT32, v);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT32, q);
    v = 0;
    while (n--)
    {
        q = c->bitpos++;
        v = (v << 1) | ((LV_FONT_XZL_RO_READ8_AT(c->p, q >> 3) >> (7u - (q & 7u))) & 1u);
    }
    return v;
}

static void bc_align(LV_FONT_XZL_LOCAL_PTR(bitcur_t) c) LV_FONT_XZL_REENTRANT
{
    c->bitpos = (c->bitpos + 7u) & ~7u;
}

static LV_FONT_XZL_UINT16 bc_gamma(LV_FONT_XZL_LOCAL_PTR(bitcur_t) c) LV_FONT_XZL_REENTRANT
{
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, z);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, v);
    z = 0;
    while (bc_get(c, 1) == 0u)
        z++;
    v = 1u;
    while (z--)
        v = (LV_FONT_XZL_UINT16)((v << 1) | bc_get(c, 1));
    return v;
}

static LV_FONT_XZL_UINT32 bits_arr(LV_FONT_XZL_RO_PTR(LV_FONT_XZL_UINT8) a,
                                   LV_FONT_XZL_UINT32 bitoff,
                                   LV_FONT_XZL_UINT8 n) LV_FONT_XZL_REENTRANT
{
    LV_FONT_XZL_LOCAL_VAR(bitcur_t, c);
    c.p = a;
    c.bitpos = bitoff;
    return bc_get(&c, n);
}

static LV_FONT_XZL_UINT16 be16_arr(LV_FONT_XZL_RO_PTR(LV_FONT_XZL_UINT8) a,
                                   LV_FONT_XZL_UINT16 i) LV_FONT_XZL_REENTRANT
{
    return (LV_FONT_XZL_UINT16)(((LV_FONT_XZL_UINT16)
                                     LV_FONT_XZL_RO_READ8_AT(a, 2u * i)
                                 << 8) |
                                LV_FONT_XZL_RO_READ8_AT(a, 2u * i + 1u));
}

static LV_FONT_XZL_UINT16 p10_value(LV_FONT_XZL_UINT16 i) LV_FONT_XZL_REENTRANT
{
    return (LV_FONT_XZL_UINT16)bits_arr(xzl_p10,
                                        (LV_FONT_XZL_UINT32)i * 11u,
                                        11);
}

static LV_FONT_XZL_UINT16 floor_p10(LV_FONT_XZL_UINT16 target) LV_FONT_XZL_REENTRANT
{
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, lo);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, hi);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, d);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, m);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, v);
    lo = 0;
    hi = 0x0133u;
    while (hi != lo)
    {
        d = (LV_FONT_XZL_UINT16)(hi - lo);
        m = (LV_FONT_XZL_UINT16)(lo + (d >> 1) + (d & 1u));
        v = p10_value(m);
        if (v == target)
            return m;
        if (target > v)
            lo = m;
        else
            hi = (LV_FONT_XZL_UINT16)(m - 1u);
    }
    return lo;
}

static LV_FONT_XZL_UINT16 floor_th10(LV_FONT_XZL_UINT16 target) LV_FONT_XZL_REENTRANT
{
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, lo);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, hi);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, d);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, m);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, v);
    lo = 0;
    hi = 0x0084u;
    while (hi != lo)
    {
        d = (LV_FONT_XZL_UINT16)(hi - lo);
        m = (LV_FONT_XZL_UINT16)(lo + (d >> 1) + (d & 1u));
        v = be16_arr(xzl_th10, m);
        if (v == target)
            return m;
        if (target > v)
            lo = m;
        else
            hi = (LV_FONT_XZL_UINT16)(m - 1u);
    }
    return lo;
}

static LV_FONT_XZL_UINT16 sparse10(LV_FONT_XZL_UINT16 gid) LV_FONT_XZL_REENTRANT
{
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, i);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, v);
    i = floor_p10(gid);
    v = p10_value(i);
    if (v < be16_arr(xzl_th10, 0))
        return i;
    return (LV_FONT_XZL_UINT16)(i + be16_arr(xzl_mp10, floor_th10(v)));
}

static LV_FONT_XZL_BOOL shared_record(LV_FONT_XZL_UINT16 gid, LV_FONT_XZL_RECORD_PTR(xzl_rec_t) r) LV_FONT_XZL_REENTRANT
{
    LV_FONT_XZL_LOCAL_RO_PTR_VAR(LV_FONT_XZL_UINT8, a);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, lo);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, hi);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, w);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT32, val);

    a = NULL;
    lo = 0;
    hi = 0;
    w = 0;
    if (gid >= 0x007Eu && gid <= 0x0086u)
    {
        a = xzl_rec28_a;
        lo = 0x007E;
        hi = 0x0086;
        w = 28;
    }
    else if (gid >= 0x0087u && gid <= 0x0374u)
    {
        a = xzl_rec20_a;
        lo = 0x0087;
        hi = 0x0374;
        w = 20;
    }
    else if (gid >= 0x0375u && gid <= 0x03C5u)
    {
        a = xzl_rec28_b;
        lo = 0x0375;
        hi = 0x03C5;
        w = 28;
    }
    else if (gid >= 0x03C6u && gid <= 0x04E2u)
    {
        a = xzl_rec20_b;
        lo = 0x03C6;
        hi = 0x04E2;
        w = 20;
    }
    else if (gid >= 0x04E3u && gid <= 0x07CEu)
    {
        a = xzl_rec10;
        lo = 0x04E3;
        hi = 0x07CE;
        w = 10;
    }
    else if (gid >= 0x07CFu && gid <= 0x07FFu)
    {
        a = xzl_rec28_c;
        lo = 0x07CF;
        hi = 0x07FF;
        w = 28;
    }
    else
        return 0;

    (void)lo;
    val = bits_arr(a, (LV_FONT_XZL_UINT32)(hi - gid) * w, w);
    XZL_RECORD_ZERO(r, sizeof(*r));
    r->width = w;
    if (w == 28)
    {
        r->special = (LV_FONT_XZL_UINT8)((val >> 22) & 0x3Fu);
        r->nodeval = (LV_FONT_XZL_UINT16)((val >> 11) & 0x7FFu);
        r->cont = (LV_FONT_XZL_UINT16)(val & 0x7FFu);
        r->node_active = 1;
    }
    else if (w == 20)
    {
        r->node_index = (LV_FONT_XZL_UINT16)((val >> 10) & 0x3FFu);
        r->cont = (LV_FONT_XZL_UINT16)(val & 0x3FFu);
        r->node_active = (LV_FONT_XZL_UINT8)(r->node_index != 0);
    }
    else
    {
        r->node_index = sparse10(gid);
        r->cont = (LV_FONT_XZL_UINT16)val;
        r->node_active = (LV_FONT_XZL_UINT8)(r->node_index != 0);
    }
    return 1;
}

static void xzl_node(LV_FONT_XZL_UINT16 ni, LV_FONT_XZL_LOCAL_PTR(xzl_node_t) out) LV_FONT_XZL_REENTRANT
{
    LV_FONT_XZL_LOCAL_RO_PTR_VAR(LV_FONT_XZL_UINT8, p);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT32, v);

    if (ni >= 613u)
    {
        out->op = 0;
        out->flags = 0;
        out->p3 = 0;
        out->p4 = 0;
        out->child = 0;
        return;
    }
    p = LV_FONT_XZL_RO_ADDR(xzl_nodes, 4u * ni);
    v = ((LV_FONT_XZL_UINT32)LV_FONT_XZL_RO_READ8_AT(p, 0) << 24) |
        ((LV_FONT_XZL_UINT32)LV_FONT_XZL_RO_READ8_AT(p, 1) << 16) |
        ((LV_FONT_XZL_UINT32)LV_FONT_XZL_RO_READ8_AT(p, 2) << 8) |
        LV_FONT_XZL_RO_READ8_AT(p, 3);
    out->op = (LV_FONT_XZL_UINT8)((v >> 27) & 31u);
    out->flags = (LV_FONT_XZL_UINT8)((v >> 21) & 63u);
    out->p3 = (LV_FONT_XZL_UINT8)((v >> 16) & 31u);
    out->p4 = (LV_FONT_XZL_UINT8)((v >> 11) & 31u);
    out->child = (LV_FONT_XZL_UINT16)(v & 0x7FFu);
}

#define XZL_NODE(ni, n) xzl_node((ni), &(n))

static LV_FONT_XZL_UINT16 page_glyph_count(LV_FONT_XZL_UINT16 pg) LV_FONT_XZL_REENTRANT
{
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, first);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, rem);
    first = (LV_FONT_XZL_UINT16)(pg * XZL_PAGE_GLYPHS);
    rem = (LV_FONT_XZL_UINT16)(XZL_HANZI_COUNT - first);
    return rem < XZL_PAGE_GLYPHS ? rem : XZL_PAGE_GLYPHS;
}

/* Exact Unicode membership + rank lookup. No polynomial predictor. */
static LV_FONT_XZL_UINT16 unicode_to_rank(LV_FONT_XZL_UINT32 cp) LV_FONT_XZL_REENTRANT
{
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, lo);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, hi);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, m);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, pg);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, n);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, i);
    LV_FONT_XZL_LOCAL_VAR(bitcur_t, c);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT32, u);

    if (cp < XZL_UNICODE_FIRST || cp > XZL_UNICODE_LAST)
        return 0xFFFFu;
    lo = 0;
    hi = (LV_FONT_XZL_UINT16)(XZL_PAGE_COUNT - 1u);
    if (cp < LV_FONT_XZL_RO_READ16_AT(xzl_page_first_unicode, 0))
        return 0xFFFFu;
    while (lo < hi)
    {
        m = (LV_FONT_XZL_UINT16)((lo + hi + 1u) >> 1);
        if ((LV_FONT_XZL_UINT32)LV_FONT_XZL_RO_READ16_AT(xzl_page_first_unicode, m) <= cp)
            lo = m;
        else
            hi = (LV_FONT_XZL_UINT16)(m - 1u);
    }
    pg = lo;
    n = page_glyph_count(pg);
    c.p = LV_FONT_XZL_RO_ADDR(xzl_page_stream, LV_FONT_XZL_RO_READ16_AT(xzl_page_offsets, pg));
    c.bitpos = 0;
    u = LV_FONT_XZL_RO_READ16_AT(xzl_page_first_unicode, pg);
    if (u == cp)
        return (LV_FONT_XZL_UINT16)(pg * XZL_PAGE_GLYPHS);
    for (i = 1; i < n; i++)
    {
        u += bc_gamma(&c);
        if (u == cp)
            return (LV_FONT_XZL_UINT16)(pg * XZL_PAGE_GLYPHS + i);
        if (u > cp)
            return 0xFFFFu;
    }
    return 0xFFFFu;
}

static void mtf_front(LV_FONT_XZL_SCRATCH_PTR(LV_FONT_XZL_UINT16) mtf,
                      LV_FONT_XZL_LOCAL_PTR(LV_FONT_XZL_UINT8) n,
                      LV_FONT_XZL_UINT16 v,
                      int old) LV_FONT_XZL_REENTRANT
{
    LV_FONT_XZL_LOCAL_VAR(int, i);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, lim);

    if (old >= 0)
    {
        for (i = old; i > 0; i--)
            mtf[i] = mtf[i - 1];
        mtf[0] = v;
        return;
    }
    lim = *n < 7u ? *n : 6u;
    for (i = lim; i > 0; i--)
        mtf[i] = mtf[i - 1];
    mtf[0] = v;
    if (*n < 7u)
        (*n)++;
}

/* Decode the synthetic Unicode-order root descriptor for one rank. */
static LV_FONT_XZL_BOOL root_record(LV_FONT_XZL_UINT16 rank, LV_FONT_XZL_RECORD_PTR(xzl_rec_t) r) LV_FONT_XZL_REENTRANT
{
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, pg);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, local);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, n);
    LV_FONT_XZL_LOCAL_VAR(bitcur_t, c);
    LV_FONT_XZL_SCRATCH_ARRAY(LV_FONT_XZL_UINT16, mtf, 7);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, mn);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, i);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, a);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, ones);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, ni);
    LV_FONT_XZL_LOCAL_VAR(int, old);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, k);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, b);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, gid);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, branch);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT32, val);

    if (rank >= XZL_HANZI_COUNT)
        return 0;
    pg = (LV_FONT_XZL_UINT16)(rank >> 6);
    local = (LV_FONT_XZL_UINT16)(rank & 63u);
    n = page_glyph_count(pg);
    c.p = LV_FONT_XZL_RO_ADDR(xzl_page_stream, LV_FONT_XZL_RO_READ16_AT(xzl_page_offsets, pg));
    c.bitpos = 0;
    for (i = 1; i < n; i++)
        (void)bc_gamma(&c);
    bc_align(&c);
    XZL_WORK_ZERO(mtf, sizeof(mtf));
    mn = 0;

    for (i = 0; i <= local; i++)
    {
        XZL_RECORD_ZERO(r, sizeof(*r));
        a = (LV_FONT_XZL_UINT8)bc_get(&c, 1);
        if (a == 0)
        {
            r->width = 11;
            r->cont = (LV_FONT_XZL_UINT16)bc_get(&c, 11);
            ones = 0;
            while (ones < 7u && bc_get(&c, 1) != 0u)
                ones++;
            if (ones < 7u)
            {
                if (ones >= mn)
                    return 0;
                ni = mtf[ones];
                mtf_front(mtf, &mn, ni, ones);
            }
            else
            {
                ni = (LV_FONT_XZL_UINT16)bc_get(&c, 10);
                old = -1;
                for (k = 0; k < mn; k++)
                {
                    if (mtf[k] == ni)
                    {
                        old = k;
                        break;
                    }
                }
                mtf_front(mtf, &mn, ni, old);
            }
            r->node_index = ni;
            r->node_active = 1;
        }
        else
        {
            b = (LV_FONT_XZL_UINT8)bc_get(&c, 1);
            if (b == 0)
            {
                gid = (LV_FONT_XZL_UINT16)bc_get(&c, 11);
                if (!shared_record(gid, r))
                    return 0;
            }
            else
            {
                branch = (LV_FONT_XZL_UINT8)bc_get(&c, 1);
                if (branch == 0)
                {
                    val = bc_get(&c, 28);
                    r->width = 28;
                    r->special = (LV_FONT_XZL_UINT8)((val >> 22) & 0x3Fu);
                    r->nodeval = (LV_FONT_XZL_UINT16)((val >> 11) & 0x7FFu);
                    r->cont = (LV_FONT_XZL_UINT16)(val & 0x7FFu);
                    r->node_active = 1;
                }
                else
                {
                    r->width = 11;
                    r->cont = (LV_FONT_XZL_UINT16)bc_get(&c, 11);
                    r->node_active = 0;
                }
            }
        }
    }
    return 1;
}

static LV_FONT_XZL_UINT8 sx_layout(LV_FONT_XZL_WORK_CONST_PTR(xzl_dec_t) d, LV_FONT_XZL_UINT8 a) LV_FONT_XZL_REENTRANT
{
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, v);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, q);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, r);
    v = (LV_FONT_XZL_UINT16)a * d->g.w;
    q = (LV_FONT_XZL_UINT8)(v / 15u);
    r = (LV_FONT_XZL_UINT8)(v % 15u);
    q = (LV_FONT_XZL_UINT8)(q + (r >= 8u));
    return q ? q : 1;
}

static LV_FONT_XZL_UINT8 sy_layout(LV_FONT_XZL_WORK_CONST_PTR(xzl_dec_t) d, LV_FONT_XZL_UINT8 a) LV_FONT_XZL_REENTRANT
{
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, v);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, q);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, r);
    v = (LV_FONT_XZL_UINT16)a * d->g.h;
    q = (LV_FONT_XZL_UINT8)(v / 16u);
    r = (LV_FONT_XZL_UINT8)(v % 16u);
    q = (LV_FONT_XZL_UINT8)(q + (r >= 8u));
    return q ? q : 1;
}

static LV_FONT_XZL_UINT8 sx_line(LV_FONT_XZL_WORK_CONST_PTR(xzl_dec_t) d, LV_FONT_XZL_UINT8 a) LV_FONT_XZL_REENTRANT
{
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, v);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, q);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, r);
    v = (LV_FONT_XZL_UINT16)a * d->g.w;
    q = (LV_FONT_XZL_UINT8)(v / 15u);
    r = (LV_FONT_XZL_UINT8)(v % 15u);
    q = (LV_FONT_XZL_UINT8)(q + (r >= 9u));
    return q ? q : 1;
}

static LV_FONT_XZL_UINT8 sy_line(LV_FONT_XZL_WORK_CONST_PTR(xzl_dec_t) d, LV_FONT_XZL_UINT8 a) LV_FONT_XZL_REENTRANT
{
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, v);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, q);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, r);
    v = (LV_FONT_XZL_UINT16)a * d->g.h;
    q = (LV_FONT_XZL_UINT8)(v / 16u);
    r = (LV_FONT_XZL_UINT8)(v % 16u);
    q = (LV_FONT_XZL_UINT8)(q + (r >= 9u));
    return q ? q : 1;
}

/* Explicit outputs avoid dependence on C51 aggregate-return temporaries. */
static void layout(LV_FONT_XZL_WORK_PTR(xzl_dec_t) d, LV_FONT_XZL_UINT8 op,
                   LV_FONT_XZL_LOCAL_PTR(xzl_geom_t) out) LV_FONT_XZL_REENTRANT
{
    LV_FONT_XZL_LOCAL_WORK_PTR_VAR(xzl_geom_t, g);
    LV_FONT_XZL_LOCAL_VAR(xzl_geom_t, c);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, p2);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, p3);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, p4);

    g = &d->g;
    c = *g;
    p2 = d->p2;
    p3 = d->p3;
    p4 = d->p4;
    switch (op)
    {
    case 0:
    case 18:
        c.x = (LV_FONT_XZL_UINT8)(g->x + sx_layout(d, p3));
        c.w = sx_layout(d, p2);
        break;
    case 1:
    case 2:
        c.x = (LV_FONT_XZL_UINT8)(g->x + sx_layout(d, (LV_FONT_XZL_UINT8)(15 - p3)));
        c.w = sx_layout(d, p3);
        g->w = sx_layout(d, p2);
        break;
    case 3:
    case 4:
        c.h = sy_layout(d, p3);
        g->y = (LV_FONT_XZL_UINT8)(g->y + sy_layout(d, (LV_FONT_XZL_UINT8)(16 - p2)));
        g->h = sy_layout(d, p2);
        break;
    case 5:
    case 13:
        c.x = (LV_FONT_XZL_UINT8)(g->x + sx_layout(d, (LV_FONT_XZL_UINT8)((15 - p4) / 2)));
        c.w = sx_layout(d, p4);
        c.h = sy_layout(d, p3);
        g->y = (LV_FONT_XZL_UINT8)(g->y + sy_layout(d, (LV_FONT_XZL_UINT8)(16 - p2)));
        g->h = sy_layout(d, p2);
        break;
    case 6:
        c.x = (LV_FONT_XZL_UINT8)(g->x + sx_layout(d, p2));
        c.y = (LV_FONT_XZL_UINT8)(g->y + sy_layout(d, p3));
        c.w = sx_layout(d, (LV_FONT_XZL_UINT8)(15 - p2));
        c.h = sy_layout(d, (LV_FONT_XZL_UINT8)(16 - p3));
        break;
    case 7:
        c.x = (LV_FONT_XZL_UINT8)(g->x + sx_layout(d, p4));
        c.w = sx_layout(d, p2);
        c.h = sy_layout(d, p3);
        break;
    case 8:
        c.x = (LV_FONT_XZL_UINT8)(g->x + sx_layout(d, p2));
        c.w = sx_layout(d, (LV_FONT_XZL_UINT8)(15 - p2));
        c.h = sy_layout(d, p3);
        break;
    case 9:
        c.x = (LV_FONT_XZL_UINT8)(g->x + sx_layout(d, (LV_FONT_XZL_UINT8)((15 - p2) / 2)));
        c.y = (LV_FONT_XZL_UINT8)(g->y + sy_layout(d, (LV_FONT_XZL_UINT8)(p4 - p3)));
        c.w = sx_layout(d, p2);
        c.h = sy_layout(d, p3);
        break;
    case 10:
        c.x = (LV_FONT_XZL_UINT8)(g->x + sx_layout(d, (LV_FONT_XZL_UINT8)(15 - p2)));
        c.y = (LV_FONT_XZL_UINT8)(g->y + sy_layout(d, p4));
        c.w = sx_layout(d, p2);
        c.h = sy_layout(d, p3);
        break;
    case 11:
        c.w = sx_layout(d, p2);
        c.h = sy_layout(d, p3);
        break;
    case 12:
        c.x = (LV_FONT_XZL_UINT8)(g->x + sx_layout(d, p4));
        c.y = (LV_FONT_XZL_UINT8)(g->y + sy_layout(d, (LV_FONT_XZL_UINT8)(16 - p3)));
        c.w = sx_layout(d, p2);
        c.h = sy_layout(d, p3);
        break;
    case 14:
        c.w = sx_layout(d, p4);
        c.h = sy_layout(d, p3);
        g->y = (LV_FONT_XZL_UINT8)(g->y + sy_layout(d, (LV_FONT_XZL_UINT8)(16 - p2)));
        g->h = sy_layout(d, p2);
        break;
    case 15:
        c.x = (LV_FONT_XZL_UINT8)(g->x + sx_layout(d, (LV_FONT_XZL_UINT8)(15 - p4)));
        c.w = sx_layout(d, p4);
        c.h = sy_layout(d, p3);
        g->y = (LV_FONT_XZL_UINT8)(g->y + sy_layout(d, (LV_FONT_XZL_UINT8)(16 - p2)));
        g->h = sy_layout(d, p2);
        break;
    case 16:
        c.x = (LV_FONT_XZL_UINT8)(g->x + sx_layout(d, p2));
        c.w = sx_layout(d, p3);
        c.h = sy_layout(d, p4);
        break;
    case 17:
        c.y = (LV_FONT_XZL_UINT8)(g->y + sy_layout(d, p2));
        c.h = sy_layout(d, p3);
        break;
    default:
        break;
    }
    /* These variants exchange parent and child after the same calculation.
     * Keep layout nonrecursive so C51 can safely overlay its local variables.
     */
    if (op == 2 || op == 4 || op == 13)
    {
        *out = d->g;
        d->g = c;
    }
    else
        *out = c;
}

#define XZL_LAYOUT(d, op, c) layout((d), (op), &(c))

static void push_op(LV_FONT_XZL_WORK_PTR(xzl_dec_t) d, LV_FONT_XZL_UINT8 v) LV_FONT_XZL_REENTRANT
{
    if (d->ops_n < LV_FONT_XZL_STACK_MAX)
        d->ops[d->ops_n++] = v;
}

static void push_cont(LV_FONT_XZL_WORK_PTR(xzl_dec_t) d, LV_FONT_XZL_UINT16 v) LV_FONT_XZL_REENTRANT
{
    if (d->conts_n < LV_FONT_XZL_STACK_MAX)
        d->conts[d->conts_n++] = v;
}

static LV_FONT_XZL_UINT16 pop_cont(LV_FONT_XZL_WORK_PTR(xzl_dec_t) d) LV_FONT_XZL_REENTRANT
{
    return d->conts_n ? d->conts[--d->conts_n] : 0;
}

/* The source can be a LOCAL object or d->g in WORK storage.  A generic
 * pointer deliberately accepts both C51 address spaces without a cast.
 */
static void push_geom(LV_FONT_XZL_WORK_PTR(xzl_dec_t) d, const xzl_geom_t *v) LV_FONT_XZL_REENTRANT
{
    if (d->geoms_n < LV_FONT_XZL_STACK_MAX)
        d->geoms[d->geoms_n++] = *v;
}

static void pop_geom_to_current(LV_FONT_XZL_WORK_PTR(xzl_dec_t) d) LV_FONT_XZL_REENTRANT
{
    if (d->geoms_n)
        d->g = d->geoms[--d->geoms_n];
    else
    {
        d->g.x = 0;
        d->g.y = 0;
        d->g.w = 15;
        d->g.h = 16;
    }
}

#define XZL_POP_CONT_TO_CURRENT(d) ((d)->current = pop_cont((d)))
#define XZL_PUSH_GEOM(d, v) push_geom((d), &(v))
#define XZL_POP_GEOM_TO_CURRENT(d) pop_geom_to_current((d))

static void setpx(LV_FONT_XZL_WORK_PTR(xzl_dec_t) d, int x, int y) LV_FONT_XZL_REENTRANT
{
    LV_FONT_XZL_LOCAL_VAR(int, yt);
    LV_FONT_XZL_LOCAL_VAR(int, xx);
    if (x < 1 || x > 16 || y < 1 || y > 16)
        return;
    yt = 16 - y;
    xx = x - 1;
    LV_FONT_XZL_WORK_OR8_AT(d->bm, yt * 2 + (xx >> 3),
                            (LV_FONT_XZL_UINT8)(0x80u >> (xx & 7)));
}

static void hline(LV_FONT_XZL_WORK_PTR(xzl_dec_t) d, int x1, int y, int x2) LV_FONT_XZL_REENTRANT
{
    LV_FONT_XZL_LOCAL_VAR(int, t);
    LV_FONT_XZL_LOCAL_VAR(int, x);
    if (x1 > x2)
    {
        t = x1;
        x1 = x2;
        x2 = t;
    }
    for (x = x1; x <= x2; x++)
        setpx(d, x, y);
}

static void vline(LV_FONT_XZL_WORK_PTR(xzl_dec_t) d, int x, int y1, int y2) LV_FONT_XZL_REENTRANT
{
    LV_FONT_XZL_LOCAL_VAR(int, t);
    LV_FONT_XZL_LOCAL_VAR(int, y);
    if (y1 < y2)
    {
        t = y1;
        y1 = y2;
        y2 = t;
    }
    for (y = y1; y >= y2; y--)
        setpx(d, x, y);
}

static void diagp(LV_FONT_XZL_WORK_PTR(xzl_dec_t) d, int x1, int y1, int x2, int y2) LV_FONT_XZL_REENTRANT
{
    LV_FONT_XZL_LOCAL_VAR(int, t);
    LV_FONT_XZL_LOCAL_VAR(int, i);
    if (x2 < x1)
    {
        t = x1;
        x1 = x2;
        x2 = t;
        t = y1;
        y1 = y2;
        y2 = t;
    }
    for (i = 0; i <= x2 - x1; i++)
        setpx(d, x1 + i, y1 + i);
}

static void diagn(LV_FONT_XZL_WORK_PTR(xzl_dec_t) d, int x1, int y1, int x2, int y2) LV_FONT_XZL_REENTRANT
{
    LV_FONT_XZL_LOCAL_VAR(int, t);
    LV_FONT_XZL_LOCAL_VAR(int, i);
    if (x2 < x1)
    {
        t = x1;
        x1 = x2;
        x2 = t;
        t = y1;
        y1 = y2;
        y2 = t;
    }
    for (i = 0; i <= x2 - x1; i++)
        setpx(d, x1 + i, y1 - i);
}

static void raster_line(LV_FONT_XZL_WORK_PTR(xzl_dec_t) d, int x1, int y1, int x2, int y2) LV_FONT_XZL_REENTRANT
{
    LV_FONT_XZL_LOCAL_VAR(int, dx);
    LV_FONT_XZL_LOCAL_VAR(int, dy);
    LV_FONT_XZL_LOCAL_VAR(int, t);
    LV_FONT_XZL_LOCAL_VAR(int, s);
    LV_FONT_XZL_LOCAL_VAR(int, ym);
    LV_FONT_XZL_LOCAL_VAR(int, xm);

    dx = x2 - x1;
    dy = y2 - y1;
    if (dx < 0)
    {
        t = x1;
        x1 = x2;
        x2 = t;
        t = y1;
        y1 = y2;
        y2 = t;
        dx = -dx;
        dy = -dy;
    }
    if (dx == 0)
    {
        vline(d, x1, y1, y2);
        return;
    }
    if (dy == 0)
    {
        hline(d, x1, y1, x2);
        return;
    }
    if (dy < 0)
    {
        s = dx + dy;
        if (s == 0)
            diagn(d, x1, y1, x2, y2);
        else if (s < 0)
        {
            ym = y2 + dx;
            vline(d, x1, y1, ym);
            diagn(d, x1, ym, x2, y2);
        }
        else
        {
            xm = x1 - dy;
            diagn(d, x1, y1, xm, y2);
            hline(d, xm, y2, x2);
        }
    }
    else
    {
        if (dx == dy)
            diagp(d, x1, y1, x2, y2);
        else if (dx > dy)
        {
            xm = x2 - dy;
            hline(d, x1, y1, xm);
            diagp(d, xm, y1, x2, y2);
        }
        else
        {
            ym = y1 + dx;
            diagp(d, x1, y1, x2, ym);
            vline(d, x2, ym, y2);
        }
    }
}

static LV_FONT_XZL_UINT16 rd_le16(LV_FONT_XZL_RO_PTR(LV_FONT_XZL_UINT8) p) LV_FONT_XZL_REENTRANT
{
    return (LV_FONT_XZL_UINT16)(LV_FONT_XZL_RO_READ8_AT(p, 0) |
                                ((LV_FONT_XZL_UINT16)LV_FONT_XZL_RO_READ8_AT(p, 1) << 8));
}

static void draw_primitive(LV_FONT_XZL_WORK_PTR(xzl_dec_t) d, LV_FONT_XZL_UINT16 prim_code) LV_FONT_XZL_REENTRANT
{
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, variant);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, group);
    LV_FONT_XZL_LOCAL_RO_PTR_VAR(LV_FONT_XZL_UINT8, q);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, count);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, base);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT32, po);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, i);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, a);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, z);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, x1);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, y1);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, x2);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, y2);

    if ((LV_FONT_XZL_UINT8)(d->g.y + d->g.h) == 17u)
        d->g.y--;
    variant = (LV_FONT_XZL_UINT8)(prim_code & 31u);
    group = (LV_FONT_XZL_UINT8)(prim_code >> 5);
    if (group >= 28)
        return;
    q = LV_FONT_XZL_RO_ADDR(xzl_prim_desc, 3u * group);
    count = LV_FONT_XZL_RO_READ8_AT(q, 0);
    base = rd_le16(q + 1);
    po = (LV_FONT_XZL_UINT32)(base - 0x34DCu) +
         (LV_FONT_XZL_UINT32)variant * count * 2u;
    if (po + (LV_FONT_XZL_UINT32)count * 2u > sizeof(xzl_primitives))
        return;
    for (i = 0; i < count; i++)
    {
        a = LV_FONT_XZL_RO_READ8_AT(xzl_primitives, po + 2u * i);
        z = LV_FONT_XZL_RO_READ8_AT(xzl_primitives, po + 2u * i + 1u);
        x1 = (LV_FONT_XZL_UINT8)((a >> 4) + 1);
        y1 = (LV_FONT_XZL_UINT8)((a & 15) + 1);
        x2 = (LV_FONT_XZL_UINT8)((z >> 4) + 1);
        y2 = (LV_FONT_XZL_UINT8)((z & 15) + 1);
        raster_line(d,
                    sx_line(d, x1) + d->g.x,
                    sy_line(d, y1) + d->g.y,
                    sx_line(d, x2) + d->g.x,
                    sy_line(d, y2) + d->g.y);
    }
}

static void special28(LV_FONT_XZL_WORK_PTR(xzl_dec_t) d, LV_FONT_XZL_RECORD_CONST_PTR(xzl_rec_t) r) LV_FONT_XZL_REENTRANT
{
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, v);
    LV_FONT_XZL_LOCAL_VAR(xzl_geom_t, c);
    push_op(d, 1);
    push_cont(d, r->cont);
    d->current = r->nodeval;
    v = r->special;
    if (v & 0x20u)
    {
        d->p2 = (LV_FONT_XZL_UINT8)(v & 31u);
        d->p3 = (LV_FONT_XZL_UINT8)(16 - d->p2);
        XZL_LAYOUT(d, 3, c);
    }
    else
    {
        d->p2 = v;
        d->p3 = (LV_FONT_XZL_UINT8)(15 - v);
        XZL_LAYOUT(d, 1, c);
    }
    XZL_PUSH_GEOM(d, c);
}

static void special_type(LV_FONT_XZL_WORK_PTR(xzl_dec_t) d, LV_FONT_XZL_UINT8 op) LV_FONT_XZL_REENTRANT
{
    LV_FONT_XZL_LOCAL_VAR(xzl_geom_t, c);
    if (op == 0x13)
    {
        push_op(d, 1);
        push_cont(d, d->cont);
        d->current = d->cont;
        XZL_LAYOUT(d, 1, c);
        XZL_PUSH_GEOM(d, c);
    }
    else if (op == 0x14)
    {
        push_op(d, 1);
        push_cont(d, d->cont);
        d->current = d->cont;
        XZL_LAYOUT(d, 3, c);
        XZL_PUSH_GEOM(d, c);
    }
    else if (op == 0x15)
    {
        LV_FONT_XZL_LOCAL_VAR(xzl_geom_t, c);
        LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, nx);
        push_op(d, 3);
        push_cont(d, d->current);
        d->current = d->cont;
        XZL_LAYOUT(d, 1, c);
        XZL_PUSH_GEOM(d, c);
        XZL_PUSH_GEOM(d, d->g);
        nx = (LV_FONT_XZL_UINT8)(d->g.x + d->g.w);
        d->g.x = nx;
        d->g.w = (LV_FONT_XZL_UINT8)(c.x - nx);
    }
    else if (op == 0x16)
    {
        LV_FONT_XZL_LOCAL_VAR(xzl_geom_t, c);
        LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, ny);
        LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, old_y);
        push_op(d, 3);
        push_cont(d, d->current);
        d->current = d->cont;
        XZL_LAYOUT(d, 3, c);
        XZL_PUSH_GEOM(d, c);
        XZL_PUSH_GEOM(d, d->g);
        ny = (LV_FONT_XZL_UINT8)(c.y + c.h);
        old_y = d->g.y;
        d->g.y = ny;
        d->g.h = (LV_FONT_XZL_UINT8)(old_y - ny);
    }
    else
    {
        LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, savep3);
        LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, savep4);
        LV_FONT_XZL_LOCAL_VAR(xzl_geom_t, saved);
        LV_FONT_XZL_LOCAL_VAR(xzl_geom_t, c2);
        push_op(d, 6);
        savep3 = d->p3;
        savep4 = d->p4;
        d->p3 = (LV_FONT_XZL_UINT8)(16 - d->p2);
        XZL_LAYOUT(d, 4, saved);
        d->p2 = savep3;
        d->p3 = savep4;
        XZL_LAYOUT(d, 1, c2);
        XZL_PUSH_GEOM(d, c2);
        XZL_PUSH_GEOM(d, d->g);
        d->g.x = saved.x;
        d->g.y = saved.y;
        d->g.w = saved.w;
        d->g.h = saved.h;
        push_cont(d, d->cont);
        d->current = d->cont;
    }
}

static LV_FONT_XZL_BOOL xzl_decode_rank(LV_FONT_XZL_UINT16 rank,
                                        LV_FONT_XZL_WORK_PTR(LV_FONT_XZL_UINT8) bm) LV_FONT_XZL_REENTRANT
{
    LV_FONT_XZL_WORK_VAR(xzl_dec_t, d);
    LV_FONT_XZL_RECORD_VAR(xzl_rec_t, pending);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_BOOL, first);
    LV_FONT_XZL_LOCAL_VAR(unsigned int, guard);

    XZL_WORK_ZERO(bm, 32);
    XZL_WORK_ZERO(&d, sizeof(d));
    d.g.x = 0;
    d.g.y = 0;
    d.g.w = 15;
    d.g.h = 16;
    d.bm = bm;
    if (!root_record(rank, &pending))
        return 0;
    first = 1;
    guard = 0;

    for (;;)
    {
        if (++guard > 1000)
            return 0;
        {
            LV_FONT_XZL_RECORD_VAR(xzl_rec_t, r);
            if (first)
            {
                r = pending;
                first = 0;
            }
            else if (!shared_record(d.current, &r))
                return 0;
            d.cont = r.cont;
            if (r.width == 28)
            {
                special28(&d, &r);
                continue;
            }
            if (!r.node_active)
                draw_primitive(&d, r.cont);
            else
            {
                LV_FONT_XZL_LOCAL_VAR(xzl_node_t, n);
                XZL_NODE(r.node_index, n);
                d.flags = n.flags;
                d.p2 = (LV_FONT_XZL_UINT8)(n.flags & 31u);
                d.p3 = n.p3;
                d.p4 = n.p4;
                d.current = n.child;
                if (n.op >= 0x13)
                {
                    special_type(&d, n.op);
                    continue;
                }
                {
                    LV_FONT_XZL_LOCAL_VAR(xzl_geom_t, child);
                    XZL_LAYOUT(&d, n.op, child);
                    push_op(&d, 1);
                    push_cont(&d, r.cont);
                    XZL_PUSH_GEOM(&d, child);
                }
                if (d.flags & 0x20u)
                    continue;
                draw_primitive(&d, d.current);
            }
        }
        for (;;)
        {
            if (!d.ops_n)
                return 1;
            {
                LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT8, s);
                s = d.ops[d.ops_n - 1];
                if (s == 2 || s == 5 || s == 8)
                {
                    d.ops_n--;
                    continue;
                }
                if (s == 1)
                {
                    d.ops[d.ops_n - 1] = 2;
                    XZL_POP_CONT_TO_CURRENT(&d);
                    XZL_POP_GEOM_TO_CURRENT(&d);
                    break;
                }
                if (s == 3)
                {
                    LV_FONT_XZL_RECORD_VAR(xzl_rec_t, rr);
                    d.ops[d.ops_n - 1] = 4;
                    XZL_POP_CONT_TO_CURRENT(&d);
                    if (!shared_record(d.current, &rr))
                        return 0;
                    if (rr.width == 28)
                    {
                        d.current = rr.nodeval;
                        push_cont(&d, rr.cont);
                    }
                    else
                    {
                        d.current = rr.cont;
                        push_cont(&d, d.current);
                    }
                    XZL_POP_GEOM_TO_CURRENT(&d);
                    break;
                }
                if (s == 6)
                {
                    d.ops[d.ops_n - 1] = 7;
                    d.current = d.conts[d.conts_n - 1];
                    XZL_POP_GEOM_TO_CURRENT(&d);
                    break;
                }
                if (s == 4)
                {
                    d.ops[d.ops_n - 1] = 5;
                    XZL_POP_CONT_TO_CURRENT(&d);
                    XZL_POP_GEOM_TO_CURRENT(&d);
                    break;
                }
                d.ops[d.ops_n - 1] = 8;
                XZL_POP_CONT_TO_CURRENT(&d);
                XZL_POP_GEOM_TO_CURRENT(&d);
                break;
            }
        }
    }
}

static bool xzl_get_glyph_dsc(const lv_font_t *font,
                              lv_font_glyph_dsc_t *o,
                              LV_FONT_XZL_UINT32 letter,
                              LV_FONT_XZL_UINT32 next) LV_FONT_XZL_REENTRANT
{
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, rank);
    (void)font;
    (void)next;
    rank = unicode_to_rank(letter);
    if (rank == 0xFFFFu)
        return 0;
    o->adv_w = 16;
    o->box_w = 16;
    o->box_h = 16;
    o->ofs_x = 0;
    o->ofs_y = 0;
    o->stride = 0;
    o->format = LV_FONT_GLYPH_FORMAT_A1;
    o->is_placeholder = 0;
    o->gid.index = (LV_FONT_XZL_UINT32)rank + 1u;
    return 1;
}

static const void *xzl_get_bitmap(lv_font_glyph_dsc_t *g, lv_draw_buf_t *draw_buf) LV_FONT_XZL_REENTRANT
{
    LV_FONT_XZL_WORK_ARRAY(LV_FONT_XZL_UINT8, bm, 32);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT32, st);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT32, y);
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT32, x);

    if (!draw_buf || g->gid.index == 0)
        return NULL;
    if (!xzl_decode_rank((LV_FONT_XZL_UINT16)(g->gid.index - 1u), bm))
        return NULL;
    st = draw_buf->header.stride;
    for (y = 0; y < 16; y++)
    {
        for (x = 0; x < 16; x++)
        {
#if defined(LV_FONT_XZL_PLATFORM_KEIL_C51) || defined(LV_FONT_XZL_PLATFORM_KEIL_C251)
            draw_buf->dat_[y * st + x] =
#else
            draw_buf->data[y * st + x] =
#endif
                (LV_FONT_XZL_WORK_READ8_AT(bm, y * 2 + (x >> 3)) &
                 (0x80u >> (x & 7u)))
                    ? 0xFFu
                    : 0;
        }
    }
    lv_draw_buf_flush_cache(draw_buf, NULL);
    return draw_buf;
}

LV_FONT_XZL_BOOL xzl_unicode_to_bitmap(
    LV_FONT_XZL_UINT32 cp,
    LV_FONT_XZL_WORK_PTR(LV_FONT_XZL_UINT8) buf) LV_FONT_XZL_REENTRANT
{
    LV_FONT_XZL_LOCAL_VAR(LV_FONT_XZL_UINT16, rank);

    return (buf != NULL) &&
           ((rank = unicode_to_rank(cp)) != 0xFFFFu) &&
           xzl_decode_rank(rank, buf);
}

/* LVGL 9.5.x field order; positional initialization keeps this C89-compatible. */
const lv_font_t lv_font_xzl_16 = {
    xzl_get_glyph_dsc,
    xzl_get_bitmap,
    NULL,
    18,
    2,
    LV_FONT_SUBPX_NONE,
    LV_FONT_KERNING_NONE,
    0,
    0,
    0,
    NULL,
    NULL,
    NULL};
