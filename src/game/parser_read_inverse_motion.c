typedef unsigned char u8;
u8 *func_8003E800(void *object, u8 *input) {
    u8 *bytes = object;
    bytes[0xB6] = *input++;
    bytes[0xD5] = *input++;
    *(float *)(bytes + 0xDC) = (float)(6.2831852 / (double)(float)bytes[0xD5]);
    *(float *)(bytes + 0x20) = (float)((double)-(float)*input / 50.0);
    return input + 1;
}
