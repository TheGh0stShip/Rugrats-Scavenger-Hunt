/* Target: func_8001D418  VRAM 0x8001D418  ROM 0x1E018
 * Size: 0x504  Compiler: SN Systems GCC 2.8 (-O2) */

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
    u8 pad_000[0xF4];
    s32 unkF4;
    u8 pad_0F8[0x44];
    u8 unk13C;
    u8 unk13D;
    u8 pad_13E[6];
    s32 unk144;
    u8 pad_148[8];
    s16 unk150;
    u8 pad_152[0x56];
    u8 unk1A8;
} Player;

typedef struct {
    s32 mode;
    u8 pad_04[4];
    s32 unk8;
} Settings_80052F18;

typedef struct {
    f32 unk0;
    f32 unk4;
    f32 unk8;
    u8 pad_0C[0x28];
    s32 unk34;
    u8 pad_38[4];
} Cam_8005368C;

extern s32 D_80057D5C;
extern ScreenPos D_800535D8;
extern Settings_80052F18 D_80052F18;
extern s8 D_800581E9;
extern char D_8005B218[];
extern Cam_8005368C D_8005368C[5];
extern s32 D_8005361C[];
extern s16 D_8005B7F2;
extern Player *D_8005B214;
extern s32 D_800529F8;
extern char D_80001898[];

extern void func_80033144(s32, s32, s32, s32);
extern void *func_80032870(s32);
extern void func_80032A20(void *, s32, s32);
extern void func_800329C4(void *, s32, s32);
extern s32 func_8001F588(s32);
extern s16 func_8000E154(s16, s32);
extern s16 func_8000DF14(u8, u8);
extern s32 func_8000E108(void);
extern void func_800137E8(s32);
extern void func_80013744(void);
extern int func_8003A57C(char *, const char *, ...);
extern void func_800138FC(char *, s32, s32);
extern u8 *func_800197C8(s32);
extern void func_8000CE7C(void *, s32);
extern void func_8001A754(void);
extern void func_8001FB84(s32, s32, s32, s32);

void func_8001D418(Player *player, s32 arg1) {
    char *name;
    s32 i;
    s32 v;
    s32 t;

    if (player->unkF4 >= D_80057D5C) {
        return;
    }
    func_80033144(0xFF, 0xFF, 0xFF, 0x8C);
    func_80032A20(func_80032870(0xC), D_800535D8.x, D_800535D8.y);
    func_8001F588(0);
    if (D_80052F18.mode == 2) {
        switch ((u32)func_8000E154(player->unk150, player->unk144)) {
            case 1:
                name = "Tommy World";
                break;
            case 2:
                name = "Chuckie World";
                break;
            case 3:
                name = "Lil World";
                break;
            case 4:
                name = "Phil World";
                break;
            case 5:
                name = "Jibomizer Lever";
                break;
            case 13:
                name = "Back to Volcano";
                break;
            case 0:
            default:
                name = "Not Implemented";
                break;
        }
    } else {
        v = func_8000DF14(player->unk13D, player->unk13C);
        if (D_80052F18.unk8 == 0) {
            v &= 0x7FFF;
        }
        switch ((u32)v) {
            case 12:
                name = "?";
                break;
            case 1:
                name = "Search";
                break;
            case 2:
                name = "Go to the Crib Room";
                break;
            case 13:
                name = "Sandbox";
                break;
            case 7:
                name = "Gain 2 Cookies";
                break;
            case 8:
                name = "Gain 4 Cookies";
                break;
            case 6:
                name = "Lose 2 Cookies";
                break;
            case 5:
                name = "Lose 1 Cookie";
                break;
            case 9:
                name = "Trade For A Toy Card";
                break;
            case 0xF5:
            case 0xF6:
            case 0xF7:
            case 0xF8:
                name = "Door";
                break;
            case 10:
                name = "Mystery";
                break;
            case 0xF9:
            case 0xFA:
            case 0xFB:
            case 0xFC:
                name = "Secret Passage";
                break;
            case 3:
                name = "Gain 2 Energy";
                break;
            case 4:
                name = "Gain 5 Energy";
                break;
            case 11:
                t = D_800581E9;
                if (t == func_8000E108()) {
                    name = "Pick A Door";
                    break;
                }
                goto blank;
            default:
            blank:
                name = "";
                break;
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
                if (D_80052F18.mode == 0) {
                    name = "Door To Temple";
                } else {
                    name = "Door To Ship";
                }
                break;
        }
    }
    func_800137E8(4);
    func_80013744();
    func_8003A57C(D_8005B218, "%s", name);
    func_800138FC(D_8005B218, D_800535D8.x + 5, D_800535D8.y + 4);
    func_8000CE7C(func_800197C8(D_8005368C[1].unk34) + 0x2C, D_8005361C[arg1] * 6);
    D_8005B7F2 = 1;
    for (i = 4; i >= 0; i--) {
        D_8005368C[i].unk8 = -21000.0f;
    }
    t = D_800581E9;
    if (t != func_8000E108()) {
        f32 f = 10000.0f;
        if (arg1 != 0) {
            f = 1000.0f;
        }
        D_8005368C[1].unk0 = f;
        D_8005368C[1].unk4 = -800.0f;
        D_8005368C[1].unk8 = -6000.0f;
    }
    func_8001A754();
    func_800137E8(0x10);
    func_8001FB84(0, 0xFF, 0, 0xFF);
    func_8003A57C(D_8005B218, D_80001898, arg1);
    if (D_8005B214->unk1A8 == 0 || D_800529F8 == 0) {
        switch ((u32)func_8000DF14(D_8005B214->unk13D, D_8005B214->unk13C)) {
            case 11:
            case 0xF5:
            case 0xF6:
            case 0xF7:
            case 0xF8:
                break;
            default:
                func_80033144(0xFF, 0xFF, 0xFF, 0xFF);
                func_800329C4(func_80032870(0x37), 0x1B0, (s16)(*(u16 *)&D_800535D8.y + 4));
                break;
        }
    }
}
