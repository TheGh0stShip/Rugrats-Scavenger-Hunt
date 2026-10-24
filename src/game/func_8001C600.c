typedef unsigned char u8;
typedef signed char s8;
typedef signed int s32;

struct Obj_80005544 {
    u8 pad_00[0xF8];
    s32 state;  /* 0xF8 */
    u8 pad_FC[0xAA];
    s8 unk1A6;  /* 0x1A6 */
};

extern struct Obj_80005544 *D_8006FB78;
extern struct Obj_80005544 *func_80005544(void);
extern s32 *func_8001BF60(s32);
extern void func_80026294(struct Obj_80005544 *, s32);
extern void func_8001BED8(s32);
extern void func_8001BF24(s32);
extern void func_8001A5C0(void);

void func_8001C600(void) {
    struct Obj_80005544 *obj = func_80005544();

    if (obj->state == 6) {
        obj = D_8006FB78;
    }
    if (*func_8001BF60(0) != 0) {
        if (--*func_8001BF60(0) != 0) {
            return;
        }
    }
    if (*func_8001BF60(1) + obj->unk1A6 >= 30) {
        *func_8001BF60(1) = 30 - obj->unk1A6;
    }
    if (*func_8001BF60(1) != 0) {
        func_80026294(obj, 1);
        (*func_8001BF60(1))--;
        *func_8001BF60(0) = 6;
    } else if (*func_8001BF60(2) != 0) {
        func_8001BED8(4);
        func_8001BF24(0);
    } else {
        func_8001A5C0();
    }
}

