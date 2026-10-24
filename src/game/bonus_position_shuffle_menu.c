typedef unsigned char u8;
typedef signed char s8;
typedef signed int s32;
typedef unsigned int u32;
typedef float f32;

typedef struct { float x, y, z; } Vec3f;

struct Player_800055A4 {
    u8 pad_00[0x11C];
    s32 unk11C;    /* 0x11C */
    u8 pad_120[0x86];
    s8 unk1A6;     /* 0x1A6 */
    s8 unk1A7;     /* 0x1A7 */
};

struct Rec_800197C8 {
    u8 pad_00[0x2C];
    u8 obj2C[1];   /* 0x2C */
};

extern struct Player_800055A4 *D_8005B214;
extern s32 D_8005B7EC;
extern s32 D_8005B210;
extern s32 D_80053824;
extern s32 D_800529F4;
extern Vec3f D_800537E8[5];
extern Vec3f D_8005B348[];
extern void *D_8005B33C;
extern f32 D_80059D48[3];
extern u8 D_80059CF0[];
extern char D_800018A4[];
extern char D_800018AC[];
extern char D_800018B8[];
extern void func_8001A754(void);
extern s32 func_8001BF3C(void);
extern s32 func_8001BF00(void);
extern void func_8001BF24(s32);
extern void func_8001BED8(s32);
extern void *func_80038E90(void *, s32, s32);
extern s32 func_8003A840(void);
extern void func_80033118(void);
extern void func_8001380C(s32, char *, s32, s32);
extern void func_80023274(void);
extern u32 func_800166B0(struct Player_800055A4 *);
extern int func_8001F138(s32);
extern void func_8001AFE8(s32, s32);
extern s32 func_8001C2C8(void);
extern void func_8001A5C0(void);
extern void *func_8001E630(void);
extern void func_8000CF10(void *, float, float, float, float, float, float, float);
extern struct Rec_800197C8 *func_800197C8(s32);
extern void * func_8000BDFC(void *, float, float, float, void *, void *, int);
extern s32 func_8001C36C(void);
extern void func_8001A8BC(s32);

void func_8001B08C(void) {
    s32 used[5];
    s32 i;
    s32 r;
    u32 buttons;
    Vec3f *v;

    func_8001A754();
    switch ((u32)func_8001BF3C()) {
        case 0x27:
            for (i = 0; i < D_8005B214->unk1A7; i++) {
                if (i == (i / 5) * 5) {
                    func_80038E90(used, 0, sizeof(used));
                }
                do {
                    r = func_8003A840() % 5;
                } while (r[used] != 0);
                r[used] = 1;
            /* Empty clobber: keeps the shift count of the copy index an immediate. */
            __asm__ volatile("" ::: "$2");
                D_8005B348[i] = D_800537E8[r];
            }
            D_8005B7EC = 0;
            D_8005B210 = 0;
            func_8001BF24(0x28);
            break;
        case 0x28:
            if (D_8005B7EC < D_8005B214->unk1A7) {
                D_8005B7EC++;
            }
            if (D_8005B210 < D_8005B214->unk1A6) {
                D_8005B210++;
            }
            if (D_8005B210 == D_8005B214->unk1A6 && D_8005B7EC == D_8005B214->unk1A7) {
                func_8001BF24(0x29);
            }
            func_8001AFE8(D_8005B7EC, D_8005B210);
            break;
        case 0x29:
            if (D_800529F4 >= 0) {
                func_80033118();
                func_8001380C(0x42, D_800018A4, 0xB4, 0xA0);
                func_8001380C(0x37, D_800018AC, 0xB4, 0xB4);
                func_8001380C(0x38, D_800018B8, 0xB4, 0xC8);
            }
            if (D_80053824 != 0) {
                D_80053824--;
                buttons = 0;
            } else {
                struct Player_800055A4 *p = D_8005B214;
                if (p->unk11C == -1) {
                    func_80023274();
                    p = D_8005B214;
                }
                buttons = func_800166B0(p);
                if (buttons != 0) {
                    D_80053824 = 3;
                }
            }
            if ((buttons & 2) && func_8001BF00() == 0xE) {
                if (D_8005B7EC != 0 && D_8005B210 < 30) {
                    D_8005B7EC--;
                    D_8005B210++;
                    func_8001F138(0x25);
                }
            } else if ((buttons & 1) && func_8001BF00() == 0xE) {
                if (D_8005B7EC < 99 && D_8005B210 >= 2) {
                    D_8005B7EC++;
                    D_8005B210--;
                    func_8001F138(0x25);
                }
            } else if (buttons & 0x10) {
                if (D_8005B7EC < D_8005B214->unk1A7) {
                    func_8001F138(5);
                } else {
                    func_8001F138(4);
                }
                D_8005B214->unk1A6 = D_8005B210;
                D_8005B214->unk1A7 = D_8005B7EC;
                func_8001BF24(0);
            }
            func_8001AFE8(D_8005B7EC, D_8005B210);
            break;
        default:
            if (func_8001C2C8() != 0) {
                func_8001A5C0();
            }
            break;
    }

    for (i = 0; i < D_8005B7EC; i++) {
        D_8005B33C = func_8001E630();
        func_8000CF10(D_8005B33C, 0.0f, 0.0f, 0.0f, 1.0f, D_8005B348[i].x,
                      D_8005B348[i].y + (float)((i / 5) * 20), D_8005B348[i].z);
        func_8000BDFC(func_800197C8(0x1AC)->obj2C, D_80059D48[0], D_80059D48[1], D_80059D48[2],
                      D_80059CF0, D_8005B33C, 0);
    }

    if (func_8001C36C() != 0 && func_8001BF00() == 0xE && func_8001BF3C() == 0x29) {
        func_8001BED8(4);
        func_8001A8BC(9);
    }
}
