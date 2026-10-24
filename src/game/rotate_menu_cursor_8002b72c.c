/* func_8002B72C: SN GCC 2.8 -O2 -G0, reviewed scheduling normalization.
 * The empty asm hides the call argument 8 from post-reload CSE so the
 * modulo shift keeps its immediate count. */
typedef float f32;
typedef short s16;
typedef int s32;
typedef unsigned char u8;

typedef struct { f32 x, y, z; } Vec3f_8002B72C;

struct Ent_800544E0 {
    Vec3f_8002B72C pos;   /* 0x00 */
    Vec3f_8002B72C posA;  /* 0x0C */
    Vec3f_8002B72C posB;  /* 0x18 */
    Vec3f_8002B72C vel;   /* 0x24 */
    s16 state;            /* 0x30 */
    u8 pad_32[2];
    s32 handle;           /* 0x34 */
    s16 counter;          /* 0x38 */
    u8 pad_3A[2];
};

extern struct Ent_800544E0 D_800544E0[2];
extern char *D_800544D8[2];
extern s16 D_800B92EC;
extern s16 D_800B92EE;
extern s32 D_80057D40;
extern void *D_8005B214;
extern s32 D_800537D8;
extern s32 D_800535DC;
extern s32 D_8005382C;
extern void *D_800529F4;
extern void *func_800055A4(s32 i);
extern s32 func_8002A604(void);
extern s32 func_8002A5C8(void);
extern void func_8002A5EC(s32 value);
extern void func_8002A5B0(s32 value);
extern s32 func_800166B0(void *a);
extern void func_8001BF24(s32 a);
extern void func_800137E8(s32 a);
extern void func_800138FC(char *buf, s32 x, s32 y);
extern u8 *func_800197C8(int);
extern void func_8000CE7C(void *obj, s32 frame);
extern void func_8002B2E4(void);
extern int func_8001F138(s32 a);
extern void func_80010E24(int, s32 b);
extern void func_8002B434(s32 a);
extern void func_8002B530(s32 a);
extern s32 func_8001C338(void);
extern s32 func_8001C340(void);
extern void func_80012D7C(s32 a);

void func_8002B72C(void) {
    s32 buttons;
    s32 i;
    s16 old;
    struct Ent_800544E0 *e;

    buttons = 0;
    D_8005B214 = func_800055A4(D_80057D40);
    if (func_8002A604() == 0) {
        buttons = func_800166B0(D_8005B214);
    }
    if (func_8002A604() == 6 || func_8002A604() == 7 || func_8002A604() == 0x1B ||
        func_8002A604() == 0x1F || func_8002A604() == 8 || func_8002A604() == 9) {
        for (i = 0; i < 2; i++) {
            e = &D_800544E0[i];
            if (--e->state >= 0) {
                e->pos.x += e->vel.x;
                e->pos.y += e->vel.y;
                e->pos.z += e->vel.z;
            } else if (func_8002A604() == 6 || func_8002A604() == 7) {
                func_8001BF24(0);
            } else if (func_8002A604() == 0x1F) {
                func_8002A5B0(9);
            } else if (func_8002A604() == 9 && func_8002A5C8() == 0x29) {
                D_800B92EC = D_800B92EE;
                func_8002A5EC(0);
                func_8002A5B0(4);
            } else {
                if (func_8002A604() == 8) {
                    switch (D_800B92EC) {
                        case 0:
                            func_8002A5B0(9);
                            break;
                        case 1:
                            func_8002A5B0(5);
                            D_800537D8 = 0x5A;
                            break;
                        case -1:
                            func_8002A5B0(0x28);
                            break;
                    }
                }
                func_8002A5EC(0);
            }
        }
    }
    if (func_8002A604() == 0 && func_8002A5C8() == 4) {
        D_800544E0[D_800B92EC].counter = (s16)(D_800544E0[D_800B92EC].counter + 15) % 360;
{ s32 x = 8; __asm__("" : "+r"(x)); func_800137E8(x); }
        func_800138FC(D_800544D8[D_800B92EC], 0x100, 0xBA);
    }
    func_8000CE7C(func_800197C8(D_800544E0[1].handle) + 0x2C, 0);
    func_8002B2E4();
    if (func_8002A604() != 0) {
        return;
    }
    if (D_800535DC == 0 || --D_800535DC == 0) {
        if (buttons & 2) {
            D_800535DC = 5;
            func_8001F138(D_8005382C);
            func_80010E24(D_800529F4, 5);
            func_8002B434(6);
            D_800544E0[D_800B92EC].counter = 0;
            if (++D_800B92EC >= 2) {
                D_800B92EC = 0;
            }
            return;
        }
        if (D_800535DC == 0 && (buttons & 1)) {
            D_800535DC = 5;
            func_8001F138(D_8005382C);
            func_80010E24(D_800529F4, 5);
            func_8002B434(7);
            D_800544E0[D_800B92EC].counter = 0;
            if (--D_800B92EC < 0) {
                D_800B92EC = 1;
            }
            return;
        }
    }
    if (func_8002A5C8() == 4 && func_8002A604() == 0) {
        if (func_8001C338() != 0) {
            old = D_800B92EC;
            D_800544E0[D_800B92EC].counter = 0;
            D_800B92EC = -1;
            D_800B92EE = old;
            func_8002B530(8);
        } else if (func_8001C340() != 0) {
            if (D_800B92EC == 0) {
                func_80012D7C(10);
                func_8002A5EC(0x1F);
            } else {
                D_800544E0[D_800B92EC].counter = 0;
                func_8002B530(8);
            }
        }
    }
}
