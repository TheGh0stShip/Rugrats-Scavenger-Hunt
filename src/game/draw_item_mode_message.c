/* func_8002FF48: SN GCC 2.8, scheduling normalized */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

extern char D_8005B218[];
extern char D_800029CC[];
extern char D_800029D8[];
extern char D_800029E8[];
extern char D_800029F8[];
extern char D_80002A04[];
extern char D_80002A20[];
extern char D_80002A3C[];
extern char D_80002A4C[];
extern char D_80002A68[];
extern char D_80002A78[];
extern int func_8001F588(s32);
extern void func_800137E8(s32);
extern int func_8003A57C(char *, const char *, ...);
extern void func_800138FC(char *, s32, s32);
extern void func_80033144(s32, s32, s32, s32);
extern void func_8001380C(s32, char *, s32, s32);

void func_8002FF48(u32 kind) {
    func_8001F588(0);
    switch (kind) {
        case 0:
            func_800137E8(8);
            func_8003A57C(D_8005B218, D_800029CC);
            func_800138FC(D_8005B218, 0x100, 0x20);
            func_8001F588(0);
            func_80033144(0xFF, 0xFF, 0xFF, 0xFF);
            func_8003A57C(D_8005B218, D_800029D8);
            func_8001380C(0x41, D_8005B218, 0x8C, 0xB9);
            func_8003A57C(D_8005B218, D_800029E8);
            func_8001380C(0x42, D_8005B218, 0x8C, 0xCA);
            break;
        case 1:
            func_800137E8(8);
            func_8003A57C(D_8005B218, D_800029F8);
            func_800138FC(D_8005B218, 0x100, 0x20);
            func_8001F588(0);
            func_80033144(0xFF, 0xFF, 0xFF, 0xFF);
            func_8003A57C(D_8005B218, D_800029D8);
            func_8001380C(0x41, D_8005B218, 0x8C, 0xB9);
            func_8003A57C(D_8005B218, D_800029E8);
            func_8001380C(0x42, D_8005B218, 0x8C, 0xCA);
            break;
        case 2:
            func_800137E8(8);
            func_8003A57C(D_8005B218, D_80002A04);
            func_800138FC(D_8005B218, 0x100, 0x20);
            break;
        case 3:
            func_800137E8(8);
            func_8003A57C(D_8005B218, D_80002A20);
            func_800138FC(D_8005B218, 0x100, 0x20);
            break;
        case 4:
            func_800137E8(8);
            func_8003A57C(D_8005B218, D_80002A3C);
            func_800138FC(D_8005B218, 0x100, 0x20);
            break;
        case 5:
            func_800137E8(8);
            func_8003A57C(D_8005B218, D_80002A4C);
            func_800138FC(D_8005B218, 0x100, 0x20);
            func_8001F588(0);
            func_8003A57C(D_8005B218, D_80002A68);
            func_80033144(0xFF, 0xFF, 0xFF, 0xFF);
            func_8001380C(0x42, D_8005B218, 0xAA, 0xB9);
            func_8001380C(0x37, D_80002A78, 0xAA, 0xCB);
            break;
    }
}
