typedef unsigned int u32;

int func_8000DF94(int value) {
    return (u32)((value & 0x7FFF) - 245) < 4;
}
