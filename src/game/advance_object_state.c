typedef signed char s8;
typedef unsigned char u8;

void func_80044238(u8 *object) {
    s8 *state = (s8 *)(object + 0x11DD);

    if (*state == 2) {
        *state = 3;
    }
}
