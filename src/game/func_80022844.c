typedef signed char s8; typedef unsigned char u8; typedef short s16; typedef unsigned short u16; typedef int s32; typedef unsigned int u32;

struct Pad_80058B58 {
    u8 pad_0[0x4];
    u16 pressed;
    u8 pad_6[0x4];
};

extern char D_80001EA0[];
extern char D_80001EA8[];
extern char D_80001EB0[];
extern char D_80001EB8[];
extern char D_80001EC0[];
extern char *D_800549D8[];
extern s32 D_80052F14;
extern s32 D_8006FB90;
extern s32 D_8006FB94;
extern s32 D_80059D40;
extern struct Pad_80058B58 D_80058B58[];

extern void func_8003318C(s32, s32, s32, s32, s32, s32, s32, s32);
extern int func_8001F588(s32);
extern void func_80013724(void);
extern void func_80033118(void);
extern void func_800139F8(char *, s32, s32);
extern void func_8001380C(s32, char *, s32, s32);
extern void func_80012450(void);
extern void func_80027FD4(void);

void func_80022844(void) {
    u16 btn;

    func_8003318C(0, 0, 0x200, 0xF0, 0x73, 0x1E, 0xE6, 0xFF);
    func_8001F588(0);
    func_80013724();
    func_80033118();
    func_800139F8(D_80001EA0, 0x100, 0x32);
    func_800139F8(D_800549D8[D_80052F14], 0x100, 0x50);
    if (D_8006FB90 != 0) {
        D_8006FB90--;
    }
    switch (D_8006FB94) {
        case 0:
            func_8001380C(0x38, D_80001EA8, 0x6A, 0xB4);
            func_8001380C(0x37, D_80001EB0, 0x132, 0xB4);
            if (D_8006FB90 == 0) {
                btn = D_80058B58[D_80052F14].pressed;
                if (btn & 0x4000) {
                    D_80052F14 = -1;
                    D_8006FB94 = 0;
                } else if (btn & 0x8000) {
                    D_8006FB94 = 1;
                    D_8006FB90 = 10;
                }
            }
            break;
        case 1:
            func_8001380C(0x38, D_80001EB8, 0x6A, 0xB4);
            func_8001380C(0x37, D_80001EC0, 0x132, 0xB4);
            if (D_8006FB90 == 0) {
                btn = D_80058B58[D_80052F14].pressed;
                if (btn & 0x4000) {
                    D_8006FB90 = 10;
                    D_8006FB94 = 0;
                } else if (btn & 0x8000) {
                    D_80052F14 = -1;
                    D_80059D40 = 7;
                    D_8006FB94 = 0;
                    func_80012450();
                    func_80027FD4();
                }
            }
            break;
    }
}

