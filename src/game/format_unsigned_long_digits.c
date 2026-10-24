typedef unsigned char u8;
typedef int s32;
typedef unsigned long long u64;

extern char *D_800C3E70;
extern char D_800C3FA0[];

char *func_80039068(u64 value, s32 base) {
    char *p = D_800C3FA0;
    volatile s32 d;

    do {
        d = value % base;
        *--p = D_800C3E70[d];
        value /= base;
    } while (value != 0);
    return p;
}
