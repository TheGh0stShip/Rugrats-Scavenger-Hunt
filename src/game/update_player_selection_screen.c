typedef short s16;
typedef int s32;
typedef unsigned int u32;
typedef unsigned char u8;

struct Actor_8002A9D4 {
    u8 pad_000[0xF4];
    s32 unkF4;            /* 0x0F4 */
    s32 unkF8;            /* 0x0F8 */
    u8 pad_0FC[0x118 - 0xFC];
    s32 unk118;           /* 0x118 */
    u32 unk11C;           /* 0x11C */
    u8 pad_120[0x144 - 0x120];
    s32 unk144;           /* 0x144 */
    u8 pad_148[8];
    s16 unk150;           /* 0x150 (low byte at 0x151) */
};


struct Global_80052F04 {
    u8 pad_00[0xF0];
    s32 unkF0;            /* 0xF0 */
};

extern s32 D_800B9280[2];
extern s32 D_800B92C0;
extern s32 D_800B92C4;
extern s32 D_800B92C8;
extern s32 D_800B92CC;
extern s32 D_800B92D0;
extern s32 D_80057D5C;
extern s32 D_800581D0;
extern s32 D_80054410[];
extern struct Actor_8002A9D4 *D_8005B214;
extern struct Global_80052F04 *D_80052F04;
extern char D_800022C8[];
extern char D_800022D4[];
extern char D_800022DC[];
extern char D_800022E4[];
extern char D_800022E8[];
extern char D_800022F0[];
extern int func_8001F588(s32 a);
extern void func_800137E8(s32 a);
extern void *func_80032870(s32 id);
extern void func_80032A20(void *spr, s32 x, s32 y);
extern void func_800138FC(char *buf, s32 x, s32 y);
extern s32 func_8003A57C(char *buf, const char *fmt, ...);
extern s32 func_8003AEA0(const char *s);
extern void *func_80038D90(char *dst, const char *src, s32 n);
extern u32 func_8002A604(void);
extern void func_8002A5EC(s32 value);
extern void func_8002A5B0(s32 value);
extern int func_8001F1C4(s32 a);
extern s32 func_800166B0(struct Actor_8002A9D4 *a);
extern u8 *func_800197C8(s32 id);
extern void func_8000CE7C(void *obj, s32 frame);
extern void func_8000C530(void *obj, s32 a, s32 b);
extern struct Actor_8002A9D4 *func_800055A4(s32 i);
extern void func_8000FDAC(s32 a, struct Actor_8002A9D4 *b);
extern void func_8000DDAC(s32 a);
extern void func_8002B044(s32 a, s32 b, s32 c);
extern int func_8001F138(s32 a);
extern void func_80010E24(s32 a, s32 b);
extern s32 func_8003A840(void);
extern void func_80028F80(struct Actor_8002A9D4 *a);

void func_8002A9D4(struct Actor_8002A9D4 *arg0) {
    register s32 changed __asm__("$17");
    s32 buttons;
    s32 i;
    struct Actor_8002A9D4 *o;
    u32 state;
    register s32 one __asm__("$16");
    char buf[0x38];

    changed = 0;
    func_8001F588(0);
    func_800137E8(4);
    func_80032A20(func_80032870(0x41), 100, 0xB0);
    func_80032A20(func_80032870(0x37), 100, 0xC8);
    func_800138FC(D_800022C8, 0x96, 0xB4);
    func_8003A57C(buf, D_800022D4);
    if (D_800B9280[0] == 2) {
        func_80038D90(buf + func_8003AEA0(buf), D_800022DC, 8);
    } else if (D_800B9280[0] == 3) {
        func_80038D90(buf + func_8003AEA0(buf), D_800022E4, 4);
    } else {
        func_80038D90(buf + func_8003AEA0(buf), D_800022E8, 6);
    }
    func_80038D90(buf + func_8003AEA0(buf), D_800022F0, 9);
    func_800138FC(buf, 0x96, 0xC8);

    one = 1;
    state = func_8002A604();
    if (state == 2) goto state_2;
    if (state < 3) {
        if (state == one) goto state_1;
        goto done;
    }
    if (state == 3) goto state_3;
    goto done;
state_1:
            D_800B92C0 = arg0->unkF4 + 1;
            if (D_800B92C0 >= D_80057D5C) {
                D_800B92C0 = 0;
            }
            func_8002A5EC(2);
            changed = 1;
            D_800B92C4 = 0;
            func_8001F1C4(D_80054410[D_800B9280[0] * 4 + arg0->unkF8]);
            /* fallthrough */
state_2:
            if (D_800B92C4 != 0) {
                D_800B92C4--;
            }
            buttons = func_800166B0(D_8005B214);
            if (D_800B92C4 == 0) {
                if (buttons & 4) {
                    do {
                        if (++D_800B92C0 >= D_80057D5C) {
                            D_800B92C0 = 0;
                        }
                    } while (D_800B92C0 == arg0->unkF4);
                    changed = 1;
                    D_800B92C4 = 0xF;
                } else if (buttons & 8) {
                    do {
                        if (--D_800B92C0 < 0) {
                            D_800B92C0 = D_80057D5C - 1;
                        }
                    } while (D_800B92C0 == arg0->unkF4);
                    changed = 1;
                    D_800B92C4 = 0xF;
                }
            }
            if (buttons & 0x10) {
                D_800B9280[1] = D_800B92C0;
                func_8000CE7C(func_800197C8(0x1F) + 0x2C, 0);
                D_800B92C4 = 0xC;
                if (D_800B9280[0] == 2) {
                    D_800B92C8 = 4;
                    D_800B92D0 = 0x40;
                } else if (D_800B9280[0] == 3) {
                    D_800B92C8 = 2;
                    D_800B92D0 = 0x42;
                } else if (D_800B9280[0] == 4) {
                    D_800B92C8 = 1;
                    D_800B92D0 = 0x58;
                }
                D_800B92CC = 0;
                func_8000FDAC(6, func_800055A4(D_800B92C0));
                func_8002A5EC(3);
                goto done;
            }
            if (changed) {
                o = func_800055A4(D_800B92C0);
                func_8000DDAC(*((u8 *)o + 0x151));
                func_8000FDAC(8, o);
                for (i = 0; i < D_80057D5C; i++) {
                    if (i != arg0->unkF4) {
                        func_800055A4(i)->unk118 &= ~0x400000;
                    }
                }
                o->unk118 |= 0x400000;
            }
            goto done;
state_3:
            o = func_800055A4(D_800B9280[1]);
            if (D_800B9280[0] == 4) {
                if (D_800B92CC != 0) {
                    func_8002B044(4, o->unk144, o->unk150);
                }
            } else {
                for (i = 0; i < D_800B92CC; i++) {
                    func_8002B044(D_800B9280[0], o->unk144 + i, o->unk150);
                }
            }
            if (D_800B92C4 != 0) {
                D_800B92C4--;
            } else {
                D_800B92C4 = 0xC;
                if (D_800B92CC < D_800B92C8) {
                    D_800B92CC++;
                    func_8001F138(D_800B92D0);
                    if (o->unk11C < 4) {
                        func_80010E24(o->unk11C, 5);
                    }
                    if (o->unk11C < 4) {
                        func_80010E24(o->unk11C, 5);
                    }
                    if (D_800B92CC < D_800B92C8) {
                        goto done;
                    }
                    D_800B92C4 = 0x19;
                }
            }
            if (D_800B92CC >= D_800B92C8 && D_800B92C4 == 0) {
                D_800581D0 = (func_8003A840() % 18 + arg0->unk144 + 6) % 24;
                func_80028F80(arg0);
                func_8000C530(func_800197C8(0x1F) + 0x2C, 0, 0);
                func_8002A5B0(0);
                func_8000DDAC(*((u8 *)arg0 + 0x151));
                for (i = 0; i < D_80057D5C; i++) {
                    if (i != arg0->unkF4) {
                        func_800055A4(i)->unk118 &= ~0x400000;
                    }
                }
                D_80052F04->unkF0 = 0;
            }
done:;
}

