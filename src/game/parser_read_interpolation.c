typedef unsigned char u8;
typedef unsigned int u32;
u8 *func_8003E54C(void *object, u8 *input) {
    u8 *bytes = object;
    volatile u8 *fields = object;
    u8 duration = *input++;
    register int divisor __asm__("$6");
    register int next __asm__("$3");
    register int previous __asm__("$2");
    if (duration == 0) duration = 1;
    fields[0xBF] = duration;
    *(u32 *)(bytes + 0x64) = 0x400 / duration;
    fields[0xC0] = *input++;
    divisor = *input++;
    previous = fields[0xC0];
    fields[0xC6] = divisor;
    next = *(volatile u8 *)input++;
    *(float *)(bytes + 0x58) = (float)((1.0 / (double)(float)divisor) *
                                      (double)(float)(next - previous));
    fields[0xC1] = next;
    divisor = *input++;
    previous = fields[0xC1];
    fields[0xC7] = divisor;
    next = *(volatile u8 *)input++;
    *(float *)(bytes + 0x5C) = (float)((1.0 / (double)(float)divisor) *
                                      (double)(float)(next - previous));
    fields[0xC2] = next;
    divisor = *input;
    fields[0xC8] = divisor;
    *(float *)(bytes + 0x60) = (float)(1.0 / (double)(float)divisor);
    return input + 1;
}
