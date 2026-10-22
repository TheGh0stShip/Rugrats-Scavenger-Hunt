typedef unsigned int u32;

int func_8004C9A0(u32 *record, int mode, u32 value) {
    switch (mode) {
        case 1:
            record[0] = value;
            break;
        case 6:
            record[5] = value;
            break;
    }
    return 0;
}
