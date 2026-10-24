/* Target: func_800109EC  VRAM 0x800109EC  ROM 0x115EC
 * Size: 0xFC  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high
 */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;
typedef double f64;

struct Obj_800109EC {
    u8 pad_00[0xF0];
    s32 unk_F0;
    u8 pad_F4[0xFC - 0xF4];
    s32 unk_FC;
    s32 unk_100;
    s32 unk_104;
    u8 pad_108[0x114 - 0x108];
    f32 unk_114[3];
};

extern void * func_800055A4(unsigned int);
extern void *func_80038E90(void *dst, s32 value, s32 size);
extern s32 D_80057D94;
extern s32 D_80052F18;

void func_800109EC(struct Obj_800109EC *obj, s32 n) {
    func_800055A4(n);
    D_80057D94 = 1;
    func_80038E90(obj->unk_114, 0, 0xC);
    if (D_80052F18 == 0) {
        obj->unk_100 = 0x159;
    } else {
        obj->unk_100 = (n * 90 - 15) % 360;
    }
    if (D_80052F18 == 0) {
        obj->unk_104 = 0xC80;
    } else if (obj->unk_F0 == 0x15) {
        obj->unk_104 = 0x1F40;
    } else {
        obj->unk_104 = 0x1068;
    }
    obj->unk_FC = -0x10;
    obj->unk_F0 = 0x14;
}
