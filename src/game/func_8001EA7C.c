/* Target: func_8001EA7C  VRAM 0x8001EA7C  ROM 0x1F67C  Size: 508
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
    s32 unk0;
    s32 unk4;
    void *unk8;
    s32 unkC;
    void *unk10;
    s32 unk14;
    void *unk18;
    u32 unk1C;
    void *unk20;
    u8 pad_24[4];
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s32 unk38;
    s32 unk3C;
} AudioCfg_80061E08;

extern u8 D_B0057FD0[];
extern u8 D_B005B470[];
extern u8 D_B005B470_DMA[];
extern u8 D_B005B470_CFG[];
extern u8 D_B01105F0[];
extern u8 D_B0111310[];
extern u8 D_B0111310_DMA[];
extern u8 D_B0111310_FINAL[];
extern u8 D_B0215C80[];
extern u8 D_B021AC00[];
extern s32 D_80061E54;
extern u8 D_80061E60[];
extern u8 D_800664C8[];
extern u8 D_800674D0[];
extern u32 D_800674C8;
extern void *D_80061DC8[2];
extern void *D_80061DD0;
extern void *D_80061E4C;
extern u8 D_80097878[];
extern AudioCfg_80061E08 D_80061E08;

extern void func_8001EDEC(u32, void *, s32);
extern void *func_8001A2B0(s32);
extern void *func_80038E90(void *, s32, s32);
extern void func_80047E80(void);
extern void func_8003B76C(AudioCfg_80061E08 *);
extern void func_8001F0A8(s32, s32);
extern void func_8003C2E4(void *, u32);
extern s32 func_8004D2B0(s32);
extern void func_800431A0(s32, s32, s32, s32);

void func_8001EA7C(void) {
    void *p;

    D_80061E54 = 0xF777;
    func_8001EDEC((u32)D_B0057FD0, D_80061E60, D_B005B470_DMA - D_B0057FD0);
    func_8001EDEC((u32)D_B01105F0, D_800664C8, D_B0111310_DMA - D_B01105F0);
    D_80061DC8[0] = func_8001A2B0(0x4E20);
    D_80061DC8[1] = func_8001A2B0(0x4E20);
    func_80038E90(D_80061DC8[0], 0, 0x4E20);
    func_80038E90(D_80061DC8[1], 0, 0x4E20);
    func_8001EDEC((u32)D_B0215C80, D_800674D0, D_B021AC00 - D_B0215C80);
    p = func_8001A2B0(0x3D090);
    D_80061E4C = p;
    D_80061DD0 = p;
    func_80038E90(p, 0, 0x3D090);

    D_80061E08.unk0 = 0;
    D_80061E08.unk4 = 0x18;
    D_80061E08.unk8 = D_80097878;
    D_80061E08.unkC = 0xC;
    D_80061E08.unk10 = D_80061E4C;
    D_80061E08.unk14 = 0x3D090;
    D_80061E08.unk18 = D_80061E60;
    D_80061E08.unk1C = (u32)D_B005B470_CFG;
    D_80061E08.unk20 = D_800664C8;
    D_80061E08.unk28 = 0x80;
    D_80061E08.unk2C = 0x55F0;
    D_80061E08.unk30 = 0x1000;
    D_80061E08.unk34 = 1;
    D_80061E08.unk38 = 0x40;
    D_80061E08.unk3C = 0x500;
    func_80047E80();
    func_8003B76C(&D_80061E08);
    func_8001F0A8(0, 0);
    D_800674C8 = (u32)D_B0111310_FINAL;
    func_8003C2E4(D_800674D0, (u32)D_B0111310_FINAL);
    func_800431A0(4, func_8004D2B0(0x55F0), 0, 0);
    func_80047E80();
}
