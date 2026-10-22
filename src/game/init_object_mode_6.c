typedef unsigned char u8;

extern u8 D_80055E08[];

void func_80042550(u8 *object) {
    object[0x1C] = 6;
    *(u8 **)(object + 0x20) = D_80055E08;
}
