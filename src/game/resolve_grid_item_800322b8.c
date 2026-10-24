/* Target: func_800322B8  VRAM 0x800322B8  ROM 0x32EB8
 * Size: 0x4E8  Compiler: SN GCC 2.8 (-O2), normalized SN pipeline
 * Confidence: exact
 */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

typedef struct Vec3f { f32 x, y, z; } Vec3f;

typedef struct Cell_800581F0 {
    s16 unk0;   /* 0x0: in cell 0 of a row, number of live items */
    u8 pad_02[0xA];
    s16 unkC;   /* 0xC: item id or -1 */
    u8 pad_0E[0x2];
} Cell_800581F0;

typedef struct Grid_800581F0 {
    u8 pad_0000[0x2020];
    Cell_800581F0 cells[1][33]; /* 0x2020 */
} Grid_800581F0;

typedef struct Big_800B9378 {
    u8 pad_0000[0xA044];
    union {
        s32 w;
        struct { s16 hi; s16 lo; } h;
    } cur;      /* 0xA044 = D_800C33BC */
} Big_800B9378;

typedef struct Obj_800322B8 {
    u8 pad_000[0xF8];
    s32 unkF8;  /* 0xF8 */
} Obj_800322B8;

typedef struct Ctx_80057F70 {
    u8 pad_000[0x218];
    void *unk218;  /* 0x218 */
    s32 unk21C;    /* 0x21C */
} Ctx_80057F70;

extern Big_800B9378 D_800B9378;
extern u8 D_800B9370;
extern u8 D_800B9371;
extern s8 D_800581E9;
extern Grid_800581F0 *D_800581F0;
extern s32 D_80054814;
extern u32 D_8005480C;
extern s32 D_800547D8;
extern Vec3f D_800547CC;
extern Vec3f D_800547DC[4];
extern Ctx_80057F70 D_80057F70;
extern s32 D_80058180;
extern s32 D_80052F18;
extern void *D_800529F0;
extern void *D_800529F4;

extern void *func_800197C8(s32 id);
extern s32 func_80026040(Obj_800322B8 *obj, s32 id);
extern int func_8001F1C4(s32 id);
extern int func_8001F138(s32 id);
extern s32 func_8003131C(s32 id);
extern s32 func_80025E04(void *who, s32 id);
extern void func_800093D0(s32 id);
extern s32 func_80025C20(s32 id);
extern void *func_800055A4(s32 id);
extern void func_80010E24(int, s32 n);
extern void func_8000DB84(s32 row);
extern void func_8000916C(s32 row, s32 col, s32 id);
extern void func_8001BF24(s32 n);

void func_800322B8(Obj_800322B8 *obj) {
    s32 keep;
    s32 idx;
    Vec3f *t;
    f32 base;
    s32 four;

    D_80054814 = 0;
    D_8005480C = D_800581F0->cells[D_800581E9][D_800B9378.cur.w].unkC;
    func_800197C8(D_8005480C);
    four = 4;
    keep = obj->unkF8 != four;

    if (D_8005480C == 0x178) goto c178;
        __asm__ __volatile__("" ::: "$2");
    if (D_8005480C < 0x178) goto def;
    if (D_8005480C >= 0x17C) goto def;
        switch (D_8005480C) {
        case 0x17A:
            D_800547D8 = 3;
            break;
        case 0x17B:
            D_800547D8 = four;
            break;
        case 0x179:
            D_800547D8 = 5;
            break;
        }
        if (obj->unkF8 != 4) {
            if (func_80026040(obj, D_8005480C) == 0) {
                keep = 0;
                func_8001F1C4(obj->unkF8 + 0xB8);
            } else {
                switch (D_8005480C) {
                case 0x17A:
                    func_8001F1C4(obj->unkF8 + 0x9C);
                    break;
                case 0x17B:
                    func_8001F1C4(obj->unkF8 + 0xAC);
                    break;
                case 0x179:
                    func_8001F1C4(obj->unkF8 + 0xA4);
                    break;
                }
            }
        }
    goto out;
c178:
        if (obj->unkF8 != four) {
            func_8001F1C4(obj->unkF8 + 0xA0);
        }
        D_800547D8 = 6;
    goto out;
def:
        if (obj->unkF8 == 4) {
            if (func_8003131C(D_8005480C) != 0) {
                func_80025E04(obj, D_8005480C);
                keep = 1;
                func_8001F1C4(0x45);
                D_800547D8 = 2;
            } else if (D_80057F70.unk21C != 0) {
                func_80025E04(D_80057F70.unk218, D_8005480C);
                D_800547D8 = 7;
                func_8001F138(2);
            } else {
                D_800547D8 = 1;
                func_8001F138(8);
            }
        } else if (func_8003131C(D_8005480C) != 0) {
            D_800547D8 = 2;
            if (D_80058180 < 0) {
                func_800093D0(D_8005480C);
                func_8001F138(8);
                func_8001F1C4(obj->unkF8 + 0xB4);
            } else {
                func_8001F138(8);
                keep = 0;
                func_8001F1C4(obj->unkF8 + 0xB8);
            }
        } else if (D_80052F18 == 0) {
            func_8001F1C4(obj->unkF8 + 0xB0);
            keep = func_80025E04(func_800055A4(func_80025C20(D_8005480C)), D_8005480C) != 0;
            D_800547D8 = 1;
            func_80010E24(D_800529F4, 0x14);
        } else {
            keep = func_80025E04(D_800529F0, D_8005480C) != 0;
            func_8001F1C4(obj->unkF8 + 0xA8);
            D_800547D8 = 7;
            func_80010E24(D_800529F4, 0x14);
        }
out:;


    if (keep) {
        D_800581F0->cells[D_800581E9][D_800B9378.cur.w].unkC = -1;
        if (--D_800581F0->cells[D_800581E9][0].unk0 == 0) {
            func_8000DB84(D_800581E9);
        }
    } else {
        func_8000916C(D_800581E9, D_800B9378.cur.h.lo,
                      D_800581F0->cells[D_800581E9][D_800B9378.cur.w].unkC);
    }

    func_8001BF24(0x14);
    D_800547CC.x = 0.0f;
    D_800547CC.y = 0.0f;
    D_800547CC.z = -1500.0f;
    idx = D_8005480C - 0x190;
    if ((u32)idx < 4) {
        base = D_800547CC.x;
        D_800547CC.x = D_800547DC[idx].x + base;
        D_800547CC.y = D_800547DC[idx].y + base;
    } else if ((u32)(D_8005480C - 0x17C) < 0x14) {
        D_800547CC.y = -200.0f;
        D_800547CC.z = -3000.0f;
    }
    D_800B9371 = 0x14;
    D_800547CC.z += -8500.0f;
    D_800B9370 = 1;
    __asm__ __volatile__("");
}
