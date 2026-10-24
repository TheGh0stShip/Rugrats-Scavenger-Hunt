typedef unsigned char u8;
typedef unsigned int u32;

extern void func_80003A58(u32 addr, u32 val);
extern void func_80003DC0(u32 addr, u8 val);

void func_80003F9C(u32 dst, u8 val, u32 len) {
    u32 b;
    u32 w;

    while (len != 0 && (dst & 3)) {
        func_80003DC0(dst++, val);
        len--;
    }
    if (len >= 4) {
        b = val & 0xFF;
        w = (b << 24) | (b << 16) | (b << 8) | b;
        do {
            func_80003A58(dst, w);
            len -= 4;
            dst += 4;
        } while (len >= 4);
    }
    while (len != 0) {
        func_80003DC0(dst++, val);
        len--;
    }
}
