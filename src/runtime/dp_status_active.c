typedef unsigned int u32;

int func_80051D20(void) {
    return (*(volatile u32 *)0xA410000C & 0x100) != 0;
}
