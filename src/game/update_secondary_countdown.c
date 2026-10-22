typedef signed int s32;

extern s32 D_800B9340;
extern s32 D_80059D40;
extern s32 D_800545E4;

void func_8003121C(void) {
    if (D_800B9340 != 0) {
        D_800B9340--;
    }
    if (D_800B9340 == 0) {
        D_80059D40 = 1;
        D_800545E4 = 0;
    }
}
