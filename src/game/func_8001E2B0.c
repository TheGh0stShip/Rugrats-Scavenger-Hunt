/* Target: func_8001E2B0  VRAM 0x8001E2B0  ROM 0x1EEB0  Size: 800
 * Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: medium */

typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef float f32;

typedef struct {
    u32 w0;
    u32 w1;
} Gfx;

typedef struct {
    f32 m[4][4];
} MtxF;

typedef struct {
    s32 m[4][4];
} Mtx;

typedef struct {
    f32 x, y, z;
} Vec3f;

typedef struct {
    s16 x, y, z;
} Vec3s;

typedef struct {
    u32 total;
} Track_8005B834;

typedef struct {
    s32 unk0;
    s32 unk4;
} Pair_8005B830;

typedef struct {
    s32 id;
    Track_8005B834 *track;
    s32 count;
    Pair_8005B830 entries[20];
    u32 frame;
    u32 extra;
    MtxF lookAtF[2];
} Scene_8005B830;

extern s32 D_8007782C;
extern Gfx *D_8007784C;
extern s32 D_80054358;
extern s16 D_80052BB0;
extern Scene_8005B830 D_8005B830;
extern Mtx D_800587F8[];
extern Mtx D_80058878[];
extern f32 D_80001D60;
extern f32 D_80001D64;

extern void func_8001E28C(void);
extern void func_8000F2B8(Track_8005B834 *, u32, Vec3s *, Vec3s *);
extern void func_800461E0(MtxF *, f32, f32, f32, f32, f32, f32, f32, f32, f32);
extern void func_800468F0(MtxF *, Mtx *);
extern void func_80047E00(void *, s32);
extern void func_80047060(Mtx *, u16 *, f32, f32, f32, f32, f32);
extern void func_8000C3AC(void *, float *);
extern void *func_8001E630(void);
extern void func_80047A38(void *, f32, f32, f32, f32, f32, f32, f32);
extern void * func_8000B910(void *, void *, void *, void *, void *, int);
extern void func_8000C674(void *);

s32 func_8001E2B0(void) {
    Vec3s eye;
    Vec3s at;
    Vec3f eyeF;
    Vec3f rot;
    Vec3f pos;
    u16 perspNorm;
    register f32 one asm("$f22");
    register f32 rx asm("$f20");
    s32 i;
    s32 obj;
    void *m;
    Gfx *g;

    if (D_8007782C != 0) {
        func_8001E28C();
    }
    if (D_8005B830.frame >= D_8005B830.track->total + D_8005B830.extra) {
        return 1;
    }
    func_8000F2B8(D_8005B830.track, D_8005B830.frame, &eye, &at);
    one = D_80001D60;
    func_800461E0(&D_8005B830.lookAtF[D_80054358], eye.x, eye.y, eye.z, at.x, at.y, at.z, 0.0f, one, 0.0f);
    eyeF.x = eye.x;
    eyeF.y = eye.y;
    eyeF.z = eye.z;
    func_800468F0(&D_8005B830.lookAtF[D_80054358], &D_80058878[D_80054358]);
    func_80047E00(&D_80058878[D_80054358], 0x40);
    func_80047060(&D_800587F8[D_80054358], &perspNorm, 40.0f, 2.1333334f, D_80001D64, D_80052BB0, one);
    func_80047E00(&D_800587F8[D_80054358], 0x40);

    g = D_8007784C++;
    g->w0 = 0xBC00000E;
    g->w1 = perspNorm;
    g = D_8007784C++;
    g->w0 = 0x01030040;
    g->w1 = (u32)&D_800587F8[D_80054358];
    g = D_8007784C++;
    g->w0 = 0x01010040;
    g->w1 = (u32)&D_80058878[D_80054358];

    rot.x = 0.0f;
    rot.y = 0.0f;
    rot.z = 0.0f;
    D_8005B830.frame++;
    for (i = 0; i < D_8005B830.count; i++) {
        obj = D_8005B830.entries[i].unk4;
        pos.x = 0.0f;
        pos.y = 0.0f;
        pos.z = 0.0f;
        func_8000C3AC(obj, &pos);
        m = func_8001E630();
        rx = rot.x;
        func_80047A38(m, rx, rx, rx, one, pos.x, pos.y, pos.z);
        func_80047E00(m, 0x40);
        func_8000B910(obj, &rot, &eyeF, 0, m, 0);
        func_8000C674(obj);
    }
    return 0;
}
