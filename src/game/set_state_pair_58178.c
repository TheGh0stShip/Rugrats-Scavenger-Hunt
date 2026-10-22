typedef unsigned char u8;
typedef unsigned int u32;

extern u8 D_80057F70[];

void func_80009398(u32 first, u32 second) {
    *(u32 *)(D_80057F70 + 0x208) = first;
    *(u32 *)(D_80057F70 + 0x20C) = second;
}
