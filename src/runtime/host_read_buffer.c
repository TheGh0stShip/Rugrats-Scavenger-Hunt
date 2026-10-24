typedef unsigned char u8;
typedef int s32;
typedef unsigned int u32;

extern s32 func_800039F0(void);
extern void func_80003A28(s32 mask);
extern void func_80003B94(u32 src, u8 *dst, u32 len);
extern u32 func_800046C8(u32 cmd, u32 arg0, u32 arg1, u32 arg2);

u32 func_80004954(u32 handle, u8 *dst, u32 len) {
    s32 mask;
    u32 total;
    u32 n;
    u32 got;

    mask = func_800039F0();
    total = 0;
    while (len != 0) {
        n = len;
        if (n > 0x8000) {
            n = 0x8000;
        }
        got = func_800046C8(0x505, handle, n, 0);
        if (got == 0) {
            break;
        }
        func_80003B94(0xB1FF0000, dst, got);
        dst += got;
        len -= got;
        total += got;
    }
    func_80003A28(mask);
    return total;
}
