typedef unsigned char u8;
typedef signed char s8;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

struct Actor_80020DB8 {
    u8 pad_00[0xEC];
    s32 unkEC;
    u8 pad_F0[0x118 - 0xF0];
    u32 flags;
    u8 pad_11C[0x150 - 0x11C];
    s16 unk150;
};

extern s32 D_80057D94;
extern s8 D_800581E9;
extern s32 D_80052F18;
extern s32 D_8006FB78;
extern void func_800210E4(s32);
extern void func_80020E8C(struct Actor_80020DB8 *);
extern void func_80007B24(void *, s32);
extern void func_80007BD0(struct Actor_80020DB8 *);
extern void func_8000E32C(struct Actor_80020DB8 *);

void func_80020DB8(struct Actor_80020DB8 *a)
{
    register struct Actor_80020DB8 *obj __asm__("$16") = a;

    if (D_80057D94 == 1) {
        return;
    }
    if (a->flags & 2) {
        func_800210E4(D_8006FB78);
    }
    if (obj->unk150 == D_800581E9) {
        func_80020E8C(obj);
        if (!(obj->flags & 0x100000)) {
            func_80007B24(obj, obj->unkEC);
            func_80007BD0(obj);
            if (D_80052F18 == 1) {
                func_8000E32C(obj);
            }
        }
    }
}
