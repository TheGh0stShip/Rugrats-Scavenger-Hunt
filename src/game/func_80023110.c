/* Target: func_80023110  VRAM 0x80023110  ROM 0x23D10
 * Size: 0xA4  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: medium */
typedef signed char s8; typedef unsigned char u8; typedef short s16; typedef unsigned short u16; typedef int s32; typedef unsigned int u32;

struct Obj_80023110 {
    u8 pad_000[0x194];
    s16 unk194;
    u8 pad_196[0x2];
    s32 unk198;
};

extern s32 func_8001F3A8(struct Obj_80023110 *);
extern s32 func_8001BF3C(void);

s32 func_80023110(struct Obj_80023110 *obj, s32 arg1) {
    if (func_8001F3A8(obj) != 0) {
        return 0;
    }
    if (obj->unk194 != 0) {
        if (--obj->unk194 != -1) {
            return 0;
        }
    }
    obj->unk194 = 20;
    if (func_8001BF3C() == 20 || obj->unk198 == arg1) {
        obj->unk194 = 60;
        return 0x10;
    }
    if (obj->unk198 < arg1) {
        return 1;
    }
    return 2;
}
