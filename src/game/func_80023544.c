typedef unsigned char u8;
typedef unsigned int u32;

struct Obj_80023544 {
    u8 pad_000[0x18C];
    u32 unk18C;
};

extern int func_8003A840(void);

void func_80023544(struct Obj_80023544 *obj)
{
    switch (func_8003A840() % 3) {
        case 0:
            obj->unk18C |= 0x400;
            break;
        case 1:
            obj->unk18C |= 0x1000;
            break;
        case 2:
            obj->unk18C |= 0x800;
            break;
    }
}
