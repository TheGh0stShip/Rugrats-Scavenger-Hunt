typedef unsigned char u8;

void func_8000798C(void *object, int x, int y) {
    u8 *bytes = (u8 *)object;
    u8 oldX = bytes[0x13C];
    u8 oldY = bytes[0x13D];

    bytes[0x140] = oldX;
    bytes[0x141] = oldY;
    bytes[0x13C] = oldX + x;
    bytes[0x13D] = oldY + y;
}
