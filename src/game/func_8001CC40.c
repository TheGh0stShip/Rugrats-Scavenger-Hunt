typedef unsigned char u8;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef unsigned int u32;

struct Player_800055A4 {
    u8 pad_00[0x118];
    u32 flags;
    s32 unk11C;
    u8 pad_120[0x1C];
    u8 unk13C;
    u8 unk13D;
    u8 pad_13E[0x12];
    s16 team;
    u8 pad_152[0x54];
    s8 unk1A6;
};

struct State_8005B320 {
    s32 count;
    s32 list[4];
    s32 sel;
    s32 next;
};

struct Game_80052F18 {
    s32 mode;
    u8 pad_04[8];
    s32 character[4];
};

extern struct State_8005B320 D_8005B320;
extern struct Player_800055A4 *D_800529F0;
extern struct Game_80052F18 D_80052F18;
extern char D_8000191C[];
extern char D_80001930[];
extern char D_8000193C[];
extern void func_800139F8(char *, s32, s32);
extern void func_80033118(void);
extern void *func_80032870(s32);
extern void func_8003295C(void *, s32, s32);
extern void func_80023544(void);
extern u32 func_800166B0(struct Player_800055A4 *);
extern void func_8001BED8(s32);
extern void func_8001BF24(s32);

void func_8001CC40(void) {
    s32 choice = -1;
    s32 i;
    s16 y;
    u32 buttons;

    func_800139F8(D_8000191C, 0x100, 0x8C);
    func_800139F8((D_8005B320.next == 0x21) ? D_80001930 : D_8000193C,
                  0x100, 0xA0);
    func_80033118();
    for (i = 0; i < D_8005B320.count; i++) {
        y = i * 100 + 0x9C;
        func_8003295C(
            func_80032870(D_80052F18.character[D_8005B320.list[i]] + 0x1C5),
            y, 0x41);
        func_8003295C(func_80032870(i + 0x3C), y, 0x69);
    }
    if (D_800529F0->unk11C == -1) {
        func_80023544();
    }
    buttons = func_800166B0(D_800529F0);
    if (buttons & 0x400) {
        choice = 0;
    } else if (buttons & 0x1000) {
        choice = 1;
    } else if (buttons & 0x800) {
        choice = 2;
    }
    if (choice >= D_8005B320.count) {
        choice = -1;
    }
    if (choice >= 0) {
        D_8005B320.sel = D_8005B320.list[choice];
        func_8001BED8(D_8005B320.next);
        func_8001BF24(0x32);
    }
}
