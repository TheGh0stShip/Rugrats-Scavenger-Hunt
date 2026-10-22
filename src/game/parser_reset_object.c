typedef unsigned char u8;
typedef unsigned int u32;
int func_8003E4D0(void *object) {
    u8 *bytes = object;
    *(u32 *)(bytes + 0x38) = 0;
    *(u32 *)(bytes + 0x34) = 0;
    *(u32 *)(bytes + 0x74) = 0;
    *(u32 *)(bytes + 0x78) = 0;
    *(u32 *)(bytes + 0x44) = 0;
    *(u32 *)(bytes + 0x08) = 0;
    return 0;
}
