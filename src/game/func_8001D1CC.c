typedef unsigned char u8;
typedef signed char s8;
typedef short s16;
typedef int s32;

typedef struct {
    s16 x;
    s16 y;
} ScreenPos;

typedef struct {
    u8 pad_000[0xF8];
    s32 unkF8;
    u8 pad_0FC[0xAA];
    s8 unk1A6;
    s8 unk1A7;
} Player;

extern s32 D_80057D5C;
extern s32 D_80057D40;
extern ScreenPos D_800535B8[];
extern ScreenPos D_800535C8[];
extern Player *func_800055A4(s32);
extern void func_8001CF20(ScreenPos, Player *);
extern void func_8001D034(ScreenPos, Player *);
extern void func_80033144(s32, s32, s32, s32);
extern void *func_80032870(s32);
extern void func_80032A20(void *, s32, s32);
extern void func_8001D0F0(void);

void func_8001D1CC(void) {
    s32 i;
    Player *p;

    for (i = 0; i < D_80057D5C; i++) {
        p = func_800055A4(i);
        func_8001CF20(D_800535B8[i], p);
        func_8001D034(D_800535C8[i], p);
    }
    func_80033144(0xFF, 0xFF, 0xFF, 0xFF);
    func_80032A20(func_80032870(0xB), D_800535B8[D_80057D40].x,
                  D_800535B8[D_80057D40].y);
    func_8001D0F0();
}
