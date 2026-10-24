typedef unsigned char u8;
typedef unsigned short u16;
typedef int s32;

struct Elem_80058B58 {
    u16 unk0;   /* 0x00 */
    u16 flags;  /* 0x02 */
    u8 pad_04[6];
};

extern s32 D_80053828;
extern s32 D_800529F4;
extern struct Elem_80058B58 D_80058B58[];

s32 func_8001B7B0(void) {
    if (D_80053828 != 0) {
        D_80053828--;
        return 0;
    }
    return D_80058B58[D_800529F4].flags == 0x2010;
}

