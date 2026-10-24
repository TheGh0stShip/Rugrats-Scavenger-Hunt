/* Target: func_8001A65C  VRAM 0x8001A65C  ROM 0x1B25C
 * Size: 0xF8  Compiler: SN GCC 2.8 (-O2), reviewed scheduling normalization */
typedef unsigned char u8;
typedef signed char s8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;

extern u32 D_80052F18;
extern s32 D_8005B318;
extern char D_800016E4[];
extern char D_80001798[];
extern char D_800017F4[];
extern char D_8000187C[];
extern int func_8001F588(s32);
extern void func_800137E8(s32);
extern void func_80013724(void);
extern void func_800138FC(char *, s32, s32);
extern void func_800139F8(char *, s32, s32);
extern void func_80033144(s32, s32, s32, s32);
extern void *func_80032870(s32);
extern void func_80032A20(void *, s32, s32);
extern s32 func_8001C340(void);
extern void func_8001BED8(s32);
extern void func_8001DEE8(void);

void func_8001A65C(void) {
    char *str;

    func_8001F588(0);
    func_800137E8(4);
    func_80013724();
    switch (D_80052F18) {
        case 0:
            func_800138FC(D_800016E4, 0x3C, 0x28);
            break;
        case 1:
            func_800138FC(D_80001798, 0x3C, 0x28);
            break;
        case 2:
            func_800138FC(D_800017F4, 0x3C, 0x28);
            break;
    }
    func_80033144(0xFF, 0xFF, 0xFF, 0xFF);
    func_80032A20(func_80032870(0x37), 0xA0, 0xC8);
    func_800139F8(D_8000187C, 0x114, 0xC8);
    if ((D_8005B318 >= 0 && --D_8005B318 == 0) || func_8001C340() != 0) {
        func_8001BED8(0);
        func_8001DEE8();
    }
}
