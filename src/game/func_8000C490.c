/* Target: func_8000C490  VRAM 0x8000C490  ROM 0xD090
 * Size: 0xA0  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;
typedef double f64;

struct Entry_8000C490 {
    u8 pad_00[0x28];
    u16 unk28;
    u8 pad_2A[2];
};

struct Anim_8000C490 {
    u8 pad_00[2];
    u16 count;
    u8 pad_04[0x16];
    s16 cur;
    u8 pad_1C[0xC];
    /* entries of 0xC bytes, field at +0x28 relative to this struct */
};

struct Hdr_8000C490 {
    u8 pad_00[4];
    s32 offset;
    u8 pad_08[0x10];
    u8 *unk18;
};

struct Owner_8000C490 {
    u8 *data; /* points to Hdr + 0x18 */
    u8 pad_04[4];
    s16 index;
};

extern void func_80013570(struct Hdr_8000C490 *hdr, u16 id);

u8 *func_8000C490(struct Owner_8000C490 *owner) {
    u8 *anim;
    struct Hdr_8000C490 *hdr;

    anim = *(u8 **)(owner->data + 0x18);
    if (anim == 0) {
        return 0;
    }
    if (*(u16 *)(anim + 2) == 0) {
        return 0;
    }
    hdr = (struct Hdr_8000C490 *)(owner->data - 0x18);
    if (*(u16 *)(anim + owner->index * 12 + 0x28) != *(s16 *)(anim + 0x1A)) {
        func_80013570(hdr, *(u16 *)(anim + owner->index * 12 + 0x28));
        *(s16 *)(anim + 0x1A) = *(u16 *)(anim + owner->index * 12 + 0x28);
    }
    return (u8 *)hdr + hdr->offset;
}
