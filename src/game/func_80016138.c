typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Tbl_80052F18 {
    s32 unk0;
};
struct Obj_80016138 {
    u8 pad_00[0xFC];
    s32 unkFC;
    u8 pad_100[0x18];
    s32 unk118;
    u8 pad_11C[0x34];
    s16 unk150;
};
extern s32 D_80057D94;
extern s8 D_800581E9;
extern struct Tbl_80052F18 D_80052F18;
extern char D_800012B8[];
extern int func_80005568(void *);
extern void func_80007B00(void *);
extern void func_80007BD0(void *);
extern void func_80007D00(void *);
extern void func_8000E32C(void *);
extern s32 func_8001BF00(void);
extern s32 func_8000E108(void);
extern s32 func_80012CE8(void);
extern void func_80011CC4(s32);
extern void func_800137E8(s32);
extern void func_800138FC(char *, s32, s32);

void func_80016138(struct Obj_80016138 *arg0) {
    s32 flag = arg0->unk118 & 0x100000;

    if (D_80057D94 == 1 || arg0->unk150 != D_800581E9) {
        return;
    }
    if (func_80005568(arg0) != 0 || arg0->unkFC == 8 || (arg0->unk118 & 0x400000)) {
        func_80007B00(arg0);
        if (flag == 0) {
            func_80007BD0(arg0);
        }
    } else if (flag == 0) {
        func_80007D00(arg0);
    }
    if (flag == 0 && D_80052F18.unk0 == 1) {
        func_8000E32C(arg0);
    }
    if (func_8001BF00() == 0x31) {
        return;
    }
    if (D_80052F18.unk0 != 2 && arg0->unk150 == func_8000E108()) {
        if (arg0->unk118 & 0x200000) {
            return;
        }
        if (func_80012CE8() == 0 && (arg0->unk118 & 0x100000)) {
            func_80011CC4(3);
        }
    }
    if (arg0->unk118 & 0x1000000) {
        func_800137E8(8);
        func_800138FC(D_800012B8, 0x100, 0x78);
    }
}

