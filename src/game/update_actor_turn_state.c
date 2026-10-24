typedef unsigned char u8;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Actor_8000A488 {
    u8 pad_00[0xE0];
    s32 unkE0;
    u8 pad_E4[0x1C];
    u32 state;      /* 0x100 */
    u8 pad_104[0x14];
    u32 flags;      /* 0x118 */
    s32 unk11C;
    u8 pad_120[4];
    s32 unk124;
    u8 pad_128[0x20];
    f32 unk148;
    u8 pad_14C[0xC];
    u32 timer;      /* 0x158 */
};

extern struct Actor_8000A488 *D_8006FB80;
extern s32 D_8006FB88;
extern u32 D_80099B30;
extern s32 D_800B92E8;
extern struct Actor_8000A488 *func_800206D0(void);
extern s32 func_80005598(struct Actor_8000A488 *a);
extern u8 *func_800197C8(s32 arg);
extern void func_8000C530(void *ctrl, s32 idx, s32 mode);
extern void func_8000550C(s32 state, struct Actor_8000A488 *a);
extern void func_80005500(s32 state, struct Actor_8000A488 *a);
extern s32 func_8001F3A8(void);
extern void func_8002A5B0(s32 a);
extern void func_8002A5EC(s32 a);
extern void func_8000A63C(struct Actor_8000A488 *a);
extern void func_80010E24(s32 a, s32 b);

void func_8000A488(struct Actor_8000A488 *actor) {
    struct Actor_8000A488 *other;
    struct Actor_8000A488 *p;

    other = func_800206D0();
    switch (actor->state) {
        case 7:
            break;
        case 5:
            if (func_80005598(actor)) {
                func_8000C530(func_800197C8(actor->unkE0) + 0x2C, 0x21, 1);
                do {
                    actor->flags |= 0x400000;
                } while (0);
                p = D_8006FB80;
                p->unk124 = ((s32)p->unk148 + 180) % 360;
                other->unk124 = other->unk148;
            } else {
                func_8000C530(func_800197C8(actor->unkE0) + 0x2C, 0, 1);
            }
            func_8000550C(6, actor);
            break;
        case 6:
            if (func_80005598(actor) && func_8001F3A8() == 0) {
                D_8006FB88 = -1;
                actor->timer = D_80099B30 + 10;
                func_80005500(0, actor);
                func_80005500(0, other);
                func_8002A5B0(0x2D);
                func_8002A5EC(0);
                D_800B92E8 = -1;
                func_8000A63C(actor);
                if (actor->unk11C >= 0) {
                    func_80010E24(actor->unk11C, 0x3C);
                }
            }
            break;
    }
}
