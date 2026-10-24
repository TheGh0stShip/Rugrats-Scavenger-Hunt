/* Target: func_8001AB7C  VRAM 0x8001AB7C  ROM 0x1B77C
 * Size: 0x4C  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high
 * Pipeline: kmc -O2 -G0 -mips3 -mgp32 -mfp32 */
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;

typedef struct { s32 v[3]; } Triple;

struct Entry_8005368C {
    Triple cur;   /* 0x00 */
    Triple on;    /* 0x0C */
    Triple off;   /* 0x18 */
    u8 pad_24[0x18];
};

extern struct Entry_8005368C D_8005368C[5];

void func_8001AB7C(s32 sel) {
    s32 i;
    struct Entry_8005368C *e = D_8005368C;

    for (i = 0; i < 5; i++, e++) {
        Triple *src;
        if (i == sel) {
            src = &e->on;
        } else {
            src = &e->off;
        }
        e->cur = *src;
    }
}
