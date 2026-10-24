typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef float f32;

typedef struct {
    s32 unk0;
    f32 unk4;
    f32 unk8;
    s32 unkC;
    s32 unk10;
} State_80061D90;

extern State_80061D90 D_80061D90;
extern s32 D_80053980;

void func_8001E678(s32 arg0, s32 arg1) {
    D_80061D90.unk8 = 0xFF / (arg0 + 1);
    if (arg1 == 0) {
        D_80061D90.unk4 = 255.0f;
    } else {
        D_80061D90.unk4 = 0.0f;
    }
    D_80061D90.unk0 = arg0;
    D_80061D90.unkC = arg1;
    D_80061D90.unk10 = 0;
    D_80053980 = 0;
}

