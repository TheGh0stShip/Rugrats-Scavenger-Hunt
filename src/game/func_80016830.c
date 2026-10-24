/* Target: func_80016830  VRAM 0x80016830  ROM 0x17430
 * Size: 0x90  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: high */

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Node_80059DC0 {
    struct Node_80059DC0 *next;
    s32 unk4;
    s32 unk8;
    s32 unkC;
};
extern struct Node_80059DC0 *D_80059DC0;
extern struct Node_80059DC0 *D_80059DC4;
extern u32 D_80059DC8;
extern void *func_8001A2B0(u32);

void func_80016830(u32 count) {
    struct Node_80059DC0 *pool;
    u32 i;
    u32 n;

    D_80059DC8 = count;
    pool = func_8001A2B0(count << 4);
    D_80059DC4 = pool;
    D_80059DC0 = pool;
    n = D_80059DC8 - 1;
    for (i = 0; i < n; i++) {
        pool[i].next = &pool[i + 1];
    }
    D_80059DC4[i].next = (void *)0;
}
