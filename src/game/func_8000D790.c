/* Target: func_8000D790  VRAM 0x8000D790  ROM 0xE390
 * Size: 0xA8  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high */
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef float f32;

typedef struct Obj_8000D790 {
    u8 pad_00[0x8];
    s16 unk08;
    s16 unk0A;
    s32 unk0C;
    void *unk10;
    u8 pad_14[0x4];
    void *unk18;
    u8 pad_1C[0x4];
    s32 unk20;
    s32 unk24;
    s32 unk28;
    u8 pad_2C[0x4];
    s32 unk30;
    void *unk34;
    u8 unk38;
    u8 pad_39[0x3];
} Obj_8000D790;

extern void *func_8001A2B0(s32 size);
extern void *func_80038E90(void *p, s32 val, s32 size);
extern void func_80007EFC(void *, void *, int, void *);
extern void *func_8000D050(s32 id);
extern void func_8000D838(void *);
extern void func_8000D910();
extern void *D_80059DA8;

Obj_8000D790 *func_8000D790(s32 id) {
    Obj_8000D790 *obj = func_8001A2B0(sizeof(Obj_8000D790));

    func_80038E90(obj, 0, sizeof(Obj_8000D790));
    obj->unk38 = 0;
    obj->unk10 = func_8000D838;
    obj->unk18 = func_8000D910;
    obj->unk20 = 0;
    obj->unk24 = 0;
    obj->unk08 = 0;
    obj->unk0A = 0;
    obj->unk0C = 0;
    obj->unk28 = 0;
    func_80007EFC(obj, D_80059DA8, 4, 0);
    obj->unk30 = id;
    obj->unk34 = func_8000D050(id);
    return obj;
}
