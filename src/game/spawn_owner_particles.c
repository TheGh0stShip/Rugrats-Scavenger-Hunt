/* func_8000E1D0: spawn five particles for an owner slot. */
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef float f32;
typedef double f64;

typedef struct Part_80058208 {
    s32 x;      /* 0x00 */
    s32 y;      /* 0x04 */
    s32 z;      /* 0x08 */
    s32 unk0C;
    s32 unk10;
    u8 pad_14[0x4];
    f32 unk18;
} Part_80058208; /* size 0x1C, 5 per owner */

typedef struct Src_8000E1D0 {
    u8 pad_00[0x2C];
    f32 x;      /* 0x2C */
    f32 y;      /* 0x30 */
    f32 z;      /* 0x34 */
    u8 pad_38[0xF4 - 0x38];
    s32 slot;   /* 0xF4 */
} Src_8000E1D0;

extern void *D_80058200;
extern Part_80058208 D_80058208[][5];
extern u8 *func_800197C8(s32 id);
extern s32 func_8003A840(void);

void func_8000E1D0(Src_8000E1D0 *src) {
    s32 slot = src->slot;
    s32 i;

    D_80058200 = func_800197C8(0xE1) + 0x2C;
    if (src == 0) {
        return;
    }
    for (i = 0; i < 5; i++) {
        Part_80058208 *p = (Part_80058208 *)(slot * 0x8C + i * 0x1C + (u32)D_80058208);

        p->x = src->x;
        p->y = src->y + 300.0f;
        p->z = src->z;
        p->unk0C = (func_8003A840() % 7) * 4;
        if (p->unk0C < 2) {
            p->unk0C = 2;
        }
        p->unk10 = 0;
        p->unk18 = func_8003A840() / 65535.0f;
        if (p->unk18 < 0.75) {
            p->unk18 += 0.75;
        }
    }
}
