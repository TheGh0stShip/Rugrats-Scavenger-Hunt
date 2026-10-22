typedef unsigned char u8;

void *func_8000CD08(u8 *object, int index) {
    return *(u8 **)(object + 0x18) + 0x20 + index * 12;
}
