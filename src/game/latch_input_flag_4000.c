typedef unsigned short u16;
typedef signed int s32;

extern u16 D_80058B5A;
extern s32 D_800545E8;

void func_800312EC(void) {
    if (D_80058B5A & 0x4000) {
        D_800545E8 = 1;
    }
}
