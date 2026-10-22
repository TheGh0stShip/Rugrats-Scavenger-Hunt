typedef unsigned int u32;

int func_80051F80(void) {
    return (*(volatile u32 *)0xA4040010 & 0x1C) != 0;
}
