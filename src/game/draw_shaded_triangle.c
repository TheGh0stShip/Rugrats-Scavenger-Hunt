/* func_80033290: SN GCC 2.8, scheduling normalized */
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct Vtx_80033290 {
    s16 x;      /* 0x0 */
    s16 y;      /* 0x2 */
    s16 z;      /* 0x4 */
    s16 flag;   /* 0x6 */
    s16 s;      /* 0x8 */
    s16 t;      /* 0xA */
    u8 r, g, b, a; /* 0xC */
} Vtx_80033290;
typedef struct Mtx_80033290 { s32 m[16]; } Mtx_80033290;
typedef struct TriBuf_800C3420 {
    Vtx_80033290 v[3];   /* 0x00 */
    Mtx_80033290 mtx;    /* 0x30 */
} TriBuf_800C3420;       /* size 0x70 */

extern Gfx *D_80077830;
extern s32 D_80052AA8;
extern s32 D_80054820;
extern s32 D_80054824;
extern TriBuf_800C3420 D_800C3420[2][10];
extern void func_80047A38(void *mtx, f32 rx, f32 ry, f32 rz, f32 scale, f32 x, f32 y, f32 z);
extern void func_80047E00(void *p, s32 size);
extern u32 func_800495D0(void *p);

void func_80033290(s32 x0, s32 y0, s32 x1, s32 y1, s32 x2, s32 y2, s32 unused,
                   s32 px, s32 py, s32 pz, s32 r, s32 g, s32 b, s32 a) {
    Vtx_80033290 *v;

    D_80052AA8++;
    { Gfx *_g = D_80077830++; _g->w0 = 0xFCFFFFFF; _g->w1 = 0xFFFE793C; }
    { Gfx *_g = D_80077830++; _g->w0 = 0xFA000000; _g->w1 = 0xFFFFFFFF; }
    { Gfx *_g = D_80077830++; _g->w0 = 0xBA001001; _g->w1 = 0x10000; }
    { Gfx *_g = D_80077830++; _g->w0 = 0xBA001102; _g->w1 = 0; }
    { Gfx *_g = D_80077830++; _g->w0 = 0xBA001301; _g->w1 = 0; }
    { Gfx *_g = D_80077830++; _g->w0 = 0xBB000000; _g->w1 = 0; }
    { Gfx *_g = D_80077830++; _g->w0 = 0xBA000C02; _g->w1 = 0; }
    { Gfx *_g = D_80077830++; _g->w0 = 0xBA000903; _g->w1 = 0xC00; }
    { Gfx *_g = D_80077830++; _g->w0 = 0xB900031D; _g->w1 = 0x00504240; }
    { Gfx *_g = D_80077830++; _g->w0 = 0xE7000000; _g->w1 = 0; }

    v = &D_800C3420[D_80054824][D_80054820].v[0];
    v->y = y0;
    v->r = r; v->g = g; v->b = b; v->a = a;
    v->x = x0;
    v->z = 0; v->s = 0; v->t = 0;

    v = &D_800C3420[D_80054824][D_80054820].v[1];
    v->x = x1;
    v->r = r; v->g = g; v->b = b; v->a = a;
    v->y = y1;
    v->z = 0; v->s = 0; v->t = 0;

    v = &D_800C3420[D_80054824][D_80054820].v[2];
    v->x = x2;
    v->y = y2;
    v->z = 0; v->s = 0; v->t = 0;
    v->r = r; v->g = g; v->b = b; v->a = a;

    v = (Vtx_80033290 *)&D_800C3420[D_80054824][D_80054820];
    func_80047A38(&((TriBuf_800C3420 *)v)->mtx, 0.0f, 0.0f, 0.0f, 1.0f, (f32)px, (f32)py, (f32)pz);
    func_80047E00(&((TriBuf_800C3420 *)v)->mtx, 0x40);
    { Gfx *_g = D_80077830++; _g->w0 = 0x01020040; _g->w1 = (u32)&((TriBuf_800C3420 *)v)->mtx; }
    func_80047E00(v, 0x30);
    { Gfx *_g = D_80077830++; _g->w0 = 0xBC000C06; _g->w1 = func_800495D0(v); }
    { Gfx *_g = D_80077830++; _g->w0 = 0x04000C2F; _g->w1 = 0x03000000; }
    { Gfx *_g = D_80077830++; _g->w0 = 0xFA000000; _g->w1 = (((u32)r & 0xFF) << 24) | (((u32)g & 0xFF) << 16) | (((u32)b & 0xFF) << 8) | ((u32)a & 0xFF); }
    D_80054820++;
    { Gfx *_g = D_80077830++; _g->w0 = 0xBF000000; _g->w1 = 0x204; }
    { Gfx *_g = D_80077830++; _g->w0 = 0xE7000000; _g->w1 = 0; }
}
