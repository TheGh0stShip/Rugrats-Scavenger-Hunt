/* func_8000F800  VRAM 0x8000F800  ROM 0x10400  Size: 0x194
 * Build view/projection matrices and emit their display-list commands.
 * SN GCC 2.8 -O2 with reviewed scheduling normalization. */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

typedef f32 Mtx_f[4][4];
typedef struct { s32 m[4][4]; } Mtx;
typedef struct { u32 w0; u32 w1; } Gfx;

struct Cam_8000F800 {
    u8 pad_00[0x28];
    f32 unk_28;
    f32 unk_2C;
    f32 unk_30;
    u8 pad_34[0x40 - 0x34];
    f32 unk_40;
    f32 unk_44;
    f32 unk_48;
    u8 pad_4C[0x58 - 0x4C];
    f32 unk_58;
    f32 unk_5C;
    f32 unk_60;
    u8 pad_64[4];
    Mtx_f unk_68[2];
};

extern s32 D_80054358;
extern struct Cam_8000F800 *D_80052F04;
extern Mtx D_80058878[];
extern Mtx D_800587F8[];
extern s16 D_80052BB0;
extern Gfx *D_8007784C;

extern void func_800461E0(Mtx_f m, f32 ex, f32 ey, f32 ez, f32 ax, f32 ay, f32 az, f32 ux, f32 uy, f32 uz);
extern void func_800468F0(Mtx_f src, Mtx *dst);
extern void func_80047E00(void *p, s32 size);
extern void func_80047060(Mtx *m, u16 *perspNorm, f32 fovy, f32 aspect, f32 near, f32 far, f32 scale);

void func_8000F800(struct Cam_8000F800 *cam) {
    u16 perspNorm;
    Gfx *g;

    D_80052F04 = cam;
    func_800461E0(cam->unk_68[D_80054358], cam->unk_28, cam->unk_2C, cam->unk_30,
                  cam->unk_58, cam->unk_5C, cam->unk_60, cam->unk_40, cam->unk_44, cam->unk_48);
    func_800468F0(cam->unk_68[D_80054358], &D_80058878[D_80054358]);
    func_80047E00(&D_80058878[D_80054358], 0x40);
    func_80047060(&D_800587F8[D_80054358], &perspNorm, 40.0f, 2.1333334f, 250.0f,
                  (f32)D_80052BB0, 1.0f);
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
}
