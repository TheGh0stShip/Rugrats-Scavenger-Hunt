typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;

struct Obj_80024480 {
    u8 pad_000[0x118];
    u32 unk118;
};

extern s32 D_8006FB8C;
extern void func_80024328(s32);
extern struct Obj_80024480 *func_80020680(void);
extern void func_80005500(s32, s32);
extern void func_8000550C(s32, s32);
extern void func_80005514(s32, s32);
extern void func_80005E3C(s32, s32);

void func_80024480(s32 arg0) {
    func_80024328(arg0);
    D_8006FB8C = arg0;
    func_80020680()->unk118 |= 0x100;
    func_80005500(8, arg0);
    func_8000550C(5, arg0);
    func_80005500(8, (s32)func_80020680());
    func_8000550C(5, (s32)func_80020680());
    func_80005514(arg0, 0);
    func_80005514((s32)func_80020680(), 0);
    func_80005E3C((s32)func_80020680(), arg0);
}
