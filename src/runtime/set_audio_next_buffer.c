typedef signed int s32;
typedef unsigned char u8;
typedef unsigned int u32;

extern s32 func_8004D480(void);
extern u32 func_800495D0(void *address);

s32 func_8004D3E0(void *buffer, u32 size) {
    static u8 hardware_bug_flag = 0;
    char *adjusted = buffer;

    if (hardware_bug_flag != 0) {
        adjusted -= 0x2000;
    }
    if ((((u32)buffer + size) & 0x1FFF) == 0) {
        hardware_bug_flag = 1;
    } else {
        hardware_bug_flag = 0;
    }
    if (func_8004D480() != 0) {
        return -1;
    }
    *(volatile u32 *)0xA4500000 = func_800495D0(adjusted);
    *(volatile u32 *)0xA4500004 = size;
    return 0;
}
