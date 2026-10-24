typedef signed char s8; typedef unsigned char u8; typedef short s16; typedef unsigned short u16; typedef int s32; typedef unsigned int u32;

struct Unk_80052F04 {
    u8 pad_00[0xF0];
    s32 unkF0;
};

struct Obj_800239E8 {
    u8 pad_000[0xF8];
    s32 unkF8;
    u8 pad_0FC[0x1C];
    u32 unk118;
    u8 pad_11C[0x14];
    s32 unk130;
    u8 pad_134[0x10];
    s32 unk144;
    u8 pad_148[0x8];
    s16 unk150;
    u8 pad_152[0x4];
    s16 unk156;
    u8 pad_158[0x50];
    u8 unk1A8;
};

extern s32 D_800581C0[];
extern struct Unk_80052F04 *D_80052F04;
extern s32 D_800529F8;
extern void func_800069C8(s32, struct Obj_800239E8 *, s32);
extern void func_80005500(s32, struct Obj_800239E8 *);
extern void func_8000550C(s32, struct Obj_800239E8 *);
extern void func_8000DFB4(s16, s32, s32, s32);
extern int func_8001F138(s32);
extern s32 func_8001BF00(void);
extern void func_8001BED8(s32);
extern void func_8001BF24(s32);

void func_800239E8(struct Obj_800239E8 *obj) {
    D_800581C0[obj->unkF8] = 0;
    if (obj->unk118 & 0x8000) {
        func_800069C8(0x22, obj, 0);
        D_80052F04->unkF0 = 0;
        func_80005500(7, obj);
        if (obj->unk118 & 0x40000) {
            func_8000DFB4(obj->unk150, obj->unk144, 3, obj->unk156);
            obj->unk118 &= ~0x40000;
        } else if (obj->unk118 & 0x80000) {
            func_8000DFB4(obj->unk150, obj->unk144, 2, obj->unk156);
            obj->unk118 &= ~0x80000;
        }
    } else if (obj->unk118 & 0x20000) {
        func_800069C8(0x24, obj, 0);
        D_80052F04->unkF0 = 0;
        obj->unk1A8 = 0;
        D_800529F8 = 0;
        obj->unk130 = 0;
        func_8001F138(0x4C);
        func_80005500(6, obj);
        func_8000550C(3, obj);
        obj->unk118 &= ~0x20000;
        func_8000DFB4(obj->unk150, obj->unk144, 4, 1);
    } else if (func_8001BF00() != 0x30) {
        func_8001BED8(4);
        func_8001BF24(0);
        func_80005500(0, obj);
        func_8000550C(1, obj);
    }
}

