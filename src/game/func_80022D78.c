typedef signed char s8; typedef unsigned char u8; typedef short s16; typedef unsigned short u16; typedef int s32; typedef unsigned int u32;

struct Obj_80022D78 {
    u8 pad_000[0x190];
    s32 unk190;
    u8 pad_194[0x8];
    s32 unk19C;
    u8 pad_1A0[0x6];
    s8 unk1A6;
    s8 unk1A7;
};

extern s32 D_8005B7EC;
extern s32 func_80026010(struct Obj_80022D78 *);
extern s32 func_8003A840(void);

void func_80022D78(struct Obj_80022D78 *obj) {
    s32 n = (s16)func_80026010(obj);

    if (obj->unk1A6 < 10 && obj->unk1A7 >= 21) {
        obj->unk190 = 4;
        D_8005B7EC = 0;
        return;
    }
    if (n != 0 && func_8003A840() % 4 == 1) {
        s32 t = func_8003A840() % n;
        obj->unk190 = 2;
        obj->unk19C = t;
        return;
    }
    obj->unk190 = 1;
}
