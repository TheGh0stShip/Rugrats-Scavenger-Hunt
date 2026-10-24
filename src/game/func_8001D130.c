typedef unsigned char u8;
typedef short s16;
typedef int s32;

typedef struct {
    s16 x;
    s16 y;
} ScreenPos;

typedef struct {
    u8 pad_00[0xF4];
    s32 unkF4;
} Player_8005B214;

extern s32 D_80052F10;
extern Player_8005B214 *D_8005B214;
extern s32 D_80057D5C;
extern ScreenPos D_800535B0;
extern ScreenPos D_800535B4;
extern void func_8001CF20(ScreenPos, Player_8005B214 *);
extern void func_8001D034(ScreenPos, Player_8005B214 *);
extern void func_8001D0F0(void);

void func_8001D130(void) {
    if (D_80052F10 != 0) {
        if (D_8005B214->unkF4 < D_80057D5C) {
            func_8001CF20(D_800535B0, D_8005B214);
            func_8001D034(D_800535B4, D_8005B214);
            func_8001D0F0();
        }
    }
}
