typedef unsigned char u8;
typedef unsigned int u32;
u8 *func_8003E54C(void *object, u8 *input) {
    u8 *bytes = object;
    u8 duration = *input++;
    u8 divisor;
    u8 next;
    if (duration == 0) duration = 1;
    bytes[0xBF] = duration;
    *(u32 *)(bytes + 0x64) = 0x400 / duration;
    bytes[0xC0] = *input++;
    divisor = *input++;
    bytes[0xC6] = divisor;
    next = *input++;
    *(float *)(bytes + 0x58) = (float)((1.0 / (double)(float)divisor) *
                                      (double)(float)(next - bytes[0xC0]));
    bytes[0xC1] = next;
    divisor = *input++;
    bytes[0xC7] = divisor;
    next = *input++;
    *(float *)(bytes + 0x5C) = (float)((1.0 / (double)(float)divisor) *
                                      (double)(float)(next - bytes[0xC1]));
    bytes[0xC2] = next;
    divisor = *input;
    bytes[0xC8] = divisor;
    *(float *)(bytes + 0x60) = (float)(1.0 / (double)(float)divisor);
    return input + 1;
}
