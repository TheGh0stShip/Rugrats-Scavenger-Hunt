/* func_8002D51C  VRAM 0x8002D51C  ROM 0x2E11C  Size: 0x4FC
 * Compiler: SN Systems GCC 2.8 (-O2 -G0), asn64 with reviewed scheduling normalization
 */

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;

struct Obj_8005B214 {
    u8 pad_000[0x100];
    s32 f100;           /* 0x100 */
    u8 pad_104[0x194 - 0x104];
    s16 timer;          /* 0x194 */
};

struct Obj_80052F04 {
    u8 pad_00[0xF0];
    s32 fF0;            /* 0xF0 */
};

struct Obj_80053634 {
    u8 pad_00[0x28];
    s32 state;          /* 0x28 */
};

struct Obj_8005B7F8 {
    u8 pad_0[6];
    u8 b6;              /* 0x6 */
    u8 b7;              /* 0x7 */
};

extern s32 func_8002A5C8(void);
extern struct Obj_8005B214 *func_800055A4(s32);
extern void func_800121D0(void);
extern void func_8002D4C0(void);
extern int func_8001F588(s32);
extern void func_8002B530(s32);
extern void func_80016F50(void);
extern void func_80012D7C(s32);
extern void func_8002A5EC(s32);
extern void func_8002E8F0(void);
extern void func_80034808(void);
extern void func_8002D404(void);
extern void func_800118F0(void);
extern void func_80031CB0(void);
extern void func_8001DEF8(void);
extern void func_80035D0C(void);
extern void func_80035E74(void);
extern s32 func_8003A840(void);
extern s32 *func_8002A628(s32);
extern void func_8002FEA8(s32);
extern s32 func_8001BF00(void);
extern void func_8002DA20(struct Obj_8005B214 *);
extern void func_8001A65C(void);
extern int func_800191EC(s32);
extern s32 func_80012CE8(void);
extern s32 func_8001C2C8(void);
extern void func_8001A5C0(void);
extern void func_8002A5B0(s32);
extern void func_8002B72C(void);
extern void func_8002C168(struct Obj_8005B214 *);
extern void func_80029580(struct Obj_8005B214 *);
extern void func_8002D0A8(void);
extern void func_8002C8CC(void);
extern void func_8002CA8C(void);
extern void func_8002CC34(struct Obj_8005B214 *);
extern void func_8002A9D4(struct Obj_8005B214 *);
extern void func_8002B178(void);
extern void func_8002A640(void);
extern void func_8002A1B4(struct Obj_8005B214 *);
extern void func_8002BFF0(struct Obj_8005B214 *);
extern void func_800137E8(s32);
extern void func_800138FC(const char *, s32, s32);
extern void func_8000B2C8(void);

extern u32 D_80052EE0;
extern struct Obj_80052F04 *D_80052F04;
extern s32 D_80057D40;
extern struct Obj_8005B214 *D_8005B214;
extern struct Obj_80053634 *D_80053634;
extern s32 D_80053638;
extern struct Obj_8005B7F8 D_8005B7F8;
extern const char D_80002680[];

void func_8002D51C(void) {
    s32 st;
    s32 r;

    D_80052EE0 |= 1;
    switch (func_8002A5C8()) {
        case 4:
        case 5:
        case 10:
        case 26:
        case 34:
        case 40:
        case 41:
        case 48:
            if (D_80052F04->fF0 != 2) {
                break;
            }
        default:
            D_80052EE0 ^= 1;
            break;
    }
    D_8005B214 = func_800055A4(D_80057D40);
    func_800121D0();
    func_8002D4C0();
    func_8001F588(0);

    st = D_80053634->state;
    if (st != D_80053638) {
        D_80053638 = st;
        switch (st) {
            case 41:
                func_8002B530(9);
                break;
            case 40:
                func_80016F50();
                break;
            case 12:
            case 13:
                func_80012D7C(10);
                func_8002A5EC(0x1B);
                break;
            case 9:
                func_80012D7C(10);
                func_8002A5EC(0x1F);
                func_8002E8F0();
                break;
            case 31:
                func_80012D7C(10);
                func_8002A5EC(0x23);
                func_80034808();
                break;
            case 5:
                D_8005B7F8.b6 = 0;
                D_8005B7F8.b7 = 0;
                break;
            case 4:
                func_8002D404();
                break;
            case 21:
                func_800118F0();
                break;
            case 24:
                func_8002A5EC(0x13);
                func_80031CB0();
                func_8001DEF8();
                break;
            case 10:
                func_8002A5EC(0);
                func_80035D0C();
                break;
            case 26:
                func_8002A5EC(0);
                func_80035E74();
                break;
            case 14:
            case 35:
                func_8002A5EC(0x27);
                break;
            case 37:
                D_8005B214->timer = 50;
                func_8002A5EC(0x2E);
                break;
            case 36:
                func_8002A5EC(0x2A);
                r = func_8003A840() % 4;
                switch (r) {
                    case 0:
                        *func_8002A628(0) = 0;
                        func_8002FEA8(1);
                        break;
                    case 1:
                        *func_8002A628(0) = r;
                        func_8002FEA8(5);
                        break;
                    case 2:
                        *func_8002A628(0) = r;
                        func_8002FEA8(9);
                        break;
                    case 3:
                        *func_8002A628(0) = r;
                        func_8002FEA8(12);
                        break;
                }
                break;
            case 38:
                func_8002A5EC(0x2A);
                *func_8002A628(0) = 4;
                func_8002FEA8(15);
                break;
            case 39:
                func_8002A5EC(0x2A);
                *func_8002A628(0) = 5;
                func_8002FEA8(19);
                break;
        }
    }

    if (func_8001BF00() != 0) {
        func_8002DA20(D_8005B214);
        if (D_8005B214->f100 == 1) {
            D_8005B214->f100 = 2;
        }
    }

    switch (D_80053634->state) {
        case 48:
            func_8001A65C();
            break;
        case 40:
            func_800191EC(0);
            break;
        case 3:
            if (func_80012CE8() != 0 && func_8001C2C8() != 0) {
                func_8001A5C0();
                func_8002A5B0(0);
            }
            break;
        case 4:
        case 41:
            func_8002B72C();
            break;
        case 52:
            func_8002C168(D_8005B214);
            break;
        case 53:
            func_80029580(D_8005B214);
            break;
        case 5:
            func_8002D0A8();
            break;
        case 9:
            func_8002C8CC();
            break;
        case 36:
        case 38:
        case 39:
            func_8002CA8C();
            break;
        case 37:
            func_8002CC34(D_8005B214);
            break;
        case 43:
            func_8002A9D4(D_8005B214);
            break;
        case 44:
            func_8002B178();
            break;
        case 45:
            func_8002A640();
            break;
        case 46:
            func_8002A1B4(D_8005B214);
            break;
        case 47:
            func_8002BFF0(D_8005B214);
            break;
        case 15:
            func_800137E8(8);
            func_800138FC(D_80002680, 0x100, 0x3C);
            break;
        case 49:
        case 50:
            func_8000B2C8();
            break;
    }
}
