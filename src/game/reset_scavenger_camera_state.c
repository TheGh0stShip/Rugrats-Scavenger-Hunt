typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;
typedef struct { u8 pad[0x2C]; f32 x; f32 y; f32 z; } Obj;
extern s32 D_80057D94, D_80057D98, D_80057D50, D_80057D54;
extern Obj *func_80005544(void);
void func_8000FD38(u8 *a0) {
    Obj *o;
    *(s32 *)(a0 + 0xF0) = 4;
    D_80057D94 = 1;
    o = func_80005544();
    D_80057D98 = 0;
    D_80057D50 = 0;
    *(f32 *)(a0 + 0x114) = o->x;
    D_80057D54 = 0;
    *(f32 *)(a0 + 0x11C) = o->z;
}
