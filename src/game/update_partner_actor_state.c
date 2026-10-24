typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;
typedef double f64;

struct Actor_80009DF8 {
    u8 pad_00[0xE0];
    s32 unkE0;
    u8 pad_E4[0x1C];
    u32 state;            /* 0x100 */
    u8 pad_104[0x14];
    u32 flags;            /* 0x118 */
    u8 pad_11C[8];
    s32 unk124;
    s32 unk128;
    u8 pad_12C[0x2C];
    u32 timer;            /* 0x158 */
};

extern struct Actor_80009DF8 *D_8006FB8C[];
extern u32 D_80099B30;
extern s32 D_80052A60[4];
extern struct Actor_80009DF8 *func_80020680(void);
extern u8 *func_800197C8(s32 arg);
extern s32 func_8003A840(void);
extern void func_8000C530(void *a, s32 b, s32 c);
extern s32 func_80005568(struct Actor_80009DF8 *a);
extern void func_8000FDAC(s32 a, struct Actor_80009DF8 *b);
extern void func_8000550C(s32 a, struct Actor_80009DF8 *b);
extern void func_80005500(s32 a, struct Actor_80009DF8 *b);
extern s32 func_80005598(struct Actor_80009DF8 *a);
extern void func_8001BED8(s32 a);
extern s32 func_8001BF00(void);

void func_80009DF8(struct Actor_80009DF8 *actor) {
    struct Actor_80009DF8 *other;
    void *anim;
    s32 sel;
    s32 idx;

    other = func_80020680();
    actor->flags |= 0x800000;
    switch (actor->state) {
        case 5:
            anim = func_800197C8(actor->unkE0) + 0x2C;
            idx = func_8003A840() % 4;
            sel = D_80052A60[idx];
            func_8000C530(anim, sel, 0);
            if (func_80005568(actor)) {
                actor->timer = D_80099B30 + sel;
            }
            other->unk124 = other->unk128 = (D_8006FB8C[0]->unk124 + 180) % 360;
            func_8000FDAC(5, D_8006FB8C[0]);
            func_8000550C(6, actor);
            break;
        case 6:
            if (func_80005568(actor) && !(D_80099B30 < actor->timer)) {
                func_8001BED8(0x12);
                func_8000550C(7, actor);
            }
            break;
        case 7:
            if (func_8001BF00() == 0) {
                func_80005500(0xC, actor);
                if (func_80005598(actor)) {
                    func_80005500(0, other);
                } else {
                    func_80005500(0, D_8006FB8C[0]);
                }
            }
            break;
    }
}
