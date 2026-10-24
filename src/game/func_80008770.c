typedef unsigned int u32;
typedef int s32;
typedef float f32;

typedef struct {
    u32 w0;
    u32 w1;
} Gfx;

extern Gfx *D_8007784C;
extern s32 func_8000880C(f32 m[4][4], f32 box[6]);

void func_80008770(f32 m[4][4], f32 (*boxes)[6], u32 *dls) {
    s32 i;

    for (i = 0; dls[i] != 0; i++) {
        if (func_8000880C(m, *boxes) != 0) {
            Gfx *_g = D_8007784C++;
            _g->w0 = 0x06000000;
            _g->w1 = dls[i];
        }
        boxes++;
    }
}
