typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef float f32;

typedef struct {
    u8 pad_00[8];
    s32 unk8;
} Node_8001E178;

typedef struct {
    u8 pad_00[0x18];
    Node_8001E178 *unk18;
} Obj_8001E178;

typedef struct {
    u8 pad_00[0x2C];
    Obj_8001E178 *unk2C;
} Res_8001E178;

typedef struct {
    s32 unk0;
    Obj_8001E178 **unk4;
} Pair_8005B830;

typedef struct {
    s32 unk0;
    void *unk4;
    s32 count;
    Pair_8005B830 entries[1];
} List_8005B830;

extern List_8005B830 D_8005B830;
extern Res_8001E178 *func_800197C8(s32);
extern void func_8000CD50(Obj_8001E178 *, s32, s32);

void func_8001E178(s32 arg0) {
    Pair_8005B830 *e;
    Res_8001E178 *r;
    Node_8001E178 *n;

    e = &D_8005B830.entries[D_8005B830.count];
    e->unk0 = arg0;
    r = func_800197C8(arg0);
    e->unk4 = &r->unk2C;
    func_8000CD50(r->unk2C, 0, 1);
    n = (*e->unk4)->unk18;
    if (n != 0) {
        n->unk8 |= 8;
    }
    D_8005B830.count++;
}

