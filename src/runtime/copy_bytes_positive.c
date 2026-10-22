typedef unsigned char u8;

void func_8004CB10(const u8 *source, u8 *destination, int count) {
    int i;

    for (i = 0; i < count; i++) {
        *destination++ = *source++;
    }
}
