typedef unsigned char u8;
u8 *func_8003EDA8(void *object, u8 *input) {
    u8 *bytes = object;
    float scale = (float)((double)(float)*input * 0.015625);
    *(float *)(bytes + 0x24) = *(float *)(bytes + 0x70) * scale;
    *(float *)(bytes + 0x6C) = scale;
    return input + 1;
}
