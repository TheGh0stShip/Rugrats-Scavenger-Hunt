/* Target: func_800206F8  VRAM 0x800206F8  ROM 0x212F8
 * Size: 0x17C  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high
 * Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32 */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef float f32;

struct Actor_800206F8 {
    u8 pad_00[8];
    s16 unk8;
    s16 unkA;
    s32 unkC;
    void *update;
    void *draw;
    void *cb18;
    u8 pad_1C[0xE0 - 0x1C];
    s32 unkE0;
    u8 pad_E4[0xEC - 0xE4];
    f32 unkEC;
    u8 pad_F0[0xF4 - 0xF0];
    s32 index;
    s32 kind;
    u8 pad_FC[0x11C - 0xFC];
    s32 unk11C;
    u8 pad_120[0x134 - 0x120];
    u8 unk134[4];
    u8 pad_138[0x1D8 - 0x138];
    u8 unk1D8[0x200 - 0x1D8];
};

extern s32 D_80052F18;
extern s32 D_80059DA8;
extern struct Actor_800206F8 *D_80057DA8[];
extern s32 D_80057D58;
extern void *func_8001A2B0(s32);
extern void *func_80038E90(void *, s32, s32);
extern void *func_800197C8(s32);
extern void func_80020D68(void);
extern void func_80020DB8(void);
extern void func_80020E6C(void);
extern void func_80020874(struct Actor_800206F8 *);
extern void func_800090C0(struct Actor_800206F8 *);
extern void func_800212A4(struct Actor_800206F8 *);
extern void func_80007EFC(void *, void *, int, void *);
extern void func_8002E7FC(void *, s32);
extern void func_8000E1D0(struct Actor_800206F8 *);

void func_800206F8(s32 index, s32 kind, s32 arg2)
{
    struct Actor_800206F8 *o;

    o = func_8001A2B0(0x200);
    func_80038E90(o, 0, 0x200);
    if (D_80052F18 == 1) {
        arg2++;
    }
    func_800197C8(arg2);
    o->update = func_80020D68;
    o->draw = func_80020DB8;
    o->cb18 = func_80020E6C;
    o->unk8 = 3;
    o->unkC = 2;
    o->unkE0 = arg2;
    o->unkA = 0;
    o->unk11C = -1;
    o->index = index;
    o->kind = kind;
    func_80038E90(o->unk134, -1, 4);
    func_80020874(o);
    o->unkEC = 1.0f;
    if (kind == 4) {
        func_800090C0(o);
    } else if (kind == 5) {
        func_800212A4(o);
    }
    func_80007EFC(o, D_80059DA8, 4, 0);
    D_80057DA8[index] = o;
    if (D_80052F18 == 2) {
        func_8002E7FC(o->unk1D8, o->kind);
    }
    func_8000E1D0(o);
    D_80057D58++;
}
