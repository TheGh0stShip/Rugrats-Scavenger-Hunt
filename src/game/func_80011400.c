typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;
typedef double f64;

struct Obj_80011400 {
    u8 pad_00[0x2C];
    f32 unk_2C;
    f32 unk_30;
    f32 unk_34;
    u8 unk_38[0x60 - 0x38];
    u8 unk_60[0xA0 - 0x60];
    u8 unk_A0[0xC4 - 0xA0];
    s32 unk_C4;
    s32 unk_C8;
    u8 pad_CC[0xD4 - 0xCC];
    s32 unk_D4;
};

extern void func_8001151C(struct Obj_80011400 *obj);
extern void func_80047A38(void *m, f32 a, f32 b, f32 c, f32 d, f32 x, f32 y, f32 z);
extern void * func_8000BDFC(void *, float, float, float, void *, void *, int);

void func_80011400(struct Obj_80011400 *obj) {
    struct Obj_80011400 *o = obj;

    obj->unk_C8--;
    func_8001151C(obj);
    if (obj->unk_C4 == 1) {
        return;
    }
    if (obj->unk_C4 == 3) {
        obj->unk_D4 += 0x24;
    }
    func_80047A38(o->unk_60, 45.0f, (f32)o->unk_D4, 0.0f, 1.0f,
                  o->unk_2C, o->unk_30, o->unk_34);
    func_8000BDFC(o->unk_A0, o->unk_2C, o->unk_30, o->unk_34, o->unk_38, o->unk_60, 0);
}
