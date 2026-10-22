typedef unsigned char u8;
typedef unsigned int u32;
extern int D_800C3FD4;
extern u8 *D_800C3FDC;
void func_8003E47C(u32 key, u32 mask, u32 bits) {
    int i;
    u8 *record = D_800C3FDC;
    for (i = 0; i < D_800C3FD4; i++, record += 0x13C) {
        if (*(u32 *)(record + 0x44) == key)
            *(u32 *)record = (*(u32 *)record & mask) | bits;
    }
}
