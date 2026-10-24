typedef unsigned char u8;
typedef signed int s32;

extern u8 D_800777F0[];
extern u8 D_800774D0[];
extern u8 D_80071FB0[];
extern u8 D_80074310[];
extern void func_8004F870(s32, void *, void *, s32);
extern void func_80047EE0(void *, s32, void (*)(s32), s32, void *, s32);
extern void func_80049160(void *);
extern void func_80049090(s32, s32);
extern void func_80027AB4(s32);

void func_80027A1C(s32 arg0) {
    func_8004F870(0x96, D_800777F0, D_800774D0, 200);
    func_80047EE0(D_80071FB0, 6, func_80027AB4, arg0, D_80074310, 10);
    func_80049160(D_80071FB0);
    func_80049090(0, 0);
    for (;;) {
    }
}
