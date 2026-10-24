typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef float f32;

extern void *D_8005B214;
extern s32 D_800538F4;
extern u32 D_800538F0;
extern char D_8005B218[];
extern char D_80001954[];
extern s32 func_800166B0(void *);
extern int func_8003A57C(char *, const char *, ...);
extern void func_800138FC(char *, s32, s32);
extern s32 func_8001F138(s32);
extern void func_8001F064(void);
extern void func_8001F020(s32);
extern s32 func_8001B7B0(void);
extern void func_8001A5C0(void);

void func_8001CDFC(void) {
    s32 buttons;

    buttons = func_800166B0(D_8005B214);
    if (D_800538F4 == 0) {
        if (buttons & 2) {
            D_800538F4 = 5;
            D_800538F0++;
        }
        if (buttons & 1) {
            D_800538F4 = 5;
            D_800538F0--;
        }
    } else {
        D_800538F4--;
    }
    if (D_800538F0 >= 0x5B) {
        D_800538F0 = 0;
    }
    func_8003A57C(D_8005B218, D_80001954, D_800538F0);
    func_800138FC(D_8005B218, 0x104, 0x64);
    if (buttons & 0x10) {
        func_8001F138(D_800538F0);
    }
    if (buttons & 0x40) {
        func_8001F064();
        func_8001F020(0);
    }
    if (func_8001B7B0()) {
        func_8001A5C0();
    }
}

