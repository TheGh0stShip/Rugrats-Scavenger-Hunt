/* Target: func_800231B4  VRAM 0x800231B4  ROM 0x23DB4
 * Size: 0xC0  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high */
typedef signed char s8; typedef unsigned char u8; typedef short s16; typedef unsigned short u16; typedef int s32; typedef unsigned int u32;

struct Obj_800231B4 {
    u8 pad_000[0x18C];
    u32 unk18C;
    u8 pad_190[0x4];
    s16 unk194;
    u8 pad_196[0x6];
    s32 unk19C;
    u8 pad_1A0[0x7];
    s8 unk1A7;
};

extern s16 D_800C3E44;
extern s32 func_8001BF00(void);

s32 func_800231B4(struct Obj_800231B4 *obj) {
    if (obj->unk194 != 0) {
        obj->unk194 = (u16)obj->unk194 - 1;
        if (obj->unk194 != -1) {
            return 0;
        }
    }
    obj->unk194 = 20;
    if (obj->unk19C == D_800C3E44) {
        obj->unk194 = 60;
        if (obj->unk1A7 < 6 && func_8001BF00() != 10) {
            obj->unk18C |= 0x40;
            return 0x40;
        }
        obj->unk18C |= 0x10;
        return 0x10;
    }
    if (obj->unk19C < D_800C3E44) {
        return 1;
    }
    return 2;
}
