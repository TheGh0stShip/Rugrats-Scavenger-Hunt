#define GU_ALIGN_LINKAGE static
#define func_80045CF0 make_align
#include "gu_align_f.c"

typedef unsigned long long u64;

typedef union Mtx {
    u64 force_structure_alignment;
    char bytes[64];
} Mtx;

extern void func_800468F0(f32 source[4][4], Mtx *destination);

void func_80045EBC(Mtx *matrix, f32 angle, f32 x, f32 y, f32 z)
{
    f32 temporary[4][4];

    make_align(temporary, angle, x, y, z);
    func_800468F0(temporary, matrix);
}
