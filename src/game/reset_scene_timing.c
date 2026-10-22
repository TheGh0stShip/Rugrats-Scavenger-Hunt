extern int D_80059D40;
extern int D_800B9330;
extern int D_800B9334;

void func_80030200(void) {
    D_800B9330 = 120;
    if (D_80059D40 == 0) {
        D_800B9334 = 40;
    } else {
        D_800B9334 = 120;
    }
    __asm__ volatile ("");
}
