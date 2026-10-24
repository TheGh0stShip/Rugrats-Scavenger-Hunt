/* Target: func_8001B800  VRAM 0x8001B800  ROM 0x1C400
 * Size: 0x58C  Compiler: SN GCC 2.8 (-O2), reviewed scheduling normalization.
 * The empty asm hides the call argument 8 from post-reload CSE. */
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef float f32;

typedef struct { f32 x, y, z; } Vec3f;

struct Entry_8005368C {
    Vec3f cur;
    Vec3f on;
    Vec3f off;
    Vec3f vel;
    s16 timer;
    u8 pad_32[2];
    s32 id;
    s16 angle;
    u8 pad_3A[2];
};

struct Rec_800197C8 {
    u8 pad_00[0x2C];
    u8 obj2C[1];
};

struct Player_800055A4 {
    u8 pad_00[0x11C];
    s32 unk11C;
};

extern struct Entry_8005368C D_8005368C[5];
extern struct Player_800055A4 *D_8005B214;
extern s32 D_80057D40;
extern s16 D_8005B7F2;
extern s16 D_8005B7F0;
extern s32 D_800537D8;
extern s32 D_800535DC;
extern s32 D_8005382C;
extern char *D_80053678[];
extern struct Player_800055A4 *func_800055A4(s32);
extern s32 func_8001BF3C(void);
extern s32 func_8001BF00(void);
extern void func_8001BF24(s32);
extern void func_8001BED8(s32);
extern u32 func_800166B0(struct Player_800055A4 *);
extern void func_800137E8(s32);
extern void func_800138FC(char *, s32, s32);
extern struct Rec_800197C8 *func_800197C8(s32);
extern void func_8000CE7C(void *, s32);
extern void func_8001A754(void);
extern int func_8001F138(s32);
extern void func_80010E24(s32, s32);
extern void func_8001AA80(s32);
extern s32 func_8001C338(void);
extern s32 func_8001C340(void);
extern void func_80012D7C(s32);
extern void func_8001A8BC(s32);

void func_8001B800(void) {
    u32 buttons = 0;
    s32 i;
    s16 idx;
    struct Entry_8005368C *e;

    D_8005B214 = func_800055A4(D_80057D40);
    if (func_8001BF3C() == 0) {
        buttons = func_800166B0(D_8005B214);
    }

    if (func_8001BF3C() == 6 || func_8001BF3C() == 7 || func_8001BF3C() == 0x1B ||
        func_8001BF3C() == 0x1F || func_8001BF3C() == 8 || func_8001BF3C() == 9) {
        for (i = 0, e = D_8005368C; i < 5; i++, e++) {
            if (--D_8005368C[i].timer >= 0) {
                D_8005368C[i].cur.x += D_8005368C[i].vel.x;
                D_8005368C[i].cur.y += D_8005368C[i].vel.y;
                D_8005368C[i].cur.z += D_8005368C[i].vel.z;
                continue;
            }
            if (func_8001BF3C() == 6 || func_8001BF3C() == 7) {
                goto reset;
            }
            if (func_8001BF3C() == 0x1B) {
                func_8001BED8(0xC);
                continue;
            }
            if (func_8001BF3C() == 0x1F) {
                func_8001BED8(9);
                continue;
            }
            if (func_8001BF3C() == 9 && func_8001BF00() == 0x29) {
                D_8005B7F2 = D_8005B7F0;
                func_8001BF24(0);
                func_8001BED8(4);
                continue;
            }
            if (func_8001BF3C() == 8) {
                switch ((s16)(D_8005B7F2 + 1)) {
                    case 0:
                        func_8001BED8(0x28);
                        break;
                    case 2:
                        func_8001BED8(5);
                        D_800537D8 = 90;
                        break;
                    case 1:
                        func_8001BED8(9);
                        break;
                    case 3:
                        func_8001BED8(0xA);
                        break;
                    case 4:
                        func_8001BED8(0xC);
                        break;
                    case 5:
                        func_8001BED8(0xE);
                        break;
                }
            }
        reset:
            func_8001BF24(0);
        }
    }

    if (func_8001BF3C() == 0 && func_8001BF00() == 4) {
        idx = D_8005368C[D_8005B7F2].angle + 15;
        D_8005368C[D_8005B7F2].angle = idx % 360;
        { s32 x; __asm__("" : "=r"(x) : "0"(8)); func_800137E8(x); }
        func_800138FC(D_80053678[D_8005B7F2], 0x100, 0xBA);
    }
    e = D_8005368C;
    func_8000CE7C(func_800197C8(e[1].id)->obj2C, 0);
    func_8001A754();

    if (func_8001BF3C() != 0) {
        return;
    }
    if (D_800535DC == 0 || --D_800535DC == 0) {
        if (buttons & 2) {
            D_800535DC = 5;
            func_8001F138(D_8005382C);
            func_80010E24(D_8005B214->unk11C, 5);
            func_8001AA80(6);
            e[D_8005B7F2].angle = 0;
            if (++D_8005B7F2 >= 5) {
                D_8005B7F2 = 0;
            }
            return;
        }
        if (D_800535DC == 0 && (buttons & 1)) {
        D_800535DC = 5;
        func_8001F138(D_8005382C);
        func_80010E24(D_8005B214->unk11C, 5);
        func_8001AA80(7);
        e[D_8005B7F2].angle = 0;
        if (--D_8005B7F2 < 0) {
            D_8005B7F2 = 4;
        }
        return;
    }
    }
    if (func_8001BF00() == 4 && func_8001BF3C() == 0) {
        if (func_8001C338() != 0) {
            D_8005368C[D_8005B7F2].angle = 0;
            D_8005B7F0 = D_8005B7F2;
            D_8005B7F2 = -1;
            func_8001A8BC(8);
        } else if (func_8001C340() != 0) {
            idx = D_8005B7F2;
            if (idx == 3) {
                func_80012D7C(10);
                func_8001BF24(0x1B);
            } else if (idx == 0) {
                func_80012D7C(10);
                func_8001BF24(0x1F);
            } else {
                D_8005368C[idx].angle = 0;
                func_8001A8BC(8);
            }
        }
    }
}
