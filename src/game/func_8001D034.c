typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef float f32;

typedef struct {
    s16 x;
    s16 y;
} ScreenPos;

typedef struct {
    u8 pad_000[0x1BC];
    s32 unk1BC[3];
} Hud_8001D034;

extern s32 D_80058180;
extern void func_80013724(void);
extern void *func_80032870(s32);
extern void func_80032A20(void *, s32, s32);

void func_8001D034(ScreenPos pos, Hud_8001D034 *hud) {
    s32 i;
    s32 dx;

    func_80013724();
    for (i = 0, dx = 0; i < 3; i++) {
        if (hud->unk1BC[i] != 0) {
            func_80032A20(func_80032870(hud->unk1BC[i]), pos.x + dx, pos.y);
        }
        dx += 45;
    }
    if (D_80058180 >= 0) {
        func_80032A20(func_80032870(0x15), pos.x + i * 45, pos.y);
    }
}

