/* func_80029DC4: 2x2 zone select box draw and cursor input.
 * SN GCC 2.8 -O2; uses a reviewed register-rename normalizer for the label address. */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;

typedef struct {
    u32 w0;
    u32 w1;
} Gfx_80029DC4;

struct Actor_80029DC4 {
    u8 pad_000[0x11C];
    s32 unk11C;
};

extern s16 D_8005438C;
extern Gfx_80029DC4 *D_80077830;
extern char *D_80052FD0[];
extern void func_8002E478(struct Actor_80029DC4 *, s32);
extern u32 func_800166B0(struct Actor_80029DC4 *);
extern void func_8003318C(s32, s32, s32, s32, s32, s32, s32, s32);
extern void func_80033144(s32, s32, s32, s32);
extern void *func_80032870(s32);
extern void func_8003295C(void *, s32, s32);
extern void func_800139F8(char *, s32, s32);

s32 func_80029DC4(struct Actor_80029DC4 *obj, u32 input, s32 arg2) {
    char buf[0x28];
    s32 ret = 0;
    s32 i;
    s32 dx;
    s32 dy;
    s32 x0, y0, x1, y1;
    s32 lx, ly;
    u16 sel;
    Gfx_80029DC4 *g;

    if (obj->unk11C == -1) {
        func_8002E478(obj, D_8005438C);
        input = func_800166B0(obj);
    }

    sel = D_8005438C;
    dx = ((sel & 1) * 2 - 1) * 120;
    x0 = dx + 0xBE;
    dy = ((sel & 2) - 1) * 36;
    y0 = dy + 0x69;
    x1 = dx + 0x142;
    y1 = dy + 0xB2;
    func_8003318C(x0, y0, x1, y1, 0xD6, 0xBD, 0xA5, 0x96);
    func_8003318C(x0, dy + 0x68, x1, y0, 0, 0, 0, 200);
    func_8003318C(x1, y0, dx + 0x143, y1, 0, 0, 0, 200);
    func_8003318C(dx + 0xBD, y0, x0, y1, 0, 0, 0, 200);
    func_8003318C(x0, y1, x1, dy + 0xB3, 0, 0, 0, 200);

    g = D_80077830++;
    g->w0 = 0xE7000000;
    g->w1 = 0;
    func_80033144(0xFF, 0xFF, 0xFF, 0xFF);

    for (i = 0; i < 4; i++) {
        lx = ((i & 1) * 2 - 1) * 120;
        ly = ((i & 2) - 1) * 36;
        func_8003295C(func_80032870(i + 0x1C5), lx + 0xEC, ly + 0x6B);
        func_8003295C(func_80032870(i + 0x1EA), lx + 0x120, ly + 0x73);
        func_800139F8(D_80052FD0[i], lx + 0x100, ly + 0x8E);
        func_800139F8("Zone", lx + 0x100, ly + 0x9D);
    }

    if (obj->unk11C == -1) {
        func_8002E478(obj, D_8005438C);
    }

    if (input & 4) {
        if (D_8005438C & 2) {
            D_8005438C -= 2;
        }
    } else if (input & 8) {
        if (!(D_8005438C & 2)) {
            D_8005438C += 2;
        }
    } else if (input & 1) {
        if (D_8005438C & 1) {
            D_8005438C -= 1;
        }
    } else if (input & 2) {
        if (!(D_8005438C & 1)) {
            D_8005438C += 1;
        }
    }

    if (input & 0x10) {
        ret = D_8005438C + 1;
        D_8005438C = 0;
    }
    return ret;
}
