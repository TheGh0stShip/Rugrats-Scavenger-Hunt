typedef unsigned char u8;
typedef short s16;
typedef int s32;

typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Vec3i;

typedef struct {
    Vec3i position;
    u8 pad_0C[0x26];
    s16 enabled;
    u8 pad_34[0x08];
} RoundDisplay;

extern s32 func_8001BF3C(void);
extern s32 D_800537D8;
extern s16 D_800B92EC;
extern Vec3i D_80054490[];
extern RoundDisplay D_800544E0[2];
extern s32 D_80054514;

void func_8002D404(void) {
    s32 index;
    RoundDisplay *display;

    if (func_8001BF3C() == 0) {
        D_800537D8 = 0;
        D_800B92EC = 1;
        for (index = 0, display = D_800544E0; index < 2;
             index++, display++) {
            s32 position_index = D_800B92EC - 2;
            Vec3i *position;

            position_index = (index - position_index) % 2;
            position = &D_80054490[position_index];
            display->enabled = 1;
            display->position = *position;
        }
    }
    D_80054514 = 0x1B3;
}
