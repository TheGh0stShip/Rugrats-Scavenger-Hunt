typedef unsigned short u16;

unsigned char *func_8003E974(unsigned char *object, unsigned char *source)
{
    unsigned int index = object[0xDB];
    unsigned char value = *source++;
    unsigned char *byte_slot = object + index;

    byte_slot[0x120] = value;
    *(unsigned char **)(object + 0xE0 + index * 4) = source;
    *(unsigned int *)(object + 0xF0 + index * 4) = *(unsigned int *)(object + 0x38);
    *(unsigned int *)(object + 0x100 + index * 4) = *(unsigned int *)(object + 0x34);
    byte_slot[0x124] = object[0xBC];
    *(float *)(object + 0x128 + index * 4) = *(float *)(object + 0x70);
    *(u16 *)(object + 0x110 + index * 2) = *(u16 *)(object + 0xA2);
    *(u16 *)(object + 0x118 + index * 2) = *(u16 *)(object + 0xA4);
    object[0xDB]++;
    return source;
}
