typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef float f32;

typedef struct Key_8000F2B8 {
    u16 a, b, c;
    u16 pad;
} Key_8000F2B8;

typedef struct Out_8000F2B8 {
    u16 a, b, c;
} Out_8000F2B8;

typedef struct Anim_8000F2B8 {
    u32 count0;
    u32 count1;
    Key_8000F2B8 *keys0;
    Key_8000F2B8 *keys1;
} Anim_8000F2B8;

void func_8000F2B8(Anim_8000F2B8 *anim, u32 frame, Out_8000F2B8 *out0, Out_8000F2B8 *out1) {
    u32 i;

    if (frame < anim->count1) {
        i = frame;
    } else {
        i = anim->count1 - 1;
    }
    out1->a = anim->keys1[i].a;
    out1->b = anim->keys1[i].b;
    out1->c = anim->keys1[i].c;

    i = frame;
    if (i >= anim->count0) {
        i = anim->count0 - 1;
    }
    out0->a = anim->keys0[i].a;
    out0->b = anim->keys0[i].b;
    out0->c = anim->keys0[i].c;
}
