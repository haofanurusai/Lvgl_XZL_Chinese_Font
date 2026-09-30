#ifndef OLED_BITMAP16_H
#define OLED_BITMAP16_H

/*
 * Fixed 16x16 / 32-byte bitmap conversion, in place.
 * Input : buf[y*2 + x/8], bit (7 - x%8).
 * Output: buf[(y/8)*16 + x], bit (y%8).
 * Output bytes 0..15 are the top page, 16..31 the bottom page.
 * No loops, LUTs, 32/64-bit arithmetic, or extra 32-byte buffer.
 * Requires a writable 32-byte RAM buffer. Call once per decoded glyph.
 * C89-compatible; unsigned char must be 8 bits (as on Keil C251).
 * Pointer storage/address-space defaults follow the selected memory model.
 */

/* Swap s-bit groups between two row bytes; t is the local scratch byte. */
#define OLED_XPOSE_SWAP(a, b, s, m)                \
    do {                                          \
        t = (unsigned char)(((a) ^ ((b) >> (s))) & (m)); \
        (a) ^= t;                                 \
        (b) ^= (unsigned char)(t << (s));          \
    } while (0)

/* Transpose one 8x8 tile whose row bytes have a stride of two. */
static void oled_xpose8_stride2(unsigned char *p)
{
    unsigned char r0, r1, r2, r3, r4, r5, r6, r7;
    unsigned char t;

    /* Reversed row loads make output bit 0 correspond to the top row. */
    r0 = p[14]; r1 = p[12]; r2 = p[10]; r3 = p[8];
    r4 = p[6];  r5 = p[4];  r6 = p[2];  r7 = p[0];

    OLED_XPOSE_SWAP(r0, r4, 4, 0x0Fu);
    OLED_XPOSE_SWAP(r1, r5, 4, 0x0Fu);
    OLED_XPOSE_SWAP(r2, r6, 4, 0x0Fu);
    OLED_XPOSE_SWAP(r3, r7, 4, 0x0Fu);

    OLED_XPOSE_SWAP(r0, r2, 2, 0x33u);
    OLED_XPOSE_SWAP(r1, r3, 2, 0x33u);
    OLED_XPOSE_SWAP(r4, r6, 2, 0x33u);
    OLED_XPOSE_SWAP(r5, r7, 2, 0x33u);

    OLED_XPOSE_SWAP(r0, r1, 1, 0x55u);
    OLED_XPOSE_SWAP(r2, r3, 1, 0x55u);
    OLED_XPOSE_SWAP(r4, r5, 1, 0x55u);
    OLED_XPOSE_SWAP(r6, r7, 1, 0x55u);

    p[0] = r0; p[2] = r1; p[4] = r2; p[6] = r3;
    p[8] = r4; p[10] = r5; p[12] = r6; p[14] = r7;
}

#undef OLED_XPOSE_SWAP

/* L0,R0,L1,R1,...,L7,R7 -> L0,...,L7,R0,...,R7. */
static void oled_deinterleave16(unsigned char *p)
{
    unsigned char t;

    t = p[1];
    p[1] = p[2]; p[2] = p[4]; p[4] = p[8]; p[8] = t;

    t = p[3];
    p[3] = p[6]; p[6] = p[12]; p[12] = p[9]; p[9] = t;

    t = p[5];
    p[5] = p[10]; p[10] = t;

    t = p[7];
    p[7] = p[14]; p[14] = p[13]; p[13] = p[11]; p[11] = t;
}

static void OLED_Bitmap16ToPages(unsigned char *buf)
{
    oled_xpose8_stride2(buf);
    oled_xpose8_stride2(buf + 1);
    oled_xpose8_stride2(buf + 16);
    oled_xpose8_stride2(buf + 17);
    oled_deinterleave16(buf);
    oled_deinterleave16(buf + 16);
}

#endif /* OLED_BITMAP16_H */
