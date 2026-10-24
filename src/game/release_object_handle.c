typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;
extern void func_8002621C(s32 handle, s32 dir);
extern void func_8001A3EC(u8 *obj);
void func_800114CC(u8 *obj) {
    if (*(s32 *)(obj + 0xE0) != 0) {
        if (*(u16 *)(obj + 0xA) == 1) {
            func_8002621C(*(s32 *)(obj + 0xE0), -1);
        } else {
            func_8002621C(*(s32 *)(obj + 0xE0), 1);
        }
    }
    func_8001A3EC(obj);
}
