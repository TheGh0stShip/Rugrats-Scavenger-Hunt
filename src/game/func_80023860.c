typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;

struct Obj_80023860 {
    u8 pad_000[0x130];
    s32 unk130;
    u8 pad_134[0x8];
    u8 unk13C;
    u8 unk13D;
    u8 pad_13E[0x12];
    s16 unk150;
    u8 pad_152[0x54];
    s8 unk1A6;
    u8 pad_1A7;
    u8 unk1A8;
    u8 unk1A9;
};

extern s32 D_800529F8;
extern s16 func_8000E108(void);
extern void func_80005500(s32, struct Obj_80023860 *);
extern void func_8000550C(s32, struct Obj_80023860 *);
extern void func_80026294(struct Obj_80023860 *, s32);
extern void func_80011CC4(s32);
extern s16 func_8000DF14(u8, u8);
extern u16 func_8000DF40(s16);
extern s32 *func_8001BF60(s32);
extern void func_8001BED8(s32);
extern void func_8001BF24(s32);
extern s32 func_8001BF00(void);
extern void func_80024BDC(struct Obj_80023860 *);

void func_80023860(struct Obj_80023860 *obj) {
    u16 tile;

    if (obj->unk150 == func_8000E108()) {
        D_800529F8 = 5;
        obj->unk130 = 5;
        obj->unk1A8 = 0;
        func_80005500(14, obj);
        func_80026294(obj, 30);
        func_80011CC4(3);
        return;
    }
    tile = func_8000DF40(func_8000DF14(obj->unk13D, obj->unk13C));
    if (obj->unk1A9 != 0) {
        obj->unk1A9 = 0;
        if ((u16)(tile - 3) < 2) {
            if ((s16)tile == 3) {
                *func_8001BF60(1) = 2;
            } else {
                *func_8001BF60(1) = 5;
            }
            func_8001BED8(0x1C);
            *func_8001BF60(0) = 10;
            *func_8001BF60(2) = 1;
            func_80005500(0, obj);
        }
    } else if (obj->unk1A6 == 0) {
        func_80026294(obj, 30);
        obj->unk1A8 = 0;
        func_80024BDC(obj);
    } else if (func_8001BF00() != 0x30) {
        func_8001BED8(4);
        func_8001BF24(0);
        func_80005500(0, obj);
        func_8000550C(1, obj);
    }
}
