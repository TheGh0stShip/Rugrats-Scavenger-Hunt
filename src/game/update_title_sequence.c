/* func_80033970: SN GCC 2.8, scheduling normalized */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;

typedef struct Pad_80058B58 {
    u8 pad_0[0x4];
    u16 pressed;   /* 0x4 */
    u8 pad_6[0x4];
} Pad_80058B58;    /* size 0xA */

typedef struct Task_80033970 {
    u8 pad_00[0xC];
    u32 flags;     /* 0x0C */
    u8 pad_10[0x18];
    s32 timer;     /* 0x28 */
    u32 state;     /* 0x2C */
    s32 delay;     /* 0x30 */
    s32 sel;       /* 0x34 */
} Task_80033970;

typedef struct Obj_800055A4 {
    u8 pad_000[0x11C];
    s32 unk11C;    /* 0x11C */
    u8 pad_120[0x31];
    u8 unk151;     /* 0x151 */
} Obj_800055A4;

extern u32 D_8005435C;
extern s32 D_80052F18;
extern s32 D_80052EE8;
extern s16 D_80052BB0;
extern s32 D_80057D5C;
extern s32 D_80057D58;
extern s32 D_80057D40;
extern s32 D_80057D94;
extern Obj_800055A4 *D_800529F0;
extern s32 D_800529F4;
extern s8 D_800581E9;
extern Pad_80058B58 D_80058B58[4];
extern s32 D_800C3DA0;
extern u32 D_80000318;
extern u8 D_801B4000[];
extern u8 D_001B4000[];

extern void func_80020D34(void);
extern void func_8001ECE4(s32 a, s32 b);
extern int func_8001F138(s32 id);
extern void func_80033D00(void);
extern void func_80015D70(void);
extern void func_800206F8(s32 a, s32 b, s32 c);
extern void func_800125E0(void);
extern void func_8000E6F8(s32);
extern void func_80031408();
extern Obj_800055A4 *func_800055A4(s32 id);
extern void func_80006404(void *);
extern void func_8001FFEC(void *p, u32 size);
extern void func_80016830(s32 n);

void func_80033970(Task_80033970 *t) {
    s32 i;
    Pad_80058B58 *pad;
    Obj_800055A4 *o;

    t->timer++;
    if (t->state < 2) {
        if (D_8005435C < 6) {
            D_8005435C = 6;
        }
        func_80020D34();
        D_80052EE8 = D_80052F18;
        if (D_80052F18 == 0) {
            D_80052BB0 = 15000;
            func_8001ECE4(0x201, 0x38);
        } else if (D_80052F18 == 1) {
            func_8001ECE4(0x204, 0x38);
            func_8001F138(0x29);
            D_80052BB0 = 8000;
        } else {
            D_80052BB0 = 15000;
            func_8001ECE4(0x205, 0x38);
        }
        D_80057D5C = 0;
        if (D_80052F18 == 2) {
            func_80033D00();
        }
        func_80015D70();
        D_80057D58 = 0;
        if (D_80052F18 != 2) {
            func_800206F8(D_80057D5C, 5, 0x7E);
            func_800206F8(D_80057D5C + D_80057D58, 6, 0x80);
            func_800206F8(D_80057D5C + D_80057D58, 4, 0x7C);
            func_800125E0();
        } else {
            func_800206F8(D_80057D5C, 8, 0x86);
        }
        func_8000E6F8(0);
        if (D_80052F18 != 2) {
            func_80031408();
        }
        D_80057D40 = 0;
        o = func_800055A4(0);
        D_800529F0 = o;
        D_800529F4 = o->unk11C;
        D_800581E9 = o->unk151;
        D_80057D94 = 0;
        func_80006404(o);
        D_8005435C = 1;
        if (t->timer >= t->delay) {
            if (t->state == 0) {
                t->state = 2;
            } else {
                t->state = 3;
            }
            t->timer = 0;
        }
    }

    for (i = 0; i < 4; i++) {
        switch (t->state) {
        case 2:
            if ((D_80058B58[i].pressed & 0x1000) || t->timer >= 0x5B) {
                t->state = 3;
                t->timer = 0;
            }
            break;
        case 3:
            if ((D_80058B58[i].pressed & 0x1000) && t->timer >= 0x65) {
                t->state = 4;
                t->timer = 0;
            }
            break;
        case 4:
            if (t->timer >= 0x15) {
                t->state = 5;
            }
            break;
        case 5:
            break;
        case 6:
            if (D_80058B58[i].pressed & 0x1000) {
                func_8001FFEC(D_801B4000, D_80000318 - (u32)D_001B4000);
                func_80016830(0x32);
                D_8005435C = 0;
                t->state = 7;
                D_800C3DA0 = t->sel;
                t->flags |= 1;
            } else if (D_80058B58[i].pressed & 0x800) {
                if (++t->sel >= 3) {
                    t->sel = 2;
                }
            } else if (D_80058B58[i].pressed & 0x400) {
                if (--t->sel < 2) {
                    t->sel = 2;
                }
            }
            break;
        }
    }
}
