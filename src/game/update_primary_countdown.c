typedef signed int s32;

extern s32 D_800B9338;
extern s32 D_800B9340;
extern s32 D_800545E4;

void func_800311E8(void) {
    D_800B9338--;
    if (D_800B9338 == 0) {
        D_800B9340 = 1;
        D_800545E4 = 4;
    }
}
