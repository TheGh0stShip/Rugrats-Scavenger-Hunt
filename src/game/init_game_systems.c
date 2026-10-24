/* func_80028C18  VRAM 0x80028C18  ROM 0x29818  size 0x284
 * SN GCC 2.8 -O2 -G0 -mips3; assembled after the reviewed SN scheduling normalization. */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned long long u64;

struct ViMode_800774C0 {
    u8 pad_00[0x8];
    s32 unk08;
    u8 pad_0C[0x14];
    s32 unk20;
    u8 pad_24[0x4];
    s32 unk28;
};

extern s32 D_80000300;
extern u32 D_80000318;
extern u8 D_80077810[];
extern u8 D_80077828[];
extern u8 D_80077850[];
extern u8 D_80099B10[];
extern u8 D_800664B0[];
extern u8 D_80061DD8[];
extern u8 D_80097878[];
extern u8 D_80099B08[];
extern u8 D_80099B40[];
extern s32 D_80054348;
extern struct ViMode_800774C0 *D_800774C0;
extern struct ViMode_800774C0 D_80056470;
extern struct ViMode_800774C0 D_80056D30;
extern s32 D_8009C350;
extern s32 D_80099B30;
extern s32 D_80059D40;
extern s32 D_80097868;
extern u64 D_80097870;
extern void func_80047EB0(void *, void *, s32);
extern void func_80051120(void *, void *, s32, s32, s32);
extern void func_80051268(void *, void *, void *);
extern s32 func_80051350(void *);
extern void func_80013B40(void);
extern void func_8001FFEC(void *, u32);
extern void func_80012FB0(void);
extern void func_80047E80(void);
extern void func_80019B00(void);
extern void func_800338F8(void);
extern void func_80033850(void);
extern void func_80011098(void);
extern void func_8002A110(void);
extern void func_8001F4B0(void);
extern void func_8001EA7C(void);
extern void func_8001E650(void);
extern void func_8001E6F8(s32);
extern void func_80008F40(void);
extern void func_8001E5D0(void);
extern void func_8001E0B0(void);
extern void func_800196E8(void);
extern u64 func_80048A10(void);
extern char D_801B4000[];
extern char D_001B4000[];

void func_80028C18(void) {
    struct ViMode_800774C0 *mode;

    func_80047EB0(D_80077810, D_80077828, 1);
    func_80047EB0(D_80077850, D_80099B10, 8);
    func_80047EB0(D_800664B0, D_80061DD8, 10);

    if (D_80000300 == 1) {
        func_80051120(D_80097878, D_80099B08, 13, 2, 1);
    } else if (D_80000300 == 2) {
        func_80051120(D_80097878, D_80099B08, 13, 30, 1);
    } else {
        func_80051120(D_80097878, D_80099B08, 13, 2, 1);
        D_80054348 = 1;
    }

    if (D_80000300 == 1) {
        D_800774C0 = &D_80056470;
    } else if (D_80000300 == 2) {
        D_800774C0 = &D_80056D30;
    } else {
        D_800774C0 = &D_80056470;
        D_80054348 = 1;
    }

    mode = D_800774C0;
    mode->unk08 = 0x200;
    mode->unk20 = 0x333;
    mode->unk28 = 0x400;
    func_80051268(D_80097878, D_80099B40, D_80077850);
    D_8009C350 = func_80051350(D_80097878);
    func_80013B40();
    func_8001FFEC(D_801B4000, D_80000318 - (u32)D_001B4000);
    func_80012FB0();
    func_80047E80();
    func_80019B00();
    func_800338F8();
    func_80047E80();
    func_80033850();
    func_80047E80();
    func_80011098();
    func_80047E80();
    func_8002A110();
    func_8001F4B0();
    func_8001EA7C();
    func_8001E650();
    func_8001E6F8(1);
    func_80047E80();
    func_80008F40();
    func_8001E5D0();
    func_8001E0B0();
    D_80099B30 = 0;
    D_80059D40 = 0;
    D_80097868 = 0;
    func_800196E8();
    D_80097870 = func_80048A10();
}
