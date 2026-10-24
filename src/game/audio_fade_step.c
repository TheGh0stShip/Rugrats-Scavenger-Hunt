
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef float f32;

typedef struct {
    s32 unk0;
    f32 level;
    f32 step;
    s32 dir;
    s32 done;
} Fade_80061D90;

typedef struct {
    u8 pad_00[0x2C];
    s32 musicVol;
    s32 sfxVol;
} Settings_80052F18;

extern Fade_80061D90 D_80061D90;
extern s32 D_80053980;
extern Settings_80052F18 D_80052F18;
extern s32 D_80061E54;
extern void func_8003B9F4(s32, s32);
extern void func_8001F408(u16);
extern void func_8001F30C(void);

s32 func_8001E7FC(void) {
    Fade_80061D90 *f = &D_80061D90;
    Settings_80052F18 *st;
    Fade_80061D90 *g;
    Fade_80061D90 *h;
    f32 max;

    if (f->done != 0) {
        return -1;
    }
    if (D_80053980 == 0) {
        st = &D_80052F18;
        func_8003B9F4(1, (s32)((255.0f - f->level) * (0x80 - ((8 - st->musicVol) << 4))));
        func_8003B9F4(2, (s32)((255.0f - f->level) * (0x80 - ((8 - st->sfxVol) << 4))));
    }
    func_8001F408((u32)((255.0f - D_80061D90.level) * (0x80 - ((8 - D_80052F18.musicVol) << 4))));
    g = &D_80061D90;
    max = 255.0f;
    D_80061E54 = (0x80 - ((8 - D_80052F18.musicVol) << 4)) * 255;
    if (g->level == max) {
        func_8001F30C();
        D_80053980 = 0;
    }
    if (g->dir == 1) {
        g->level += g->step;
        if (max <= g->level) {
            g->level = max;
        }
    } else {
        g->level -= g->step;
        if (g->level <= 0.0f) {
            g->level = 0.0f;
        }
    }
    h = &D_80061D90;
    if (h->level == 255.0f || h->level == 0.0f) {
        if (h->dir != 1) {
            if (h->dir != 0) {
                goto ret;
            }
            h->dir = -1;
        }
        h->done = 1;
        D_80053980 = 0;
ret:
        return 1;

    }
    h->done = 0;
    return 0;
}
