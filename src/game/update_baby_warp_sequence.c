/* func_8002A1B4  VRAM 0x8002A1B4  ROM 0x2ADB4  size 0x3FC
 * SN GCC 2.8 -O2 -G0 -mips3; assembled after the reviewed SN scheduling normalization. */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;

typedef struct {
    s32 x, y, z;
} Vec3_8002A1B4;

struct Actor_8002A1B4 {
    u8 pad_000[0xE0];
    s32 unkE0;
    u8 pad_0E4[0xC];
    s32 unkF0;
    u8 pad_0F4[0x4];
    s32 unkF8;
    Vec3_8002A1B4 unkFC;
    Vec3_8002A1B4 unk108;
    u8 pad_114[0x4];
    u32 flags;
    s32 unk11C;
};

struct Obj_800197C8 {
    u8 pad_00[0x2C];
    s32 unk2C;
};

extern s32 D_80054420;
extern s32 D_800B92A0;
extern struct Actor_8002A1B4 *D_80052F04;
extern struct Actor_8002A1B4 **func_8002A628(s32);
extern struct Obj_800197C8 *func_800197C8(s32);
extern void func_8000C530(void *, s32, s32);
extern int func_8000CE08(void *, int);
extern void func_8000CD50(void *, int, int);
extern s32 func_8000CCC8(s32, s32);
extern int func_8001F138(s32);
extern void func_8000FDAC(s32, struct Actor_8002A1B4 *);
extern void func_8002A13C(void);
extern void func_80006534(struct Actor_8002A1B4 *, s32, s32);
extern void func_8000DDAC(s32);
extern void func_80010E24(s32, s32);
extern void func_80005500(s32, struct Actor_8002A1B4 *);
extern void func_8002A5B0(s32);

void func_8002A1B4(struct Actor_8002A1B4 *obj) {
    struct Actor_8002A1B4 *pl = *func_8002A628(0);
    struct Actor_8002A1B4 *cam;
    s32 n;
    Vec3_8002A1B4 unused;

    switch (D_80054420) {
        case 0:
            func_8000C530(&func_800197C8(0x1E4)->unk2C, 1, 1);
            func_8000C530(&func_800197C8(0x1E4)->unk2C, 0, 2);
            func_8000C530(&func_800197C8(obj->unkE0)->unk2C, 0x20, 1);
            func_8000C530(&func_800197C8(obj->unkE0)->unk2C, 0, 2);
            D_800B92A0 = func_8000CE08(func_800197C8(0x1E4)->unk2C, 1);
            func_8001F138(0x4B);
            D_80054420 = 1;
            break;
        case 1:
            if (--D_800B92A0 == 0) {
                pl->flags |= 0x400000;
                D_80052F04->unk108 = D_80052F04->unkFC;
                func_8000FDAC(6, pl);
                func_8000C530(&func_800197C8(pl->unkE0)->unk2C, 10, 1);
                func_8000C530(&func_800197C8(pl->unkE0)->unk2C, 0, 2);
                D_800B92A0 = func_8000CE08(func_800197C8(obj->unkE0)->unk2C, 10);
                D_80054420 = 2;
            }
            break;
        case 2:
            if (--D_800B92A0 == 0) {
                D_80054420 = 3;
                func_8000C530(&func_800197C8(pl->unkE0)->unk2C, 0x25, 1);
                func_8000CD50(func_800197C8(pl->unkE0)->unk2C, 0x25, 1);
                D_800B92A0 = func_8000CE08(func_800197C8(pl->unkE0)->unk2C, 0x25) + 20;
                func_8002A13C();
                func_8001F138(0x49);
            }
            break;
        case 3:
            if (func_8000CCC8(func_800197C8(pl->unkE0)->unk2C, 1) != 0) {
                pl->flags |= 0x100000;
            }
            if (D_800B92A0 != 0) {
                D_800B92A0--;
                break;
            }
            n = pl->unkF8 + 1;
            func_80006534(pl, n, 0);
            func_8000DDAC(n & 0xFF);
            func_8000FDAC(6, pl);
            D_80054420 = 5;
            break;
        case 5:
            func_8000C530(&func_800197C8(pl->unkE0)->unk2C, 0x26, 1);
            func_8000C530(&func_800197C8(pl->unkE0)->unk2C, 0, 2);
            D_800B92A0 = func_8000CE08(func_800197C8(pl->unkE0)->unk2C, 0x26) + 20;
            D_80054420 = 6;
            pl->flags &= ~0x100000;
            break;
        case 6:
            if (D_800B92A0 == 25) {
                func_8001F138(0x4A);
            }
            if ((u32)pl->unk11C < 4) {
                func_80010E24(pl->unk11C, 8);
            }
            if (D_800B92A0 != 0) {
                D_800B92A0--;
                break;
            }
            D_80054420 = 7;
            break;
        case 7:
            func_8000DDAC(0);
            D_80052F04->unkFC = D_80052F04->unk108;
            D_80052F04->unkF0 = 0;
            pl->flags &= ~0x400000;
            func_80005500(6, obj);
            func_8002A5B0(0);
            D_80054420 = 0;
            break;
    }
}
