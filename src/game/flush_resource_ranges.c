extern void func_800510F0(void *address);
void func_8001F480(void *address) {
    unsigned int fixed_address;
    func_800510F0(address);
    fixed_address = 0x80000000U;
    __asm__("" : "=r"(fixed_address) : "0"(fixed_address));
    func_800510F0((void *)(fixed_address + 0x1D90));
}
