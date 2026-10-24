typedef unsigned char u8;
typedef signed char s8;
typedef unsigned long long u64;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

typedef struct {
    u32 w0;
    u32 w1;
} Gfx;

struct Task_80013EE0 {
    s32 unk0;
    u8 pad_04[4];
    s32 unk8;
    s32 unkC;
    s32 type;
    s32 flags;
    void *ucodeBoot;
    s32 ucodeBootSize;
    void *ucode;
    u8 pad_24[4];
    void *outputBuff;
    s32 outputBuffSize;
    void *dataPtr;
    s32 dataSize;
    void *yieldPtr;
    void *yieldEnd;
    Gfx *dlStart;
    s32 dlSize;
    void *unk48;
    s32 unk4C;
    void *unk50;
    void *unk54;
    u8 pad_58[8];
    u64 unk60;
    Gfx dl[1];
    u8 pad_70[0xE768 - 0x70];
    u8 unkE768[0x20];
    s32 unkE788;
};

extern Gfx *D_8007784C;
extern Gfx *D_80052F00;
extern s32 D_80052EF8;
extern u64 D_80058CE8;
extern void *D_8009C350;
extern u8 D_800367F0[];
extern u8 D_800368C0[];
extern u8 rspbootTextEnd[];
extern u8 D_80054BC0[];
extern u8 D_80058CF0[];
extern u8 D_8006FFA0[];
extern u8 D_800590F0[];
extern u8 D_80077850[];

extern s32 func_80012DBC(void);
extern void func_80015CCC(u64);
extern void func_80047E00(void *, s32);
extern void func_80048F00(void *, void *, s32);
extern void func_80047E80(void);

void func_80013EE0(struct Task_80013EE0 *t) {
    Gfx *g;
    u8 *boot;

    func_80012DBC();
    func_80015CCC(D_80058CE8);
    g = D_8007784C++;
    g->w0 = 0xE9000000;
    g->w1 = 0;
    g = D_8007784C++;
    g->w0 = 0xB8000000;
    g->w1 = 0;
    func_80047E00(t->dl, (u8 *)D_8007784C - (u8 *)t->dl);

    boot = D_800367F0;
    t->type = 1;
    t->ucodeBootSize = (s32)rspbootTextEnd - (s32)boot;
    t->ucode = D_800368C0;
    t->outputBuff = D_80054BC0;
    t->outputBuffSize = 0x800;
    t->dataPtr = D_80058CF0;
    t->dataSize = 0x400;
    t->yieldPtr = D_8006FFA0;
    t->yieldEnd = D_8006FFA0 + 0x2000;
    t->unk48 = D_800590F0;
    t->unk4C = 0xC00;
    t->unk8 = 0x63;
    t->unk50 = D_80077850;
    t->unk54 = t->unkE768;
    t->flags = 0;
    t->ucodeBoot = boot;
    t->unk0 = 0;
    t->dlStart = D_80052F00 + 0x30;
    t->dlSize = (D_8007784C - (D_80052F00 + 0x30)) * sizeof(Gfx);
    t->unkC = t->unkE788;
    t->unk60 = 0;
    func_80048F00(D_8009C350, t, 1);
    func_80047E80();
    D_80052EF8++;
}
