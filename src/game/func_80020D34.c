typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

extern u8 D_8006FB70[7];

void func_80020D34(void)
{
    u8 i;

    for (i = 0; i < 7; i++) {
        D_8006FB70[i] = 0;
    }
}

