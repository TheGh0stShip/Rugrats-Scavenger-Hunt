extern int D_800545E8;
extern int D_800B9330;
extern int D_800B9334;

int func_80030238(void) {
    int r;
    if (D_800545E8 != 0) {
        r = 3;
    } else if (D_800B9330 != 0) {
        if (--D_800B9330 == 0) {
            r = 2;
        } else {
            r = D_800B9330 < D_800B9334;
            if (r) r = 1;
        }
    } else {
        r = 3;
    }
    __asm__("" : : "r"(r));
    return r;
}
