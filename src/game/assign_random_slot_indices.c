/* func_80033D00: SN GCC 2.8, scheduling normalized */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;

typedef struct Slots_80052F18 {
    u8 pad_00[0xC];
    s32 idx[4];    /* 0x0C */
    s32 active[4]; /* 0x1C */
} Slots_80052F18;
extern Slots_80052F18 D_80052F18;
extern s32 func_8003A840(void);

void func_80033D00(void) {
    s32 used[4];
    s32 i;
    s32 k;

    for (i = 3; i >= 0; i--) {
        used[i] = 0;
    }
    for (i = 0; i < 4; i++) {
        if (D_80052F18.active[i] != 0) {
            *(D_80052F18.idx[i] + used) = 1;
        }
    }
    for (i = 0; i < 4; i++) {
        if (D_80052F18.active[i] == 0) {
            D_80052F18.active[i] = 1;
            do {
                k = func_8003A840() % 4;
                D_80052F18.idx[i] = k;
            } while (*(k + used) != 0);
            used[k] = 1;
        }
    }
}
