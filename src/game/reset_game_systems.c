/* func_80028E9C  VRAM 0x80028E9C  ROM 0x29A9C  size 0xD8
 * SN GCC 2.8 -O2 -G0 -mips3; assembled after the reviewed SN scheduling normalization. */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;

extern u32 D_80000318;
extern s32 D_8006D698[2];
extern s32 D_80059D40;
extern s32 D_800535E0;
extern s32 D_80097868;
extern s32 D_80052EE4;
extern u32 D_80052EE0;
extern void func_8001FFEC(void *, u32);
extern char D_801B4000[];
extern char D_001B4000[];
extern void func_80012FB0(void);
extern void func_80019B00(void);
extern void func_800338F8(void);
extern void func_80033850(void);
extern void func_80011098(void);
extern void func_8002A110(void);
extern void func_8001E650(void);
extern void func_8001F4B0(void);
extern void func_8001EC78(void);
extern void func_80008F40(void);
extern void func_8001E5D0(void);
extern void func_8001E0B0(void);

void func_80028E9C(void) {
    func_8001FFEC(D_801B4000, D_80000318 - (u32)D_001B4000);
    D_8006D698[0] = 0;
    D_8006D698[1] = 0;
    func_80012FB0();
    func_80019B00();
    func_800338F8();
    func_80033850();
    func_80011098();
    func_8002A110();
    func_8001E650();
    func_8001F4B0();
    func_8001EC78();
    func_80008F40();
    func_8001E5D0();
    func_8001E0B0();
    D_80059D40 = 7;
    D_800535E0 = 0;
    D_80097868 = 0;
    D_80052EE4 = 0;
    D_80052EE0 &= ~1;
}
