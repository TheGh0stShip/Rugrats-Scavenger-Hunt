/* Target: func_80023C40  VRAM 0x80023C40  ROM 0x24840
 * Size: 0x124  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: medium */
typedef signed char s8; typedef unsigned char u8; typedef short s16; typedef unsigned short u16; typedef int s32; typedef unsigned int u32;

struct Unk_80052F04 {
    u8 pad_00[0xF0];
    s32 unkF0;
};

struct Obj_80023C40 {
    u8 pad_000[0x118];
    u32 unk118;
    s32 unk11C;
};

extern s32 D_80052F18;
extern s32 D_800581C0[];
extern struct Unk_80052F04 *D_80052F04;
extern void func_8000DFD8(s32);
extern void func_80005500(s32, struct Obj_80023C40 *);
extern s32 func_80023D64(struct Obj_80023C40 *);
extern s32 func_80023F50(struct Obj_80023C40 *);
extern void func_80022A50(struct Obj_80023C40 *);
extern void func_8002DA20(struct Obj_80023C40 *);
extern s32 func_800166B0(struct Obj_80023C40 *);
extern void func_800247C8(struct Obj_80023C40 *, s32);

void func_80023C40(struct Obj_80023C40 *obj) {
    u32 flag = obj->unk118 & 0x80;
    s32 r;

    if (flag) {
        obj->unk118 &= ~0x80;
        if (flag && !(obj->unk118 & 8)) {
            if (D_800581C0[5] != 0) {
                if (D_800581C0[8] != 4) {
                    func_8000DFD8(6);
                }
                func_80005500(6, obj);
            } else {
                func_80005500(0, obj);
                D_80052F04->unkF0 = 1;
            }
            return;
        }
    }
    if (D_80052F18 == 2) {
        r = func_80023D64(obj);
    } else {
        r = func_80023F50(obj);
    }
    if (r != 0) {
        return;
    }
    if (obj->unk11C == -1) {
        if (D_80052F18 != 2) {
            func_80022A50(obj);
        } else {
            func_8002DA20(obj);
        }
    }
    func_800247C8(obj, func_800166B0(obj));
}
