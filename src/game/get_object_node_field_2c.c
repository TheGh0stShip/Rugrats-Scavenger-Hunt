typedef unsigned char u8;
extern u8 *func_800197C8(void *handle);
void *func_8002A9A4(u8 *object, int alternate) {
    register u8 *base __asm__("$2") = object;
    register void *handle __asm__("$4") = base + 0x2F;

    if (alternate > 0) {
        handle = base + 0x32;
    }
    return func_800197C8(handle) + 0x2C;
}
