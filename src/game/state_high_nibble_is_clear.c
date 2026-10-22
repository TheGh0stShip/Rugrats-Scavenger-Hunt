typedef unsigned int u32;

extern u32 D_80052AB8;

int func_8000B8EC(void) {
    return (D_80052AB8 & 0xF0000000) == 0;
}
