typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;
typedef float f32;

struct Actor_80020E8C {
    u8 pad_00[0xEC];
    f32 unkEC;
    f32 unkF0;
    s32 unkF4;
    u8 pad_F8[0x20];
    u32 flags;
};

extern s32 D_80052F18;
extern s32 D_80057D40;
extern struct Actor_80020E8C *func_800206A8(void);
extern void func_80020FB0(struct Actor_80020E8C *);

void func_80020E8C(struct Actor_80020E8C *a) {
    s32 full = 0;

    if (D_80052F18 != 2) {
        if (a->unkF4 == D_80057D40) {
            full = 1;
        }
        if (a->flags & 0x800000) {
            full = 1;
        }
        if (a == func_800206A8() && (a->flags & 2)) {
            full = 1;
        }
    }
    if (full) {
        a->unkEC = a->unkF0 = 1.0f;
    } else {
        func_80020FB0(a);
    }
    if (a->unkEC < a->unkF0) {
        a->unkEC += 0.1;
        if (a->unkF0 < a->unkEC) {
            a->unkEC = a->unkF0;
        }
    } else if (a->unkF0 < a->unkEC) {
        a->unkEC -= 0.2;
        if (a->unkEC < a->unkF0) {
            a->unkEC = a->unkF0;
        }
    }
}
