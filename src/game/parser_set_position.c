typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
void *func_8003EB58(void *object, u8 *input) {
    u8 *bytes = object;
    register int first_high __asm__("$6") = *input++;
    register int first_low __asm__("$9") = *input++;
    register int second_high __asm__("$7") = *input++;
    register int second_low __asm__("$3") = *input++;
    register int third_high __asm__("$2");
    register int third_low __asm__("$5");
    register int third __asm__("$7");

    *(u16 *)(bytes + 0xA2) = 1;
    second_high <<= 8;
    __asm__("addu %0,%0,%2" : "=r"(second_high)
                           : "0"(second_high), "r"(second_low));
    *(u32 *)(bytes + 0x38) = *(u32 *)(bytes + 0x8C) + second_high;
    third_high = input[0];
    third_low = input[1];
    first_high <<= 8;
    __asm__("addu %0,%0,%2" : "=r"(first_high)
                           : "0"(first_high), "r"(first_low));
    *(u16 *)(bytes + 0xA4) = 1;
    third = third_high << 8;
    __asm__("addu %0,%0,%2" : "=r"(third)
                           : "0"(third), "r"(third_low));
    *(u32 *)(bytes + 0x34) = *(u32 *)(bytes + 0x88) + third;
    return (void *)(*(u32 *)(bytes + 0x80) + first_high);
}
