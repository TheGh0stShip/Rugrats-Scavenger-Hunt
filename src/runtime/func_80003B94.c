/* Target: func_80003B94  VRAM 0x80003B94  ROM 0x4794
 * Size: 0xDC  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: high */
typedef unsigned char u8;
typedef unsigned int u32;

extern u32 func_80003A90(u32 addr);
extern u8 func_80003B14(u32 addr);

void func_80003B94(u32 src, u8 *dst, u32 len) {
    u32 w;

    while (len != 0 && (src & 3)) {
        *dst++ = func_80003B14(src++);
        len--;
    }
    while (len >= 4) {
        w = func_80003A90(src);
        *dst++ = w >> 24;
        *dst++ = w >> 16;
        *dst++ = w >> 8;
        *dst++ = w;
        src += 4;
        len -= 4;
    }
    while (len != 0) {
        *dst++ = func_80003B14(src++);
        len--;
    }
}


