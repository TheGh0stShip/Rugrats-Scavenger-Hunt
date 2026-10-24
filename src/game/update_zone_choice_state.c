/* func_80029580  VRAM 0x80029580  ROM 0x2A180  size 0x154
 * SN GCC 2.8 -O2 -G0 -mips3; assembled after the reviewed SN scheduling normalization. */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Obj_80029580 {
    u8 pad_000[0x11C];
    s32 unk11C;
    u8 pad_120[0x74];
    s16 unk194;
};

struct Obj_800B9290 {
    u32 unk0;
    s32 unk4;
    s32 unk8;
};

extern s32 D_80054374;
extern s32 D_80054568;
extern struct Obj_800B9290 D_800B9290;
extern s32 func_8003A840(void);
extern void func_8002E1A8(struct Obj_80029580 *, s32, s32);
extern u32 func_800166B0(struct Obj_80029580 *);
extern s32 func_800296D4(struct Obj_80029580 *, u32);
extern s32 func_800299A8(struct Obj_80029580 *, u32);
extern s32 func_80029BB4(struct Obj_80029580 *, u32);
extern void func_8002A5B0(s32);

void func_80029580(struct Obj_80029580 *obj) {
    s32 done;
    u32 input;

    if (D_80054374 != 0) {
        D_80054374 = 0;
        D_800B9290.unk0 = func_8003A840() % 3;
        D_800B9290.unk4 = func_8003A840() % 4;
        D_800B9290.unk8 = 35;
        obj->unk194 = 40;
        D_80054568 = 0;
    }
    if (obj->unk11C == -1) {
        func_8002E1A8(obj, D_800B9290.unk0, D_800B9290.unk4);
    }
    input = func_800166B0(obj);
    switch (D_800B9290.unk0) {
        case 0:
            done = func_800296D4(obj, input);
            break;
        case 1:
            done = func_800299A8(obj, input);
            break;
        case 2:
            done = func_80029BB4(obj, input);
            break;
    }
    if (done != 0) {
        func_8002A5B0(0);
        D_80054374 = 1;
    }
}
