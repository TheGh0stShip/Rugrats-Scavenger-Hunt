typedef unsigned char u8;
typedef short s16;
typedef int s32;

typedef struct {
    s16 x;
    s16 y;
} Vec2s;

typedef struct {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} Rect4s;

typedef struct {
    u8 r0, r;
    u8 g0, g;
    u8 b0, b;
} Color6;

typedef struct {
    s16 id;
    u8 pad_002[0x210 - 2];
} Entry210;

typedef struct {
    u8 pad_0000[4];
    s16 count;
    u8 pad_0006[0x201C - 6];
    Entry210 entries[1];
} Track;

typedef struct {
    u8 pad_000[0xF8];
    s32 unkF8;
    u8 pad_0FC[0x150 - 0xFC];
    s16 unk150;
} Racer;

extern s32 D_80052F18;
extern Track *D_800581F0;
extern s32 D_80057D5C;
extern s32 D_80057D58;
extern Vec2s D_800534F0[];
extern Color6 D_800534C0[];
extern Rect4s D_80053480[];
extern Vec2s D_80053548[];
extern u8 D_80001640[];

extern s32 func_8000D050(s32);
extern void func_8000D0AC(s32);
extern void func_80033144(s32, s32, s32, s32);
extern s32 func_80032870(s32);
extern void func_80032A20(s32, s32, s32);
extern void func_8003295C(s32, s32, s32);
extern Racer *func_800055A4(s32);
extern void func_80019DE0(s32, s16 *, s16 *);
extern void func_80033290(s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);
extern void func_80033118(void);
extern void func_8001380C(s32, void *, s32, s32);

void func_80019E3C(void) {
    s32 i;
    s32 j;
    Racer *p;
    s16 *q;

    func_8000D0AC(func_8000D050(0x1C3));
    func_80033144(0xFF, 0xFF, 0xFF, 0xFF);
    func_80032A20(func_80032870(0x1E1), D_800534F0[D_80052F18 + 20].x, D_800534F0[D_80052F18 + 20].y);

    for (i = 0; i < D_800581F0->count - 1; i++) {
        s32 k;
        k = D_800581F0->entries[i].id;
        func_80032A20(func_80032870(k + 0x18E), D_800534F0[k - 0x3F].x, D_800534F0[k - 0x3F].y);
    }

    for (i = 0; i < D_80057D5C; i++) {
        s32 k;
        p = func_800055A4(i);
        k = p->unkF8;
        func_80019DE0(p->unk150, &D_80053480[i].w, &D_80053480[i].w + 1);
        func_80033290(D_80053480[i].x, D_80053480[i].y, D_80053480[i].x,
                      (s16)(D_80053480[i].y - 20), D_80053480[i].w, D_80053480[i].h,
                      0, 0, 0, -1200, D_800534C0[k].r, D_800534C0[k].g, D_800534C0[k].b, 0xA3);
    }

    for (i = 0; i < D_80057D58; i++) {
        s32 k;
        s32 *mode = &D_80052F18;
        p = func_800055A4(i + D_80057D5C);
        k = p->unkF8;
        if (*mode == 1 && k == 4) {
            k = 7;
        }
        q = &D_80053480[k].w;
        func_80019DE0(p->unk150, q, q + 1);
        func_80033290(D_80053480[k].x, D_80053480[k].y, D_80053480[k].x,
                      (s16)(D_80053480[k].y - 20), D_80053480[k].w, D_80053480[k].h,
                      0, 0, 0, -1200, D_800534C0[k].r, D_800534C0[k].g, D_800534C0[k].b, 0xA3);
    }

    func_80033144(0xFF, 0xFF, 0xFF, 0xFF);

    for (i = 0; i < D_80057D5C; i++) {
        j = func_800055A4(i)->unkF8;
        func_8003295C(func_80032870(j + 0x1C5),
                      (s16)(D_800534F0[i].x + D_80053548[j].x),
                      (s16)(D_800534F0[i].y + D_80053548[j].y));
    }

    for (i = 0; i < D_80057D58; i++) {
        j = i + D_80052F18;
        func_80032A20(func_80032870(j + 0x1C9), D_800534F0[j + 4].x, D_800534F0[j + 4].y);
    }

    func_80033118();
    func_8001380C(0x3A, D_80001640, 0x6E, 0x1E);
}
