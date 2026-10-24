typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

struct Actor_80021464 {
    u8 pad_00[0xF8];
    s32 unkF8;
    s32 unkFC;
    u8 pad_100[0x130 - 0x100];
    s32 unk130;
};

extern s32 D_8005435C;
extern s32 D_800529F8;
extern s32 D_80052F18;
extern s32 func_800222A8(struct Actor_80021464 *);
extern void func_80005500(s32, struct Actor_80021464 *);
extern void func_800069C8(s32, struct Actor_80021464 *, s32);
extern s32 func_80006E88(struct Actor_80021464 *);
extern void func_80021698(struct Actor_80021464 *);

void func_80021464(struct Actor_80021464 *a)
{
    u32 r;

    if (D_8005435C != 1 || a->unkFC == 1) {
        return;
    }
    if (D_800529F8 != a->unk130 &&
        (D_800529F8 == 0 || (a->unkF8 == 4 && D_80052F18 == 0))) {
        r = func_800222A8(a);
        switch (r) {
        case 0:
            func_80005500(0xC, a);
            return;
        case 2:
            func_80005500(0, a);
            func_800069C8(0, a, 0);
            return;
        case 1:
            if (D_800529F8 == 0) {
                D_800529F8 = r;
            }
            break;
        case 3:
            func_800069C8(0, a, 0);
            return;
        }
    }
    if (func_80006E88(a) == 0) {
        func_80021698(a);
    }
}
