typedef unsigned char u8;
typedef signed char s8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;

extern char D_80001898[];
extern char D_8000189C[];
extern int func_8001F588(s32);
extern void func_800137E8(s32);
extern s32 func_8003A57C(char *, const char *, ...);
extern void func_800138FC(char *, s32, s32);

void func_8001AFE8(s32 a, s32 b) {
    char buf[8];

    func_8001F588(0);
    func_800137E8(8);
    func_8003A57C(buf, D_80001898, a);
    func_800138FC(buf, 0x9C, 100);
    func_800138FC(D_8000189C, 0x164, 75);
    func_8003A57C(buf, D_80001898, b);
    func_800138FC(buf, 0x164, 100);
}

