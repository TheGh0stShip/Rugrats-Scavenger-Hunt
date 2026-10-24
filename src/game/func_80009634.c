
typedef unsigned char u8;
typedef signed char s8;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

struct Sub_80057F70 {
    u8 pad_000[0x1A7];
    s8 unk1A7;
};
struct Obj_80057F70 {
    u8 pad_000[0x210];
    s32 unk210;
    u8 pad_214[0x4];
    struct Sub_80057F70 *unk218;
    u8 pad_21C[0x4];
    s32 unk220;
    s32 unk224;
};
struct Actor_80009634 {
    u8 pad_000[0xE0];
    s32 unkE0;
    u8 pad_0E4[0x74];
    s32 unk158;
};
struct Res_80009634 {
    u8 pad_00[0x2C];
    s32 unk2C;
};

extern struct Obj_80057F70 D_80057F70;
extern s32 D_80052F18;
extern s32 D_80058190;
extern char *D_80054994[];
extern char D_80000630[];
extern s32 D_80099B30;

extern struct Actor_80009634 *func_80009140(void);
extern void func_80009930(void);
extern s32 func_8001BF3C(void);
extern void func_8001BF24(s32 arg0);
extern void func_8002621C(struct Sub_80057F70 *arg0, s32 arg1);
extern s32 func_800093AC(void);
extern s32 func_80025C20(s32 arg0);
extern void * func_800055A4(unsigned int);
extern void func_80009434(s32 arg0);
extern int func_80025E04(struct Actor_80009634 *arg0, s32 arg1);
extern s32 func_80009AF4(void);
extern struct Res_80009634 *func_800197C8(s32 arg0);
extern void func_8000C530(s32 *arg0, s32 arg1, s32 arg2);
extern int func_8000CE08(void *, int);
extern void func_8000550C(s32 arg0, struct Actor_80009634 *arg1);
extern s32 func_8001C2C8(void);
extern void func_8001A5C0(void);
extern void func_800137E8(s32 arg0);
extern int func_8003A57C(char *dst, const char *fmt, ...);
extern void func_800138FC(char *str, s32 x, s32 y);

void func_80009634(void) {
    char buf[104];
    struct Actor_80009634 *actor;
    struct Obj_80057F70 *p;
    s32 done = 0;
    s32 sfx;
    s32 v;

    actor = func_80009140();
    if (D_80052F18 == 1) {
        func_80009930();
        return;
    }
    switch (func_8001BF3C()) {
    case 0x15:
        if (!done) {
            struct Obj_80057F70 *r = &D_80057F70;
            if (r->unk218->unk1A7 >= 10) {
                func_8002621C(r->unk218, -10);
                r->unk220 = 0;
                v = func_800093AC();
                if (v != 0) {
                    func_80009434(func_800055A4(func_80025C20(v)));
                    r->unk220 = 4;
                    done = 1;
                } else {
                    done = 1;
                }
            }
        }
        if (!done) {
            p = &D_80057F70;
            if (p->unk210 >= 0) {
                done = 1;
                func_80025E04(func_80009140(), p->unk210);
                p->unk210 = -1;
                p->unk220 = done;
            }
        }
        if (!done) {
            if (func_800093AC() == 0 && func_80009AF4() != 0) {
                p->unk220 = 2;
                done = 1;
            }
        }
        if (!done) {
            struct Obj_80057F70 *q = &D_80057F70;
            if (q->unk218->unk1A7 != 0) {
                func_8002621C(q->unk218, -99);
                q->unk220 = 3;
            } else {
                q->unk220 = 5;
            }
        }
        switch (D_80058190) {
        case 0:
        case 3:
            sfx = 0x1D;
            break;
        case 4:
            sfx = 0xF;
            break;
        case 1:
            sfx = 0x1F;
            break;
        case 2:
            sfx = 0x18;
            break;
        case 5:
            sfx = 0x23;
            break;
        }
        func_8000C530(&func_800197C8(actor->unkE0)->unk2C, sfx & 0xFF, 0);
        v = func_8000CE08(func_800197C8(actor->unkE0)->unk2C, sfx & 0xFF);
        actor->unk158 = D_80099B30 + v;
        func_8000550C(7, actor);
        func_8001BF24(0x1A);
        break;
    case 0x16:
    case 0x17:
        switch (func_8001BF3C()) {
        case 0x16: {
            struct Obj_80057F70 *q = &D_80057F70;
            if (--q->unk224 == 0) {
                func_8001BF24(0x19);
            }
            break;
        }
        case 0x17:
            if (func_8001C2C8() != 0) {
                func_8001BF24(0x19);
            }
            break;
        }
        func_800137E8(8);
        func_8003A57C(buf, D_80000630, D_80054994[D_80058190]);
        func_800138FC(buf, 0x100, 0x78);
        break;
    case 0x19:
        func_8001A5C0();
        break;
    case 0x1A:
        break;
    }
}
