/* func_80004A18: send data to the host in chunks of at most 0x8000 bytes. The empty volatile asm keeps the first loop branch delay slot empty. */
typedef unsigned char u8;
typedef int s32;
typedef unsigned int u32;

extern s32 func_800039F0(void);
extern void func_80003A28(s32 mask);
extern void func_80003EB0(u32 dst, u8 *src, u32 len);
extern u32 func_800046C8(u32 cmd, u32 arg0, u32 arg1, u32 arg2);

u32 func_80004A18(u32 handle, u8 *src, u32 len) {
    s32 mask = func_800039F0();
    u32 total = 0;
    u32 n;
    u32 got;

    while (len != 0) {
        n = len;
        if (n > 0x8000) {
            n = 0x8000;
        }
        func_80003EB0(0xB1FF0000, src, n);
        got = func_800046C8(0x606, handle, n, 0);
        if (got == 0) {
            func_80003A28(mask);
            return total;
        }
        __asm__ __volatile__("");
        src += got;
        len -= got;
        total += got;
    }
    func_80003A28(mask);
    return total;
}
