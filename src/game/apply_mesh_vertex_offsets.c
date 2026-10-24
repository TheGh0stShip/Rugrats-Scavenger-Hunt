/* func_8000B71C: add per-vertex float offsets to mesh vertices, then copy shared vertices. */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

typedef struct {
    s16 x, y, z;
    u8 pad_06[0xA];
} Vtx16_8000B71C;

struct Mesh_8000B71C {
    u8 pad_00[0x14];
    u16 *groups;
    u8 pad_18[4];
    f32 *offsets;
};

void func_8000B71C(struct Mesh_8000B71C *mesh, Vtx16_8000B71C *out, Vtx16_8000B71C *in) {
    u16 *list = mesh->groups;
    f32 *off = mesh->offsets;
    s32 k = 0;
    s32 i;
    s32 n;

    while (*list != 0) {
        for (i = 0; i < *list; k++, i++) {
            out[k].x = in[k].x + *off++;
            out[k].y = in[k].y + *off++;
            out[k].z = in[k].z + *off++;
        }
        list++;
        n = *list++;
        if (n != 0) {
            for (i = 0; i < n; k++, i++, list++) {
                out[k].x = out[*list].x;
                out[k].y = out[*list].y;
                out[k].z = out[*list].z;
            }
        }
    }
}
