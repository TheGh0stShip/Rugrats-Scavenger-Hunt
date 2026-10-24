/* func_8001F588: selects the active scene slot and, on first use, fills its
 * eight tile display lists with texture-block load commands.
 * SN Systems GCC 2.8 -O2; scheduling reviewed in tools/normalize_sn_schedule.py */
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

typedef struct {
    u32 w0;
    u32 w1;
} Gfx;

typedef struct {
    u8 pad_00[4];
    s32 unk4;
    u8 pad_08[4];
    s32 unkC;
    u8 pad_10[0x12];
    u16 unk22;
    u8 pad_24[0x12C];
    u8 unk150;
    u8 pad_151[0x207];
    u8 unk358;
} Scene_8001F588;

typedef struct {
    Gfx *dl[8];
    Gfx buf[1];
    u8 pad[0x70A0 - 0x28];
} DlSet_8001F588;

#define GFX(pp, a, b) { Gfx *_t = (pp); Gfx *_g; __asm__ __volatile__("" : "=r"(_g) : "0"(_t)); (pp) = _t + 1; _g->w0 = (a); _g->w1 = (b); }

extern DlSet_8001F588 *D_8006D680;
extern s32 D_8006D684;
extern Scene_8001F588 *D_8006D688;
extern Scene_8001F588 *D_8006D698[];
extern s32 D_80053990;
extern Scene_8001F588 *func_80013238(s32);

s32 func_8001F588(s32 arg0) {
    s32 prev;
    s32 idx;
    Scene_8001F588 **p;
    DlSet_8001F588 *set;

    prev = D_8006D684;
    D_8006D684 = arg0;
    D_80053990 = prev;
    if (D_8006D698[arg0] == 0) {
        D_8006D698[D_8006D684] = func_80013238(arg0);
    }
    idx = D_8006D684;
    p = &D_8006D698[idx];
    (*p)->unk4 = arg0;
    D_8006D688 = *p;
    set = &D_8006D680[idx];
    if (set->dl[0] == set->buf) {
        s32 i;
        s32 off;
        Gfx **dl;
        Scene_8001F588 **q;
        i = 0;
        __asm__("" : "=r"(q) : "0"(p));
        dl = set->dl;
        off = 0x850;
        {
            s32 w = (u16)((*q)->unk22 * 2);
            s32 words = (u32)w >> 4;
            u32 settile = (((s32)(((u32)w >> 1) + 7) >> 3) & 0x1FF) << 9 | 0xF5000000;
            u32 tilesize = (((w - 1) << 2) & 0xFFF) << 12;
    loop:
        {
            s32 dxt;
            s32 lrs;
            u32 hi;

            GFX(*dl, 0xFD100000, (u32)*q + off);
            {
                u32 k = 0x070A82A0;
                GFX(*dl, 0xF5100000, k);
            }
            GFX(*dl, 0xE6000000, 0);
            {
                Gfx *g = (*dl)++;
                g->w0 = 0xF3000000;
                dxt = 0x800;
                if (words != 0) {
                    dxt = words + 0x7FF;
                }
                lrs = ((w * (*q)->unkC + 3) >> 2) - 1;
                if (lrs >= 0x800) {
                    lrs = 0x7FF;
                }
                hi = ((lrs & 0xFFF) << 12) | 0x07000000;
                if (words != 0) {
                    g->w1 = hi | ((dxt / words) & 0xFFF);
                } else {
                    g->w1 = hi | (dxt & 0xFFF);
                }
            }
            GFX(*dl, 0xE7000000, 0);
            {
                u32 k = 0x000A82A0;
                GFX(*dl, settile, k);
            }
            off += 0x800;
            GFX(*dl, 0xF2000000, tilesize | ((((*q)->unkC - 1) << 2) & 0xFFF));
            dl++;
            i++;
        }
        if (i < 8) goto loop;
        }
    }
    if (D_8006D688->unk150 == 0) {
        D_8006D688->unk150 = D_8006D688->unk358;
    }
    return D_80053990;
}
