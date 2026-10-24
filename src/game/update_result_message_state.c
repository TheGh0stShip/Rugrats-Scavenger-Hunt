typedef unsigned char u8;
typedef signed char s8;
typedef int s32;

struct Sub_80057F70 {
    u8 pad_000[0x1A7];
    s8 unk1A7;
};
struct Obj_80057F70 {
    u8 pad_000[0x204];
    s32 unk204;
    u8 pad_208[0x10];
    struct Sub_80057F70 *unk218;
    s32 unk21C;
    s32 unk220;
    s32 unk224;
};
struct Ent_80058188 {
    u8 pad_000[0xF4];
    s32 unkF4;
};

extern struct Obj_80057F70 D_80057F70;
extern struct Ent_80058188 *D_80058188;
extern char *D_800549AC[];
extern char D_80000630[];

extern void *func_80009140(void);
extern s32 func_8001BF3C(void);
extern void func_8001BF24(s32 arg0);
extern void func_8001BED8(s32 arg0);
extern s32 func_80009BE0(struct Sub_80057F70 *arg0);
extern void func_8002621C(struct Sub_80057F70 *arg0, s32 arg1);
extern void func_8000550C(s32 arg0, void *arg1);
extern void *func_800055A4(s32 arg0);
extern void func_800137E8(s32 arg0);
extern int func_8003A57C(char *dst, const char *fmt, ...);
extern void func_800138FC(char *str, s32 x, s32 y);
extern void func_8001A5C0(void);

void func_80009930(void) {
    char buf[104];
    void *actor;
    struct Obj_80057F70 *p;
    s32 done;
    s32 idx;

    actor = func_80009140();
    done = 0;
    switch (func_8001BF3C()) {
    case 0x15:
        if (!done) {
            p = &D_80057F70;
            idx = func_80009BE0(p->unk218);
            if (idx == -1) {
                p->unk220 = 2;
                done = 1;
            }
        }
        if (!done) {
            if (p->unk218->unk1A7 >= 10) {
                func_8002621C(p->unk218, -10);
                p->unk220 = 0;
                p->unk204 = idx;
            } else {
                p->unk220 = 1;
            }
        }
        func_8000550C(9, actor);
        func_8000550C(9, func_800055A4(D_80058188->unkF4));
        func_8001BF24(0x1A);
        break;
    case 0x16: {
        struct Obj_80057F70 *q = &D_80057F70;
        if (--q->unk224 == 0) {
            func_8001BF24(0x18);
        }
        func_800137E8(8);
        func_8003A57C(buf, D_80000630, D_800549AC[q->unk220]);
        func_800138FC(buf, 0x100, 0x78);
        break;
    }
    case 0x18: {
        struct Obj_80057F70 *q = &D_80057F70;
        if (q->unk220 == 0) {
            q->unk224 = 30;
            q->unk21C = 1;
            func_8001BED8(0x18);
        } else {
            func_8001BF24(0x19);
        }
        break;
    }
    case 0x19:
        func_8001A5C0();
        break;
    case 0x1A:
        break;
    }
}
