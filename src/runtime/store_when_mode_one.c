typedef unsigned int u32;

int func_8004C070(u32 *destination, int mode, u32 value) {
    if (mode == 1) {
        *destination = value;
    }
    return 0;
}
