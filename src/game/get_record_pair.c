typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;
typedef signed int s32;

typedef struct Pair {
    u16 first;
    u16 second;
} Pair;

extern u8 *D_800581F0;
extern s32 D_80052F18;
extern Pair D_8005356C[];

void func_80019DE0(s32 index, u16 *first, u16 *second) {
    register u8 *record __asm__("$3") = D_800581F0 + index * 0x210;
    s32 pair_index = *(s16 *)(record + 0x201C) - 0x47;

    if (pair_index == 0xC) {
        pair_index = D_80052F18 + 0xC;
    }

    *first = D_8005356C[pair_index].first;
    *second = D_8005356C[pair_index].second;
}
