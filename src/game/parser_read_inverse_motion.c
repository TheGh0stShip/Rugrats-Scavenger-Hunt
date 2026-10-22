typedef unsigned char u8;
u8 *func_8003E800(void *object, u8 *input) {
    u8 *bytes = object;
    float inverse;
    float motion;

    bytes[0xB6] = *input++;
    *(volatile u8 *)(bytes + 0xD5) = *input++;
    inverse = (float)(6.2831852 / (double)(float)*(volatile u8 *)(bytes + 0xD5));
    motion = (float)((double)-(float)*input / 50.0);
    *(float *)(bytes + 0xDC) = inverse;
    *(float *)(bytes + 0x20) = motion;
    return input + 1;
}
