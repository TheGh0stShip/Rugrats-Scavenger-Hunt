typedef unsigned short u16;

extern float func_800475F0(float value);

float func_8003D680(unsigned char *object)
{
    int remaining = *(u16 *)(object + 0xAA) - object[0xB6];

    if (remaining > 0) {
        float value = func_800475F0((float)remaining * *(float *)(object + 0xDC));
        value *= *(float *)(object + 0x20);
        *(float *)(object + 0x68) = value;
        return value;
    }
    return 0.0f;
}
