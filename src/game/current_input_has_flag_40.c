extern void *D_8005B214;
extern int func_800166B0(void *input);

int func_8001C36C(void) {
    unsigned int value = func_800166B0(D_8005B214) & 0x40;
    return 0 < value;
}
