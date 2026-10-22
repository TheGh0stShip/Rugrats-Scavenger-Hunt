typedef unsigned int u32;

int func_8004DAA0(void) {
    return (*(volatile u32 *)0xA4800018 & 3) != 0;
}
