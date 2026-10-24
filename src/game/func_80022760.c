typedef unsigned char u8;
typedef short s16;
typedef int s32;

struct Obj_80022760 {
    u8 pad_000[0x144];
    s32 unk144;
    u8 pad_148[0x8];
    s16 unk150;
};

extern struct Obj_80022760 *func_800206D0(void);

s32 func_80022760(struct Obj_80022760 *obj)
{
    if (obj->unk150 != 0) {
        return 0;
    }
    if (obj->unk144 != obj->unk144 / 3 * 3) {
        return 0;
    }
    return obj->unk144 / 3 == func_800206D0()->unk144;
}
