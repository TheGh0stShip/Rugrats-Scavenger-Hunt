typedef unsigned int u32;

void func_80008290(u32 *dst, u32 *src) {
    int i;

    for (i = 0; i < 16; i++) {
        *dst++ = *src++;
    }
}
