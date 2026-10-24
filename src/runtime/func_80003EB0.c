typedef unsigned char u8;
typedef unsigned int u32;

extern void func_80003A58(u32 addr, u32 val);
extern void func_80003DC0(u32 addr, u8 val);

void func_80003EB0(u32 dst, u8 *src, u32 len) {
    while (len != 0 && (dst & 3)) {
        func_80003DC0(dst++, *src++);
        len--;
    }
    while (len >= 4) {
        func_80003A58(dst, (src[0] << 24) | (src[1] << 16) |
                               (src[2] << 8) | src[3]);
        dst += 4;
        src += 4;
        len -= 4;
    }
    while (len != 0) {
        func_80003DC0(dst++, *src++);
        len--;
    }
}
