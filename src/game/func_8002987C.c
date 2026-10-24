/* Target: func_8002987C  VRAM 0x8002987C  ROM 0x2A47C
 * Size: 0x12C  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: high */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Obj_8002987C {
    u8 pad_000[0x1F8];
    s16 unk1F8;
};

struct Obj_800B9290 {
    u8 pad_00[0x8];
    s32 unk8;
};

extern char D_80002178[];
extern char D_80002190[];
extern char D_800021A4[];
extern char D_800021A8[];
extern char D_800021AC[];
extern char D_800021C4[];
extern char *D_80052FD0[];
extern struct Obj_800B9290 D_800B9290;
extern void func_800139F8(char *, s32, s32);
extern int func_8003A57C(char *, char *, ...);
extern void func_8001380C(s32, char *, s32, s32);

s32 func_8002987C(struct Obj_8002987C *obj, s32 arg1, u32 arg2) {
    char buf[0x28];
    s32 ret = 0;

    if (obj->unk1F8 >= 5) {
        func_800139F8(D_80002178, 0x100, 0x8C);
        func_8003A57C(buf, D_80002190, D_80052FD0[arg1]);
        func_800139F8(buf, 0x100, 0xA0);
        func_8001380C(0x38, D_800021A4, 100, 200);
        func_8001380C(0x37, D_800021A8, 300, 200);
        if (arg2 & 0x10) {
            ret = 2;
        } else if (arg2 & 0x40) {
            ret = 1;
        }
    } else {
        func_800139F8(D_800021AC, 0x100, 0x8C);
        func_800139F8(D_800021C4, 0x100, 0xA0);
        if (--D_800B9290.unk8 == 0) {
            ret = 1;
        }
    }
    return ret;
}
