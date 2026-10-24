typedef unsigned char u8;
typedef signed char s8;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

struct Player_800055A4 {
    u8 pad_00[0x118];
    u32 flags;
    u8 pad_11C[0x34];
    s16 team;
};

struct Cam_80052F04 {
    u8 pad_00[0x2C];
    float unk2C;
};

struct State_80058C00 {
    u8 pad_00[4];
    s32 timer;
    s32 choice;
};

extern struct State_80058C00 D_80058C00;
extern struct Player_800055A4 *D_800529F0;
extern struct Cam_80052F04 *D_80052F04;
extern u32 D_80057D5C;
extern u32 D_80057D58;
extern s8 D_800581E9;
extern const float D_80001918;
extern s32 func_8001BF3C(void);
extern void func_8001BF24(s32);
extern void func_800124B0(s32);
extern s32 func_800124A0(void);
extern void func_8000FDAC(s32, void *);
extern s32 func_8003A840(void);
extern struct Player_800055A4 *func_800055A4(u32);
extern void func_80026560(s32);
extern s32 func_8000E008(s32);
extern void func_8000DB84(s32);
extern s32 func_8001C2C8(void);
extern void func_8001A5C0(void);

void func_8001C77C(void)
{
    u32 i;

    switch (func_8001BF3C()) {
        case 0x33:
            func_800124B0(D_800529F0->team);
            D_80058C00.timer = 50;
            func_8000FDAC(0xB, func_800124A0());
            D_80052F04->unk2C = D_80001918;
            D_80058C00.choice = func_8003A840() % 3;
            func_8001BF24(0x34);
            for (i = 0; i < D_80057D5C + D_80057D58; i++) {
                func_800055A4(i)->flags |= 0x100000;
            }
            break;
        case 0x34:
            if (D_80058C00.timer > 0) {
                D_80058C00.timer--;
                func_80026560(D_800581E9);
                if (func_8000E008(D_800581E9) == 0) {
                    func_8000DB84(D_800581E9);
                }
            } else {
                func_8001BF24(0x35);
            }
            break;
        case 0x35:
            if (func_8001C2C8() != 0) {
                func_8001A5C0();
            }
            break;
    }
}
