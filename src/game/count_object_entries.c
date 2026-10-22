typedef signed short s16;
typedef signed int s32;

s16 func_80026010(unsigned char *object) {
    int index;
    int count = 0;

    for (index = 0; index < 3; index++) {
        if (*(s32 *)(object + 0x1C8 + index * 4) != 0) {
            count++;
        }
    }
    return count;
}
