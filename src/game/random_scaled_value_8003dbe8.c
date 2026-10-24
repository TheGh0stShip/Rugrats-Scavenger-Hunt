extern int D_800C3FF4;

int func_8003DBE8(int range)
{
    int index;

    for (index = 0; index < 8; index++) {
        int state = D_800C3FF4;
        int next = state << 1;
        int bits = state & 0x48000000;

        D_800C3FF4 = next;
        if (bits == 0x48000000 || bits == 0x08000000) {
            D_800C3FF4 = next | 1;
        }
    }
    return (int)((float)range * (((float)D_800C3FF4 / 65536.0f) / 65536.0f));
}
