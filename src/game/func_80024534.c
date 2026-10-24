typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;

struct Obj_80024534 {
    u8 pad_000[0x118];
    u32 unk118;
    u32 unk11C;
    u8 pad_120[0x88];
    u8 unk1A8;
};

extern struct Obj_80024534 *D_8006FB78;
extern s32 func_80005568(struct Obj_80024534 *);
extern struct Obj_80024534 *func_800206A8(void);
extern void func_80005500(s32, struct Obj_80024534 *);
extern void func_80010E24(u32, s32);

void func_80024534(struct Obj_80024534 *obj) {
    D_8006FB78 = obj;
    if (func_80005568(obj)) {
        obj->unk118 |= 2;
        func_800206A8()->unk118 |= 2;
        obj->unk118 &= ~4;
        func_800206A8()->unk118 &= ~4;
        obj->unk1A8 = 0;
        func_80005500(11, obj);
        if (obj->unk11C < 4) {
            func_80010E24(obj->unk11C, 15);
        }
        return;
    }
    obj->unk118 |= 4;
    func_800206A8()->unk118 |= 4;
}
