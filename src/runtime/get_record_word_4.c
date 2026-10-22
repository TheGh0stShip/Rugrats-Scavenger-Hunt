typedef unsigned int u32;

extern u32 *D_80056070;

u32 func_8004FA00(u32 *record) {
    if (record == 0) {
        record = D_80056070;
    }
    return record[1];
}
