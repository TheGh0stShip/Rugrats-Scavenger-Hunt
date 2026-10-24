typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Entry_80058C38 {
    s32 offset;
    s32 size;
    s32 packedSize;
};

struct Table_80058C38 {
    u32 count;
    s32 unk4;
    struct Entry_80058C38 *entries;
};

extern struct Table_80058C38 D_80058C38;
extern s32 D_80052EB0;
extern void *D_80052EC8;

extern void func_80028B58(s32, void *, s32);
extern void func_800166F0(void *, void *, s32 *);

s32 func_8001313C(u32 index, void *dst) {
    struct Entry_80058C38 *e;
    s32 src;
    s32 size;
    s32 packed;
    s32 outSize;

    if (index >= D_80058C38.count || dst == 0) {
        return 0;
    }
    e = (struct Entry_80058C38 *)(index * sizeof(struct Entry_80058C38) + (u32)D_80058C38.entries);
    {
        s32 off = e->offset;
        s32 base = D_80052EB0;
        src = base + off;
    }
    packed = e->packedSize;
    size = e->size;
    if (packed == 0) {
        func_80028B58(src, dst, size);
    } else {
        func_80028B58(src, D_80052EC8, packed);
        func_800166F0(D_80052EC8, dst, &outSize);
    }
    return 1;
}
