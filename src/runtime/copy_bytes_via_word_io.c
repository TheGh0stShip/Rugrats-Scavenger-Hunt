typedef unsigned char u8;
typedef unsigned int u32;

extern u8 func_80003B14(u8 *address);
extern void func_80003DC0(u8 *address, u8 value);

void func_80004068(u8 *dst, u8 *src, int count) {
    while (count-- != 0) {
        func_80003DC0(dst++, func_80003B14(src++));
    }
}
