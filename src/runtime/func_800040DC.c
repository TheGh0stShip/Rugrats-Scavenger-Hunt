/* Target: func_800040DC  VRAM 0x800040DC  ROM 0x4CDC
 * Size: 0x1B0  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: medium */
typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;

typedef struct Region_800529A0 {
    /* 0x0 */ u32 end;
    /* 0x4 */ u32 flags;
} Region_800529A0;

extern Region_800529A0 D_800529A0[];
extern u8 D_8F057290[];

extern u32 func_80003A90(u32 addr);
extern void func_80003F9C(u32 dst, u8 val, u32 len);
extern void func_80004068(u32 dst, u32 src, s32 len);
extern void func_80003EB0(u32 dst, u8 *src, u32 len);
extern void func_80003CAC(u32 src, u32 dst, u32 len);
extern void func_80003B94(u32 src, u8 *dst, u32 len);

void func_800040DC(s32 mode) {
    u32 buf = 0xB1FE0000;
    u32 addr = func_80003A90(0xB1FFFFF4);
    u32 len = func_80003A90(0xB1FFFFF8);
    u32 n;
    u32 flags;
    s32 i;

    if (addr > 0xEFFFFFFF) {
        addr += (u32)D_8F057290;
    }
    while (len != 0) {
        i = 0;
        while (D_800529A0[i].end < addr) {
            i++;
        }
        n = (D_800529A0[i].end + 1) - addr;
        if (len < n) {
            n = len;
        }
        flags = D_800529A0[i].flags;
        if (!(flags & mode)) {
            if (mode == 1) {
                func_80003F9C(buf, 0, n);
            }
        } else if (flags & 4) {
            if (mode == 1) {
                func_80004068(buf, addr, n);
            } else {
                func_80004068(addr, buf, n);
            }
        } else if (mode == 1) {
            func_80003EB0(buf, (u8 *)addr, n);
        } else if (flags & 8) {
            func_80003CAC(buf, addr, n);
        } else {
            func_80003B94(buf, (u8 *)addr, n);
        }
        buf += n;
        len -= n;
        addr += n;
    }
}
