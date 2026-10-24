typedef unsigned char u8;
typedef unsigned int u32;
typedef signed short s16;

struct Obj_8002E0E0 {
    u8 pad_000[0x18C];
    u32 flags;
    u8 pad_190[0x4];
    s16 timer;
    u8 pad_196[0x1E8 - 0x196];
    s16 f1E8[4];
    s16 f1F0[4];
    s16 f1F8;
};

extern u8 func_8002FED0(void);
extern u8 func_8002FEEC(void);
extern u32 D_800B92F8;
extern u32 D_800B92F0;

void func_8002E0E0(struct Obj_8002E0E0 *o) {
    if (o->timer > 0) {
        o->timer--;
        return;
    }
    o->timer = 15;
    if (func_8002FED0() != D_800B92F8) {
        o->flags |= 8;
    } else if (func_8002FEEC() != D_800B92F0) {
        o->flags |= 2;
    } else {
        o->flags |= 0x10;
    }
}
