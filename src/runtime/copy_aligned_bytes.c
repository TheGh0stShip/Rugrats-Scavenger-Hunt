typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

void *func_80038D90(void *destination, const void *source, u32 length)
{
    u8 *destination_bytes;
    const u8 *source_bytes;
    u32 words;

    if (length != 0) {
        destination_bytes = (u8 *)destination;
        source_bytes = (const u8 *)source;
        if (((u32)source_bytes - (u32)destination_bytes) & 3) {
            while (length--) {
                *destination_bytes++ = *source_bytes++;
            }
        } else {
            if ((u32)destination_bytes & 1) {
                *destination_bytes++ = *source_bytes++;
                length--;
            }
            if (length >= 2 && ((u32)destination_bytes & 2)) {
                *(u16 *)destination_bytes = *(const u16 *)source_bytes;
                source_bytes += 2;
                destination_bytes += 2;
                length -= 2;
            }
            words = length >> 2;
            while (words--) {
                *(u32 *)destination_bytes = *(const u32 *)source_bytes;
                source_bytes += 4;
                destination_bytes += 4;
            }
            if (length & 2) {
                *(u16 *)destination_bytes = *(const u16 *)source_bytes;
                source_bytes += 2;
                destination_bytes += 2;
            }
            if (length & 1) {
                *destination_bytes = *source_bytes;
            }
        }
    }
    return destination;
}
