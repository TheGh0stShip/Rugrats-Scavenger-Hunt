typedef signed int s32;

s32 func_8004D0A4(const unsigned char *state, s32 value) {
    s32 scale = *(const s32 *)(state + 0x44);

    return (s32)(float)(((float)value * (float)scale) / 1000000.0 + 0.5);
}
