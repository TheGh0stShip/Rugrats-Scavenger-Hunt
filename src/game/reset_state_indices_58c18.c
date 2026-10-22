typedef unsigned char u8;
typedef signed int s32;

extern u8 D_80058C18[];

void func_80012CD0(void) {
    *(s32 *)(D_80058C18 + 0xC) = -1;
    *(s32 *)(D_80058C18 + 0x10) = -1;
}
