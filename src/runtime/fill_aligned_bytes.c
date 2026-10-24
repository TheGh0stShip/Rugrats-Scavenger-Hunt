typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

void *func_80038E90(void *destination, u32 value, u32 length)
{
    u8 *current;
    u32 words;

    if (length != 0) {
        value &= 0xFF;
        value |= value << 8;
        value |= value << 16;
        current = (u8 *)destination;
        if ((u32)current & 1) {
            *current++ = value;
            length--;
        }
        if (length >= 2 && ((u32)current & 2)) {
            *(u16 *)current = value;
            current += 2;
            length -= 2;
        }
        words = length >> 2;
        while (words--) {
            *(u32 *)current = value;
            current += 4;
        }
        if (length & 2) {
            *(u16 *)current = value;
            current += 2;
        }
        if (length & 1) {
            *current = value;
        }
    }
    return destination;
}
