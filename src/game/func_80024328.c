/* Target: func_80024328  VRAM 0x80024328  ROM 0x24F28
 * Size: 0x158  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: medium */
typedef signed char s8; typedef unsigned char u8; typedef short s16; typedef unsigned short u16; typedef int s32; typedef unsigned int u32;

struct Grid_800581F0 {
    u8 pad_00[0x12];
    s16 cell[64][64];
};

struct Obj_80024328 {
    u8 pad_000[0x124];
    s32 unk124;
    s32 unk128;
    u8 pad_12C[0x10];
    u8 unk13C;
    u8 unk13D;
};

extern struct Grid_800581F0 *D_800581F0;
extern s32 func_80005798(s16);

void func_80024328(struct Obj_80024328 *obj) {
    s32 done = 0;
    u8 x;
    u8 y;

    do {
        switch (obj->unk128 / 90) {
            case 0:
            case 4:
                x = obj->unk13C;
                y = obj->unk13D + 1;
                break;
            case 1:
                y = obj->unk13D;
                x = obj->unk13C + 1;
                break;
            case 2:
                x = obj->unk13C;
                y = obj->unk13D - 1;
                break;
            case 3:
                y = obj->unk13D;
                x = obj->unk13C - 1;
                break;
        }
        if (func_80005798(D_800581F0->cell[y][x])) {
            done = 1;
        } else {
            obj->unk124 = obj->unk128 = (obj->unk128 + 90) % 360;
        }
    } while (!done);
}
