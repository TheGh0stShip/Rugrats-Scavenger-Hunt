typedef signed char s8; typedef unsigned char u8; typedef short s16; typedef unsigned short u16; typedef int s32; typedef unsigned int u32;

struct Obj_800237C0 {
    u8 pad_000[0x118];
    u32 unk118;
};

extern s32 D_80052F18;
extern s32 func_8001EEAC(void);
extern void func_8001ECE4(s32, s32);
extern void func_80005EF8(struct Obj_800237C0 *);

void func_800237C0(struct Obj_800237C0 *obj) {
    if (!(obj->unk118 & 0x400)) {
        if (D_80052F18 == 2) {
            if (func_8001EEAC() != 0x20C) {
                func_8001ECE4(0x20C, 0x30);
            }
        } else if (D_80052F18 == 0) {
            if (func_8001EEAC() != 0x201) {
                func_8001ECE4(0x201, 0x38);
            }
        } else if (func_8001EEAC() != 0x204) {
            func_8001ECE4(0x204, 0x38);
        }
    }
    func_80005EF8(obj);
}

