typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

struct Obj_80020050 {
    u8 pad_00[0x13C];
    u8 unk13C;
    u8 unk13D;
};

extern u8 D_8006D6A0[];
extern s32 D_8006F6B8;
extern s32 D_800539A0;
extern void *func_80038E90(void *, s32, s32);
extern s32 func_80025FC4(struct Obj_80020050 *, s32);
extern void func_8002012C(s32, s32, s32, s16, s32, u8 *, s32, s32);

void func_80020050(struct Obj_80020050 *obj, s32 a, s32 b, s32 count)
{
    s32 flag = 0;
    u8 buf[2];

    func_80038E90(D_8006D6A0, flag, 0x2000);
    D_8006F6B8 = 0;
    if (count == 0) {
        count = 5;
        flag = 1;
    }
    if (func_80025FC4(obj, 0x17A) != 0) {
        buf[0] = 0;
        buf[1] = 0;
    } else {
        buf[0] = a;
        buf[1] = b;
    }
    D_800539A0 = 0;
    func_8002012C(obj->unk13C, obj->unk13D, 0, count, flag, buf, buf[0], buf[1]);
}
