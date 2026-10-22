typedef unsigned char u8;
u8 *func_8003E8E4(void *object, u8 *input) {
    u8 *bytes = object;
    int value = *input++;
    float delta;

    if (value & 0x80) {
        value |= -0x100;
    }
    delta = (float)((double)(float)value / 100.0);
    *(float *)(bytes + 0x30) = *(float *)(bytes + 0x30) - *(float *)(bytes + 0x90) + delta;
    *(float *)(bytes + 0x90) = delta;
    return input;
}
