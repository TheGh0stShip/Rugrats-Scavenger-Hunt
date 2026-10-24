/* Target: func_800218BC  VRAM 0x800218BC  ROM 0x224BC
 * Size: 0x130  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high
 * Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32 */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef float f32;

struct Actor_800218BC {
    u8 pad_00[0x128];
    s32 unk128;
    u8 pad_12C[0x138 - 0x12C];
    s32 index;
    u8 tx;
    u8 ty;
};

extern s32 D_80057D5C;
extern struct Actor_800218BC *func_800055A4(s32);
extern s32 func_80021C3C(struct Actor_800218BC *, u8 *);
extern float sqrtf(float);

struct Actor_800218BC *func_800218BC(struct Actor_800218BC *a)
{
    f32 best = 2147483648.0f;
    s32 i = 0;
    struct Actor_800218BC *nearest = 0;
    struct Actor_800218BC *o;
    f32 dx, dy, d;
    u8 pos[2];

    for (; i < D_80057D5C; i++) {
        if (i == a->index && D_80057D5C != 1) {
            continue;
        }
        o = func_800055A4(i);
        dx = a->tx - o->tx;
        dy = a->ty - o->ty;
        d = sqrtf(dx * dx + dy * dy);
        if (d < best) {
            best = d;
            pos[0] = o->tx;
            nearest = o;
            pos[1] = o->ty;
        }
    }
    a->unk128 = func_80021C3C(a, pos);
    return nearest;
}
