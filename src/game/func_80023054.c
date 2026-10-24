/* Target: func_80023054  VRAM 0x80023054  ROM 0x23C54
 * Size: 0xBC  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: medium */
typedef signed char s8; typedef unsigned char u8; typedef short s16; typedef unsigned short u16; typedef int s32; typedef unsigned int u32;

extern s32 func_8000E008(s16);
extern s32 func_8003A840(void);
extern s32 func_8000DFE4(s16);
extern s16 func_8000E02C(s16, s32);

s32 func_80023054(void *arg0, s16 arg1) {
    s32 r;

    if (func_8000E008(arg1) != 0) {
        do {
            r = func_8003A840() % func_8000DFE4(arg1);
        } while (func_8000E02C(arg1, r) == -1);
        return r;
    }
    return 0;
}
