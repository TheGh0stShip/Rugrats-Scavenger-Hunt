typedef unsigned char u8;
typedef int s32;

struct Unk_80024D14 {
    u8 pad_00[0x2C];
    s32 unk2C;
};

struct Obj_80024D14 {
    u8 pad_000[0xE0];
    s32 unkE0;
    u8 pad_0E4[0x74];
    s32 unk158;
};

extern s32 D_8006FBA0;
extern s32 D_80099B30;
extern void func_800069C8(s32, struct Obj_80024D14 *, s32);
extern struct Unk_80024D14 *func_800197C8(s32);
extern int func_8000CE08(void *, int);
extern void func_80005500(s32, struct Obj_80024D14 *);
extern void func_8000FDAC(s32, struct Obj_80024D14 *);

void func_80024D14(struct Obj_80024D14 *obj, s32 arg1, s32 arg2)
{
    D_8006FBA0 = arg2;
    func_800069C8(arg1, obj, 1);
    func_800069C8(0, obj, 2);
    obj->unk158 = D_80099B30 + func_8000CE08(func_800197C8(obj->unkE0)->unk2C, arg1);
    func_80005500(9, obj);
    func_8000FDAC(10, obj);
}
