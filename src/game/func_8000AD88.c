typedef unsigned char u8;
typedef signed int s32;

struct Obj_800055A4 {
    u8 pad_00[0xF8];
    s32 unkF8;
};

extern s32 D_800535E0;
extern s32 D_80057D5C;
extern struct Obj_800055A4 *func_800055A4(s32 idx);

struct Obj_800055A4 *func_8000AD88(void) {
    s32 i;
    s32 target = D_800535E0 - 2;

    for (i = 0; i < D_80057D5C; i++) {
        if (func_800055A4(i)->unkF8 == target) {
            return func_800055A4(i);
        }
    }
}
