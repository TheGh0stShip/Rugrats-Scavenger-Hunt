typedef unsigned char u8;
typedef unsigned int u32;
typedef signed short s16;
typedef signed int s32;

typedef struct State_800C3E24 {
    u8 pad_000[0x1AC];
    u32 flags[4];
} State_800C3E24;

typedef struct Pos_800549FC {
    s16 x;
    s16 y;
} Pos_800549FC;

extern State_800C3E24 *D_800C3E24;
extern State_800C3E24 *D_800C3E30;
extern Pos_800549FC D_800549FC[4];
extern void func_80033144(s32 r, s32 g, s32 b, s32 a);
extern void *func_80032870(s32 id);
extern void func_80032A20(void *t, s32 x, s32 y);

void func_80034350(void) {
    s32 i;
    Pos_800549FC *pos;

    func_80033144(0xFF, 0xFF, 0xFF, 0xFF);
    pos = D_800549FC;
    for (i = 0; i < 4; i++, pos++) {
        if (!(D_800C3E24->flags[i] & 0x80000000) &&
            (D_800C3E30->flags[i] & 0x80000000)) {
            func_80032A20(func_80032870((i + 1) % 4 + 0x3C), pos->x, pos->y);
        }
    }
}
