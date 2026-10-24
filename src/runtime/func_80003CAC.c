/* Target: func_80003CAC  VRAM 0x80003CAC  ROM 0x48AC
 * Size: 0x114  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: medium */
typedef unsigned char u8;
typedef unsigned int u32;

extern u32 func_80003A90(u32 addr);
extern u8 func_80003B14(u32 addr);
extern void func_80003C70(u32 addr, u8 val);

void func_80003CAC(u32 src, u32 dst, u32 len) {
    u32 w;

    while (len != 0 && (src & 3)) {
        func_80003C70(dst++, func_80003B14(src++));
        len--;
    }
    while (len >= 4) {
        w = func_80003A90(src);
        func_80003C70(dst++, w >> 24);
        func_80003C70(dst++, w >> 16);
        func_80003C70(dst++, w >> 8);
        func_80003C70(dst++, w);
        src += 4;
        len -= 4;
    }
    while (len != 0) {
        func_80003C70(dst++, func_80003B14(src++));
        len--;
    }
}


