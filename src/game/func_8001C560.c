typedef unsigned char u8;
typedef int s32;

struct Obj_80005544 {
    u8 pad_00[0xF8];
    s32 state;
};

extern struct Obj_80005544 *func_80005544(void);
extern struct Obj_80005544 *D_8006FB8C;

struct Obj_80005544 *func_8001C560(void)
{
    struct Obj_80005544 *obj = func_80005544();

    if (obj->state == 5) {
        obj = D_8006FB8C;
    }
    return obj;
}
