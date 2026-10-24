typedef unsigned char u8;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;

struct Actor_80009FD8 {
    u8 pad_00[0xE0];
    s32 unkE0;
    u8 pad_E4[0x10];
    s32 unkF4;
    s32 unkF8;
    u8 pad_FC[4];
    u32 state;      /* 0x100 */
    u8 pad_104[0x14];
    u32 flags;      /* 0x118 */
    u8 pad_11C[8];
    s32 unk124;
    s32 unk128;
    u8 pad_12C[0x2C];
    u32 timer;      /* 0x158 */
};

struct AnimCtrl_80009FD8 {
    u8 pad_00[0xE];
    s16 frame;
};

extern s32 D_80052F18;
struct State_80057F70 {
    u8 pad_000[0x218];
    struct Actor_80009FD8 *actor;   /* 0x218 */
    u8 pad_21C[4];
    s32 mode;                       /* 0x220 */
    s32 timer;                      /* 0x224 */
};

extern struct State_80057F70 D_80057F70;
extern u32 D_80099B30;
extern struct Actor_80009FD8 *func_80009140(void);
extern s32 func_80005598(struct Actor_80009FD8 *a);
extern u8 *func_800197C8(s32 arg);
extern void func_8000C530(void *ctrl, s32 idx, s32 mode);
extern void func_8000CCE4(void *ctrl, s32 arg);
extern void func_80010190(struct Actor_80009FD8 *a);
extern void func_8000550C(s32 state, struct Actor_80009FD8 *a);
extern void func_8001BED8(s32 a);
extern void func_8001BF24(s32 a);
extern s32 func_8001F3A8(void);
extern struct Actor_80009FD8 *func_800055A4(s32 idx);
extern struct Actor_80009FD8 *func_80005544(void);

void func_80009FD8(struct Actor_80009FD8 *actor) {
    struct Actor_80009FD8 *other;
    struct AnimCtrl_80009FD8 *ctrl;
    s32 animA;
    s32 animB;
    u32 flagA;
    u32 flagB;

    other = func_80009140();
    actor->flags |= 0x800000;
    if (D_80052F18 == 1) {
        switch (actor->state) {
            case 5:
                if (!func_80005598(actor)) {
                    func_8000C530(func_800197C8(other->unkE0) + 0x2C, 0x2B, 3);
                }
                other->unk124 = other->unk128 = (D_80057F70.actor->unk124 + 180) % 360;
                func_80010190(D_80057F70.actor);
                func_8000550C(8, actor);
                func_8001BED8(0x1E);
                func_8001BF24(0x15);
                break;
            case 9:
                switch (D_80057F70.mode) {
                    case 2:
                        animA = 0x2D;
                        flagA = 0x1000;
                        animB = 0x28;
                        flagB = 0x2000;
                        break;
                    case 0:
                        animA = 0x2D;
                        flagA = 0x1000;
                        animB = 0x2E;
                        flagB = 0x2000;
                        break;
                    case 1:
                        animA = 0x2F;
                        flagA = 0x2000;
                        animB = 0x2B;
                        flagB = 0x1000;
                        break;
                }
                if (func_80005598(actor)) {
                    func_8000C530(func_800197C8(actor->unkE0) + 0x2C, animA, 3);
                    actor->flags |= flagA;
                } else {
                    func_8000C530(func_800197C8(actor->unkE0) + 0x2C, animB, 3);
                    actor->flags |= flagB;
                }
                func_8000550C(6, actor);
                break;
            case 6:
                if (actor->flags & 0x1000) {
                    func_8000CCE4(func_800197C8(actor->unkE0) + 0x2C, 4);
                    func_80010190(actor);
                    ctrl = (struct AnimCtrl_80009FD8 *)(func_800197C8(actor->unkE0) + 0x2C);
                    if (func_8001F3A8() == 0 && ctrl->frame > 0) {
                        actor->flags &= ~0x1000;
                        if (actor->flags & 0x2000) {
                            actor->flags &= ~0x2000;
                            func_8000550C(0xA, actor);
                        } else if (func_80005598(actor)) {
                            other->flags |= 0x1000;
                        } else {
                            func_800055A4(D_80057F70.actor->unkF4)->flags |= 0x1000;
                        }
                    }
                }
                break;
            case 7:
                break;
            case 10:
                D_80057F70.timer = 0x32;
                func_8001BF24(0x16);
                func_8000550C(6, actor);
                break;
        }
    } else {
        switch (actor->state) {
            case 5:
                if (func_80005598(actor)) {
                    func_8000C530(func_800197C8(actor->unkE0) + 0x2C, 0x23, 0);
                } else {
                    func_8000C530(func_800197C8(actor->unkE0) + 0x2C, 0x23, 3);
                }
                other->unk124 = other->unk128 = (D_80057F70.actor->unk124 + 180) % 360;
                func_8000550C(6, actor);
                break;
            case 6:
                if (func_80005598(actor)) {
                    func_80010190(actor);
                    if (func_8001F3A8() == 0) {
                        func_8001BED8(0x1E);
                        func_8001BF24(0x15);
                        func_8000C530(func_800197C8(actor->unkE0) + 0x2C, 0, 2);
                        func_8000550C(7, actor);
                    }
                }
                break;
            case 7:
                if (!func_80005598(actor)) {
                    func_80010190(actor);
                    if (!(D_80099B30 < actor->timer)) {
                        func_8000550C(6, actor);
                        if (func_80005544()->unkF8 == 4) {
                            D_80057F70.timer = 0x32;
                            func_8001BF24(0x16);
                        } else {
                            func_8001BF24(0x17);
                        }
                    }
                }
                break;
        }
    }
}
