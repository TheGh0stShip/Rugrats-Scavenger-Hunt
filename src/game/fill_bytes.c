typedef unsigned char u8;
typedef unsigned int u32;

void func_80043100(u8 *destination, u8 value, u32 count) {
    while (count != 0) {
        *destination++ = value;
        count--;
    }
}
