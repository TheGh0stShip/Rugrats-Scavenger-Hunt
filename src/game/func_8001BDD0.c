typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

struct Task_80053634 {
    u8 pad_00[8];
    s16 unk08;
    s16 unk0A;
    s32 unk0C;
    void (*func10)(void);
    u8 pad_14[4];
    void (*func18)(void);
    s32 unk1C;
    s32 unk20;
    s32 unk24;
    s32 unk28;
    u8 pad_2C[0x14];
};

extern struct Task_80053634 *D_80053634;
extern s32 D_80059D40;
extern s32 D_80052E90;
extern s32 D_8005B318;
extern void *D_80059DA8;
extern void *func_8001A2B0(u32);
extern void *func_80038E90(void *, s32, s32);
extern void func_8001D91C(void);
extern void func_8001DEC0(void);
extern void func_8001DEF8(void);
extern void func_80007EFC(void *, void *, int, void *);
extern void func_8001BD8C(void);
extern void func_8001BDC0(void);

void func_8001BDD0(void)
{
    D_80053634 = func_8001A2B0(0x40);
    func_80038E90(D_80053634, 0, 0x40);
    D_80053634->unk08 = 6;
    D_80053634->unk0A = 0;
    D_80053634->unk0C = 0;
    if (D_80059D40 == 8) {
        D_80053634->func10 = func_8001D91C;
    } else {
        D_80053634->func10 = 0;
    }
    D_80053634->func18 = func_8001DEC0;
    D_80053634->unk1C = 0;
    D_80053634->unk20 = 0;
    D_80053634->unk24 = 0;
    if (D_80052E90 == 0) {
        D_80053634->unk28 = 0x30;
        D_8005B318 = -1;
    } else {
        D_80053634->unk28 = 0;
        func_8001DEF8();
    }
    func_80007EFC(D_80053634, D_80059DA8, 4, 0);
    if (D_80059D40 == 8) {
        func_8001BD8C();
        func_8001BDC0();
    }
}
