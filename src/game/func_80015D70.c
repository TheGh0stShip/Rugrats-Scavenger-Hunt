typedef unsigned char u8;
typedef signed int s32;

struct Tbl_80052F18 {
    u8 pad_00[0xC];
    s32 idx[4];
    s32 arg[4];
};

extern struct Tbl_80052F18 D_80052F18;
extern s32 D_80052FEC[];
extern s32 D_80057D5C;
extern void func_80015DF4(s32, s32, s32, s32);

void func_80015D70(void) {
    s32 i;

    for (i = 0; i < 4; i++) {
        if (D_80052F18.arg[i] != 0) {
            func_80015DF4(D_80057D5C, D_80052F18.idx[i], D_80052FEC[D_80052F18.idx[i]], D_80052F18.arg[i]);
        }
    }
}

