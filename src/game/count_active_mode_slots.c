typedef unsigned char u8;

typedef struct ModeState {
    u8 unused0[0x1C];
    int slots[4];
} ModeState;

extern ModeState D_80052F18;
extern int D_80057D5C;

void func_800196A4(void) {
    int index;

    D_80057D5C = 0;
    for (index = 0; index < 4; index++) {
        if (D_80052F18.slots[index] != 0) {
            D_80057D5C++;
        }
    }
}
