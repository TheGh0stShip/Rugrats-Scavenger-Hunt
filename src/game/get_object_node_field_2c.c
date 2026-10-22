typedef unsigned char u8;
extern u8 *func_800197C8(void *handle);
void *func_8002A9A4(u8 *object, int alternate) {
    void *handle = object + (alternate > 0 ? 0x32 : 0x2F);
    return func_800197C8(handle) + 0x2C;
}
