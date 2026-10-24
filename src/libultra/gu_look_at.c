#define GU_LOOK_AT_LINKAGE static
#define func_800461E0 make_look_at
#include "gu_look_at_f.c"

typedef unsigned long long u64;

typedef union Mtx {
    u64 force_structure_alignment;
    char bytes[64];
} Mtx;

extern void func_800468F0(f32 source[4][4], Mtx *destination);

void func_80046560(Mtx *matrix, f32 eye_x, f32 eye_y, f32 eye_z,
                   f32 at_x, f32 at_y, f32 at_z,
                   f32 up_x, f32 up_y, f32 up_z)
{
    f32 temporary[4][4];

    make_look_at(temporary, eye_x, eye_y, eye_z,
                 at_x, at_y, at_z, up_x, up_y, up_z);
    func_800468F0(temporary, matrix);
}
