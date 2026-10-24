/* Target: func_8002C168  VRAM 0x8002C168  ROM 0x2CD68
 * SN GCC 2.8 -O2 -G0 -mips3, normalized SN verifier */
typedef float f32;
typedef short s16;
typedef int s32;
typedef unsigned char u8;

struct Actor_8002C168 {
    u8 pad_000[0xE0];
    void *unkE0;          /* 0x0E0 */
    u8 pad_0E4[0x150 - 0xE4];
    s16 unk150;           /* 0x150 */
    u8 pad_152[0x1E8 - 0x152];
    s16 unk1E8[4];        /* 0x1E8 */
    s16 unk1F0[4];        /* 0x1F0 */
};

#define FABS_INT(x) ((0.0f <= (f32)(x)) ? (x) : -(x))

extern s32 D_800B92D4;
extern s32 D_800B92D8;
extern s32 D_800581C0[];
extern char D_8005B218[];
extern char D_8000245C[];
extern char D_8000247C[], D_80002498[], D_800024AC[], D_800024C4[];
extern char D_800024D8[], D_800024F0[], D_80002504[], D_8000251C[];
extern char D_80002530[], D_80002550[], D_80002568[], D_80002584[];
extern char D_8000259C[], D_800025B4[], D_800025C4[], D_800025D8[];
extern char D_800025E8[], D_800025FC[], D_80002604[];
extern s32 *func_8002A628(s32 a);
extern s32 func_8002A604(void);
extern void func_8002A5EC(s32 value);
extern void func_8002A5B0(s32 value);
extern s32 func_8003A57C(char *buf, const char *fmt, ...);
extern void func_800137E8(s32 a);
extern void func_800139F8(char *buf, s32 x, s32 y);
extern void func_8002E6D0(struct Actor_8002C168 *a, s32 b, s32 c);
extern s32 func_80005598(struct Actor_8002C168 *a);
extern void func_800069C8(s32 a, struct Actor_8002C168 *b, s32 c);
extern u8 *func_800197C8(int);
extern s32 func_8000CE08(void *a, s32 b);
extern void func_8000FDAC(s32 a, struct Actor_8002C168 *b);

void func_8002C168(struct Actor_8002C168 *arg0) {
    s32 kind;
    s32 amount;
    s32 anim;
    s32 t;

    kind = *func_8002A628(0);
    amount = *func_8002A628(1);
    if (func_8002A604() == 1) {
        D_800B92D4 = -1;
        func_8002A5EC(3);
        D_800B92D8 = (*(arg0->unk1E8 + kind) == *(arg0->unk1F0 + kind)) ||
                     (*(arg0->unk1F0 + kind) == 0) || (*(arg0->unk1F0 + kind) == -1);
    }
    if (D_800B92D4 != 0) {
    if (arg0->unk150 != 0) {
        s32 *tbl = D_800581C0;
        t = tbl[arg0->unk150 - 1];
        if (t == 1) {
            amount = -amount;
        } else if (t == 2) {
            amount = 0;
        }
    }
    if (amount == 0) {
        func_8003A57C(D_8005B218, D_8000245C);
    } else {
        switch (kind) {
            case 0:
                if (amount > 0) {
                    if (D_800B92D8) func_8003A57C(D_8005B218, D_8000247C);
                    else func_8003A57C(D_8005B218, D_80002498, FABS_INT(amount));
                    anim = 0x10;
                } else {
                    if (D_800B92D8) func_8003A57C(D_8005B218, D_800024AC);
                    else func_8003A57C(D_8005B218, D_800024C4, FABS_INT(amount));
                    anim = 0x15;
                }
                break;
            case 1:
                if (amount > 0) {
                    if (D_800B92D8) func_8003A57C(D_8005B218, D_800024D8);
                    else func_8003A57C(D_8005B218, D_800024F0, FABS_INT(amount));
                    anim = 0x11;
                } else {
                    if (D_800B92D8) func_8003A57C(D_8005B218, D_80002504);
                    else func_8003A57C(D_8005B218, D_8000251C, FABS_INT(amount));
                    anim = 0x16;
                }
                break;
            case 2:
                if (amount > 0) {
                    if (D_800B92D8) func_8003A57C(D_8005B218, D_80002530);
                    else func_8003A57C(D_8005B218, D_80002550, FABS_INT(amount));
                    anim = 0xE;
                } else {
                    if (D_800B92D8) func_8003A57C(D_8005B218, D_80002568);
                    else func_8003A57C(D_8005B218, D_80002584, FABS_INT(amount));
                    anim = 0x13;
                }
                break;
            case 3:
                if (amount > 0) {
                    if (D_800B92D8) func_8003A57C(D_8005B218, D_8000259C);
                    else func_8003A57C(D_8005B218, D_800025B4, FABS_INT(amount));
                    anim = 0xF;
                } else {
                    if (D_800B92D8) func_8003A57C(D_8005B218, D_800025C4);
                    else func_8003A57C(D_8005B218, D_800025D8, FABS_INT(amount));
                    anim = 0x14;
                }
                break;
            case 8:
                func_8003A57C(D_8005B218, D_800025E8,
                              (amount > 0) ? D_800025FC : D_80002604, FABS_INT(amount));
                anim = 0x17;
                if (amount > 0) {
                    anim = 0x12;
                }
                break;
        }
    }
    func_800137E8(4);
    func_800139F8(D_8005B218, 0x100, 0xC8);
    if (D_800B92D4 == -1) {
        func_8002E6D0(arg0, kind, amount);
        if (amount != 0) {
            if (func_80005598(arg0) != 0) {
            func_800069C8(anim, arg0, 0);
            func_800069C8(0, arg0, 2);
            D_800B92D4 = func_8000CE08(*(void **)(func_800197C8((s32)arg0->unkE0) + 0x2C), anim);
            func_8000FDAC(10, arg0);
            } else {
                D_800B92D4 = 0x14;
            }
        } else {
            D_800B92D4 = 0x14;
        }
    }
    D_800B92D4--;
    } else {
        D_800B92D4 = -1;
        func_8002A5B0(3);
    }
}
