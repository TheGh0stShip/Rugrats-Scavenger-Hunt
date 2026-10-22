typedef unsigned int u32;

extern u32 D_80057210;
extern u32 D_80057218;

u32 func_8004FEA0(void) {
    if (D_80057210 != 0) {
        return D_80057218;
    }
    return 0;
}
