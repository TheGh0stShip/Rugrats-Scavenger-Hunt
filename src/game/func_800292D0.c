/* Target: func_800292D0  VRAM 0x800292D0  ROM 0x29ED0
 * Size: 0x44  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high
 * Unresolved: first argument unused; second argument passed as full word (no callee sign-extension), stored as halfword; actor struct shared with func_80029544 */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;

struct Actor_800055A4 {
    u8 pad_000[0x118];
    u32 flags;
    u8 pad_11C[0x3A];
    s16 unk156;
};

extern s32 D_800B9284;
extern struct Actor_800055A4 *func_800055A4(s32);
extern void func_8001A5C0(void);

void func_800292D0(s32 arg0, s32 arg1) {
    struct Actor_800055A4 *a = func_800055A4(D_800B9284);

    a->unk156 = arg1;
    a->flags |= 0x8000;
    func_8001A5C0();
}
