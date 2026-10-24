/* Target: func_80012BBC  VRAM 0x80012BBC  ROM 0x137BC
 * Size: 0x110  Compiler: SN GCC 2.8 (-O2)
 * Confidence: low
 * Pipeline: kmc -O2 -G0 -mips3 -mgp32 -mfp32 */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Obj_80012BBC {
    u8 pad_00[0x104];
    s32 unk104;
    u8 pad_108[0x124 - 0x108];
    s32 unk124;
    s32 unk128;
    u8 pad_12C[0x13C - 0x12C];
    u8 unk13C;
    u8 unk13D;
};

extern u8 D_80058C0C[];

extern s32 func_80021C3C(struct Obj_80012BBC *, void *);
extern void func_80005500(s32, struct Obj_80012BBC *);
extern s16 func_8000DF14(s32, s32);
extern u16 func_8000DF40(s16);

void func_80012BBC(struct Obj_80012BBC *obj) {
    s32 d;

    obj->unk128 = func_80021C3C(obj, D_80058C0C);
    func_80005500(6, obj);
    d = obj->unk124 - obj->unk128;
    if (d == 0) {
        func_80005500(1, obj);
        if ((u16)(func_8000DF40(func_8000DF14(obj->unk13D, obj->unk13C)) - 0xF5) < 4) {
            func_80005500(0, obj);
        }
    } else {
        if (((d >= 0.0f) ? d : -d) == 180) {
            func_80005500(2, obj);
            obj->unk104 = 0x18;
        } else {
            if (d == 90 || d == -270) {
                func_80005500(2, obj);
            } else {
                func_80005500(3, obj);
            }
            obj->unk104 = 0xC;
        }
    }
}
