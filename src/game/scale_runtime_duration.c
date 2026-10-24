typedef struct RuntimeState {
    unsigned char pad[0x40];
    int scale;
} RuntimeState;

extern RuntimeState *D_80055AF4;

int func_80040C84(int value)
{
    float rounded;

    rounded = ((float)value * (float)D_80055AF4->scale) / 1000000.0 + 0.5;
    return (int)rounded;
}
