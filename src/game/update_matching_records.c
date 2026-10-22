typedef unsigned char u8;
typedef unsigned int u32;
extern int D_800C3FD4;
extern u8 *D_800C3FDC;
void func_8003E47C(u32 key, u32 mask, u32 bits) {
    int i;
    int initial_count = D_800C3FD4;
    int count;
    u8 *record = D_800C3FDC;
    volatile u8 frame[1];

    __asm__ volatile("" ::: "memory");
    if (initial_count > 0) {
        i = 0;
        count = initial_count;
        do {
            if (*(u32 *)(record + 0x44) == key)
                *(u32 *)record = (*(u32 *)record & mask) | bits;
            i++;
            record += 0x13C;
        } while (i < count);
    }
}
