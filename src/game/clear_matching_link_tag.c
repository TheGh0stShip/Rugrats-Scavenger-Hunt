typedef unsigned char u8;
typedef unsigned int u32;

u32 func_80025E04(u8 *record, u32 id) {
    int index = 0;
    u32 result;

    id |= 0x80000000;
    do {
        u32 value = *(u32 *)(record + 0x1AC);

        if (value == id) {
            result = value & 0x7FFFFFFF;
            *(u32 *)(record + 0x1AC) = result;
            goto done;
        }
        index++;
        record += 4;
    } while (index < 4);
    result = 0;

done:
    return result;
}
