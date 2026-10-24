typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct FrameBuf_8009C358 {
    u8 pad_0000[0xE768];
    s16 unkE768;
    u8 pad_E76A[0xE788 - 0xE76A];
    void *cfb;
    u8 pad_E78C[0x4];
};

extern u8 D_B0057E80[];
extern u8 D_B0057FD0[];
extern u8 D_800C7E30[];
extern u8 *D_80052EFC;
extern struct FrameBuf_8009C358 D_8009C358[];

extern u8 D_8013C000[];
extern u8 D_80178000[];
extern void func_80028B58(void *, void *, s32);
extern void func_8004D4A0(s32);

void func_80013B40(void) {
    D_80052EFC = D_800C7E30;
    func_80028B58(D_B0057E80, D_800C7E30, D_B0057FD0 - D_B0057E80);
    D_8009C358[0].unkE768 = 2;
    D_8009C358[0].cfb = D_8013C000;
    D_8009C358[1].unkE768 = 2;
    D_8009C358[1].cfb = D_80178000;
    func_8004D4A0(0x42);
}
