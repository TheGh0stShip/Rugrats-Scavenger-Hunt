typedef unsigned char u8;
typedef unsigned int u32;

extern u32 func_80003A90(u32 address);
extern void func_80003A58(u32 address, u32 value);

void func_80003DC0(u32 address, u8 value) {
    u32 shift = (~address & 3) * 8;

    address &= ~3U;
    func_80003A58(address, (func_80003A90(address) & ~(0xFFU << shift)) | ((u32)value << shift));
}
