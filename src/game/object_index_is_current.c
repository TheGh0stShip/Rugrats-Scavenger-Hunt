typedef unsigned char u8;
typedef signed int s32;

s32 func_80025FC4(u8 *object, s32 index) {
    object += index * 4;
    return *(s32 *)(object + 0x188) == index;
}
