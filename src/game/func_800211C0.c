typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

struct Level_800581F0 {
    u8 pad_00[4];
    s16 count;
    u8 pad_06[6];
    s16 unkC;
    s16 unkE;
};

struct Pos_8000DD88 { s16 y; s16 x; };

extern struct Level_800581F0 *D_800581F0;
extern s32 func_8003A840(void);
extern s32 func_8000E078(s32, s32);
extern void func_80020B28(s32 *, s32 *, s32);
extern struct Pos_8000DD88 *func_8000DD88(u8);
extern void func_80006468(void *, s32, s32);

void func_800211C0(void *obj, s32 exclude)
{
    s32 r;
    s32 x, y;
    s32 px;

    do {
        r = func_8003A840() % D_800581F0->count;
    } while (r == func_8000E078(D_800581F0->unkC, D_800581F0->unkE) || r == exclude);
    func_80020B28(&x, &y, r);
    px = func_8000DD88(r)->x + x;
    func_80006468(obj, px, func_8000DD88(r)->y + y);
}

