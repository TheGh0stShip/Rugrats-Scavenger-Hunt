typedef unsigned char u8;
typedef unsigned int u32;

void func_80043100(u8 *destination, u8 value, u32 count) {
    register int remaining __asm__("$2");
    register int finished __asm__("$3");
    volatile u8 frame[1];

    __asm__ volatile("" ::: "memory");
    if (count != 0) {
        remaining = count - 1;
        finished = -1;
        do {
            *destination = value;
            remaining--;
            destination++;
        } while (remaining != finished);
    }
}
