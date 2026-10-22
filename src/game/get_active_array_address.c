typedef unsigned char u8;

extern u8 *D_80053634;

void *func_8001BF60(int index) {
    return D_80053634 + 0x30 + index * 4;
}
