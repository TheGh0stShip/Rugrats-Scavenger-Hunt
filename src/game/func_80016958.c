/* Target: func_80016958  VRAM 0x80016958  ROM 0x17558
 * Size: 0x9C  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: high */

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct List_80016958 {
    struct List_80016958 *next;
};
extern struct List_80016958 *func_80007FF8(struct List_80016958 *, u16, u16);
extern void func_800168C0(s32, void *, s32, s32);

void func_80016958(s32 arg0, struct List_80016958 **arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5) {
    struct List_80016958 *it;

    for (it = *arg1; it != (void *)0; it = it->next) {
        it = func_80007FF8(it, arg2, arg3);
        if (it == (void *)0) {
            break;
        }
        func_800168C0(arg0, it, arg4, arg5);
    }
}
