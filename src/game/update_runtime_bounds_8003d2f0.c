typedef unsigned short u16;

void func_8003D2F0(unsigned char *object)
{
    if (*(u16 *)(object + 0x9A) != 0x7FFF) {
        if (*(u16 *)(object + 0xB2) != 0) {
            *(int *)(object + 0x54) = *(int *)(object + 0x40) + (*(u16 *)(object + 0xB2) << 8);
        } else {
            *(int *)(object + 0x54) = *(int *)(object + 0x3C) - (*(u16 *)(object + 0xB4) << 8);
        }
    } else {
        *(int *)(object + 0x54) = *(int *)(object + 0x40) + 0x7FFFFFFF;
    }
    object[0xC3] = 1;
    object[0xC4] = object[0xC0];
    object[0xC5] = object[0xBF];
}
