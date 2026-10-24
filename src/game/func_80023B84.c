typedef signed char s8; typedef unsigned char u8; typedef short s16; typedef unsigned short u16; typedef int s32; typedef unsigned int u32;

struct Obj_80023B84 {
    u8 pad_000[0xF4];
    s32 unkF4;
    u8 pad_0F8[0x4];
    s32 unkFC;
};

extern s32 D_80057D5C;
extern s32 D_80057D58;
extern void func_80012D04(s32, s32);
extern void func_80005500(s32, struct Obj_80023B84 *);
extern struct Obj_80023B84 *func_800055A4(s32);
extern void func_8001E738(s32, s32);
extern void func_8001E678(s32, s32);

void func_80023B84(struct Obj_80023B84 *obj) {
    s32 n;
    struct Obj_80023B84 *next;

    func_80012D04(10, 1);
    func_80005500(10, obj);
    n = obj->unkF4 + 1;
    if (D_80057D5C + D_80057D58 - 1 < n) {
        n = 0;
    }
    next = func_800055A4(n);
    func_8001E738(10, 1);
    if (obj->unkF4 == D_80057D5C - 1 || next->unkFC == 14) {
        func_8001E678(10, 1);
    }
}

