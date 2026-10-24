/* Target: func_80031710  VRAM 0x80031710  ROM 0x32310
 * Size: 0x70  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: medium
 * Pipeline: kmc -O2 -G0 -mips3 -mgp32 -mfp32 */
typedef unsigned char u8;
typedef signed short s16;
typedef signed int s32;

struct Entry_80031710 {
    u8 pad_00[0x8];
    s16 unk8;            /* 0x08 */
    u8 pad_0A[0x6];
};                       /* size 0x10 */
struct Obj_80031710 {
    u8 pad_00[0xA];
    s16 count;           /* 0x0A */
    u8 pad_0C[0x4];
    struct Entry_80031710 entries[1]; /* 0x10 */
};
struct Big_800B9378 {
    u8 pad_0000[0xA044];
    s32 cur;             /* 0xA044 (=0x800C33BC) */
};
extern u8 D_800581E9;
extern struct Big_800B9378 D_800B9378;
extern struct Obj_80031710 *func_8000DD88(s32);
extern void func_8003187C(s32);

void func_80031710(void) {
    struct Obj_80031710 *obj;
    s32 i;
    obj = func_8000DD88(D_800581E9);
    for (i = 0; i < obj->count; i++) {
        if (obj->entries[i].unk8 >= 0) {
            D_800B9378.cur = i;
            func_8003187C(i);
            return;
        }
    }
}
