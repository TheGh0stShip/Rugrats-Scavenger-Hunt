/* Target: func_8001CF20  VRAM 0x8001CF20  ROM 0x1DB20
 * Size: 0x114  Compiler: SN GCC 2.8 (-O2), reviewed scheduling normalization */
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
    u8 pad_000[0xF8];
    s32 unkF8;
    u8 pad_0FC[0xAA];
    s8 unk1A6;
    s8 unk1A7;
} Player;

extern char D_8005B218[];
extern char D_80001898[];
extern void func_80033144(s32, s32, s32, s32);
extern void *func_80032870(s32);
extern void func_80032A20(void *, s32, s32);
extern int func_8001F588(s32);
extern void func_80013724(void);
extern void func_80013744(void);
extern void func_800137E8(s32);
extern int func_8003A57C(char *, const char *, ...);
extern void func_800138FC(char *, s32, s32);

void func_8001CF20(ScreenPos pos, Player *player) {
    s32 v;
    s32 n;

    v = player->unkF8;
    func_80033144(0xFF, 0xFF, 0xFF, 0xA0);
    func_80032A20(func_80032870(v + 0x11), pos.x, pos.y);
    func_80032A20(func_80032870(0xA), pos.x + 0x28, pos.y);
    func_8001F588(0);
    func_80013724();
    func_800137E8(0x10);
    n = player->unk1A6;
    if (n < 0) {
        n = 0;
    }
    func_8003A57C(D_8005B218, D_80001898, n);
    func_800138FC(D_8005B218, pos.x + 0x6C, pos.y + 5);
    func_800137E8(0x10);
    func_80013744();
    func_8003A57C(D_8005B218, D_80001898, player->unk1A7);
    func_800138FC(D_8005B218, pos.x + 0xAF, pos.y + 5);
}
