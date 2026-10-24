/* func_8000CAB0  VRAM 0x8000CAB0  ROM 0xD6B0  Size: 0x200
 * Copy animation pair-table values into the output records.
 * SN GCC 2.8 -O2 with reviewed scheduling normalization. */
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef float f32;

typedef struct Pair_8000CAB0 {
    s32 a;
    s32 b;
} Pair_8000CAB0;

typedef struct Hdr_8000CAB0 {
    u8 pad_00[0x8];
    s32 flags;
    u8 pad_0C[0x4];
    Pair_8000CAB0 *pairs;   /* 0x10 */
    u16 *remap;             /* 0x14 */
} Hdr_8000CAB0;

typedef struct Res_8000CAB0 {
    u8 pad_00[0x18];
    Hdr_8000CAB0 *hdr;
} Res_8000CAB0;

typedef struct Obj_8000CAB0 {
    Res_8000CAB0 *res;
    u8 pad_04[0x2];
    s16 unk06;
    u8 pad_08[0x4];
    s16 unk0C;
} Obj_8000CAB0;

typedef struct Cmd_8000CAB0 {
    u16 dst;
    u16 type;
    s32 src;
} Cmd_8000CAB0;

typedef struct Out_8000CAB0 {
    s32 unk00;
    s32 unk04;
} Out_8000CAB0;

extern u16 *func_8000CD08(Res_8000CAB0 *res, s32 arg1);
extern u8 *func_8000CE50(Res_8000CAB0 *res, s32 arg1, s32 arg2);

void func_8000CAB0(Obj_8000CAB0 *obj, Out_8000CAB0 *out, s32 count, Cmd_8000CAB0 *cmds) {
    Res_8000CAB0 *res = obj->res;
    u16 i;
    s32 src;
    s32 k;

    if (res->hdr == 0 || !(res->hdr->flags & 2)) {
        return;
    }
    for (i = 0; i < count; i++) {
        switch (cmds[i].type) {
            case 14:
            case 15:
                if (cmds[i].src != -1) {
                    src = cmds[i].src;
                    if (*func_8000CD08(res, obj->unk06) != 0) {
                        k = *(func_8000CE50(res, obj->unk06, obj->unk0C) + src + 8);
                    } else {
                        k = res->hdr->remap[src];
                    }
                    out[cmds[i].dst].unk04 = res->hdr->pairs[k].a;
                }
                break;
            case 13:
            case 16:
                if (cmds[i].src != -1) {
                    src = cmds[i].src;
                    if (*func_8000CD08(res, obj->unk06) != 0) {
                        k = *(func_8000CE50(res, obj->unk06, obj->unk0C) + src + 8);
                    } else {
                        k = res->hdr->remap[src];
                    }
                    out[cmds[i].dst].unk04 = res->hdr->pairs[k].b;
                }
                break;
        }
    }
}
