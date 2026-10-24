typedef unsigned char u8;
typedef unsigned short u16;
typedef int s32;

typedef struct Obj_8003BD20 {
    u8 pad_00[0x10];
    s32 unk10;
    u8 pad_14[0x4];
    struct {
        s32 a;
        s32 b;
    } ent[1];
} Obj_8003BD20;

typedef struct Ent_8003BD20 {
    u8 pad_00[0x78];
    Obj_8003BD20 *owner;
    u8 pad_7C[0x2A];
    u16 id;
    u8 pad_A8[0x94];
} Ent_8003BD20;

extern Obj_8003BD20 *D_800C4008;
extern Obj_8003BD20 *D_800C400C;
extern s32 D_800C3FF8;
extern s32 D_800C3FD4;
extern Ent_8003BD20 *D_800C3FE0;
extern s32 func_8003DFB8(Ent_8003BD20 *e, Obj_8003BD20 *obj, s32 id,
                        s32 a, s32 b, s32 c);
extern s32 func_8003E068(Obj_8003BD20 *obj, s32 id, s32 a, s32 b, s32 c);

s32 func_8003BD20(s32 id, s32 a, s32 b, s32 search, s32 c) {
    Obj_8003BD20 *obj;
    Ent_8003BD20 *e;
    s32 i;
    s32 ret;

    if (D_800C400C != 0) {
        obj = D_800C400C;
        D_800C400C = 0;
    } else {
        obj = D_800C4008;
        if (obj == 0) {
            D_800C3FF8 = 0;
            return 0;
        }
    }
    if (D_800C3FF8 == 0) {
        D_800C3FF8 = obj->unk10;
    }
    if (search != 0) {
        e = D_800C3FE0;
        for (i = 4; i < D_800C3FD4; i++, e++) {
            if (e->id == id && e->owner == obj) {
                if (c == -1) {
                    c = obj->ent[id].b;
                }
                ret = func_8003DFB8(e, obj, id, a, b, c);
                D_800C3FF8 = 0;
                return ret;
            }
        }
    }
    ret = func_8003E068(obj, id, a, b, c);
done:
    D_800C3FF8 = 0;
    return ret;
}
