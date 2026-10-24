typedef unsigned char u8;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef unsigned int u32;

struct Player_800055A4 {
    u8 pad_00[0x118];
    u32 flags;     /* 0x118 */
    s32 unk11C;    /* 0x11C */
    u8 pad_120[0x1C];
    u8 unk13C;     /* 0x13C */
    u8 unk13D;     /* 0x13D */
    u8 pad_13E[0x12];
    s16 team;      /* 0x150 */
    u8 pad_152[0x54];
    s8 unk1A6;     /* 0x1A6 */
};

struct State_8005B320 {
    s32 count;     /* 0x00 */
    s32 list[4];   /* 0x04 */
    s32 sel;       /* 0x14 */
    s32 next;      /* 0x18 */
};

extern struct State_8005B320 D_8005B320;
extern struct Player_800055A4 *D_800529F0;
extern s32 D_80057D5C;
extern s32 func_8001BF3C(void);
extern struct Player_800055A4 *func_800055A4(s32);
extern void func_8001BED8(s32);
extern s32 func_8000E108(void);
extern void func_80006468(struct Player_800055A4 *, s32, s32);
extern void func_80005500(s32, void *);

void func_8001C958(void) {
    s32 i;

    switch (func_8001BF3C()) {
        case 0x31:
            D_8005B320.count = 0;
            D_8005B320.sel = -1;
            for (i = 0; i < D_80057D5C; i++) {
                if (func_800055A4(i) != D_800529F0) {
                    D_8005B320.list[D_8005B320.count] = i;
                    D_8005B320.count++;
                }
            }
            func_8001BED8(0x22);
            D_8005B320.next = 0x20;
            break;
        case 0x32:
            if (func_800055A4(D_8005B320.sel)->team == func_8000E108()) {
                func_800055A4(D_8005B320.sel)->unk1A6 = 15;
            }
            func_80006468(func_800055A4(D_8005B320.sel), D_800529F0->unk13C, D_800529F0->unk13D);
            func_800055A4(D_8005B320.sel)->flags &= ~0x100000;
            func_8001BED8(0);
            D_800529F0->flags |= 0x10;
            func_80005500(6, D_800529F0);
            break;
    }
}

