/* Target: func_80029544  VRAM 0x80029544  ROM 0x2A144
 * Size: 0x3C  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high
 * Unresolved: none significant */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;

struct Actor_800055A4 {
    u8 pad_000[0x118];
    u32 flags;
};

extern s32 D_800B9284;
extern struct Actor_800055A4 *func_800055A4(s32);
extern void func_8001A5C0(void);

void func_80029544(void) {
    struct Actor_800055A4 *a = func_800055A4(D_800B9284);

    a->flags |= 0x20000;
    func_8001A5C0();
}
