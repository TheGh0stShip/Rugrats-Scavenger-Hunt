/* Target: func_8003ADD0  VRAM 0x8003ADD0  ROM 0x3B9D0
 * Size: 0xC8 (+0x8 padding)  Pipeline: KMC GCC 2.7.2 -O1 -G0 -mips3 -mgp32 -mfp32
 * Confidence: medium */

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef float f32;
typedef double f64;

char *func_8003ADD0(char *dst, const char *src) {
    char *d = dst;
    u32 w;

    if (((u32)src - (u32)dst) & 3) {
        while ((*d++ = *src++) != 0) {
        }
        return dst;
    }
    while (((u32)d & 3) != 0) {
        if ((*d++ = *src++) == 0) {
            return dst;
        }
    }
    for (;;) {
        w = *(u32 *)src;
        src += 4;
        if ((w & 0xFF000000) == 0) {
            *d = 0;
            return dst;
        }
        if ((w & 0x00FF0000) == 0) {
            *(u16 *)d = w >> 16;
            return dst;
        }
        if ((w & 0x0000FF00) == 0) {
            *(u16 *)d = w >> 16;
            d[2] = 0;
            return dst;
        }
        *(u32 *)d = w;
        d += 4;
        if ((w & 0xFF) == 0) {
            return dst;
        }
    }
}

