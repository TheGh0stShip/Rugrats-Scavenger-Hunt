/* Target: func_800138FC  VRAM 0x800138FC  ROM 0x144FC
 * Size: 0xFC  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high
 * Pipeline: kmc -O2 -G0 -mips3 -mgp32 -mfp32 */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

extern u32 D_80052ED0;

extern s32 func_80013AF4(u8 *);
extern s32 func_8001FA40(u8, u16, u16);
extern s32 func_8001FA24(void);

void func_800138FC(u8 *str, s32 x, s32 y) {
    s32 cx;

    while (*str != 0) {
        if (D_80052ED0 & 8) {
            cx = x - func_80013AF4(str) / 2;
        } else if (D_80052ED0 & 0x10) {
            cx = x - func_80013AF4(str);
        } else {
            cx = x;
        }
        while (*str != 0 && *str != '\n') {
            cx += func_8001FA40(*str++, cx, y);
        }
        if (*str == '\n') {
            str++;
            y = y + 4 + func_8001FA24();
        }
    }
}
