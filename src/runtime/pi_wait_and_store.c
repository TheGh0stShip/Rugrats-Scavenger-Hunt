typedef unsigned int u32;

void func_80003A58(u32 *destination, u32 value) {
    register volatile u32 *piStatus __asm__("$3");
    register u32 status __asm__("$2") = *(volatile u32 *)0xA4600010;

    if ((status & 3) != 0) {
        __asm__ volatile("" ::: "memory");
        piStatus = (volatile u32 *)0xA4600010;
        do {
            status = *piStatus;
        } while ((status & 3) != 0);
    }
    *destination = value;
}
