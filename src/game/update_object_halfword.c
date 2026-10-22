typedef signed short s16;
typedef unsigned short u16;

void func_80005528(void *object, s16 value) {
    s16 *oldValue = (s16 *)((char *)object + 0x150);
    s16 *currentValue = (s16 *)((char *)object + 0x152);
    u16 previous = *(u16 *)oldValue;

    if (*oldValue != *currentValue) {
        *currentValue = previous;
    }
    *oldValue = value;
}
