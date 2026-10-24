typedef signed char s8; typedef unsigned char u8; typedef short s16; typedef unsigned short u16; typedef int s32; typedef unsigned int u32;

struct Unk_80052F04 {
    u8 pad_00[0xF0];
    s32 unkF0;
};

struct Obj_80024C30 {
    u8 pad_000[0x158];
    u32 unk158;
};

extern u32 D_80099B30;
extern s32 D_80052F18;
extern s32 D_8006FBA0;
extern struct Unk_80052F04 *D_80052F04;
extern void func_800253D8(struct Obj_80024C30 *);
extern void func_80006E30(struct Obj_80024C30 *);
extern void func_8000E680(struct Unk_80052F04 *);
extern void func_8002A5B0(s32);
extern void func_80024DAC(struct Obj_80024C30 *);
extern void func_8001BED8(s32);
extern void func_80005500(s32, struct Obj_80024C30 *);

void func_80024C30(struct Obj_80024C30 *obj) {
    if (D_80099B30 < obj->unk158) {
        return;
    }
    if (D_80052F18 == 2) {
        switch (D_8006FBA0) {
            case 0x36:
                func_800253D8(obj);
                return;
            case 0x37:
                func_80006E30(obj);
                D_80052F04->unkF0 = 0;
                func_8000E680(D_80052F04);
                return;
            default:
                func_8002A5B0(D_8006FBA0);
                break;
        }
    } else if (D_8006FBA0 == -1) {
        func_80024DAC(obj);
    } else {
        func_8001BED8(D_8006FBA0);
    }
    func_80005500(0, obj);
}

