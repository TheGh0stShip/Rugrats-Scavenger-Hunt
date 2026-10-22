typedef unsigned char u8;
typedef unsigned int u32;

extern u8 *D_80053634;
extern u32 D_80053638;

void func_8001BED8(u32 value) {
    if (D_80053634 != 0) {
        *(u32 *)(D_80053634 + 0x28) = value;
    }
    if (value == 0x31) {
        D_80053638 = value;
    }
}
