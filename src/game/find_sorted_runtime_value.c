typedef signed short s16;
typedef unsigned short u16;

int func_8000B858(int value, u16 *values, int count, u16 *last_index) {
    int i;
    u16 target;

    count = (u16)count;
    i = 0;
    if (count != 0) {
        target = value;
        do {
            if (target == *values) {
                return (s16)i;
            }
            if (target < *values) {
                return -1;
            }
            *last_index = i;
            i++;
            values++;
        } while (i < count);
    }
    return -1;
}
