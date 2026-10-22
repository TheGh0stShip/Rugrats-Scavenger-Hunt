typedef signed int s32;

void func_80026098(s32 *object, s32 index) {
    for (; index < 2; index++) {
        object[index + 114] = object[index + 115];
    }
    object[index + 114] = 0;
}
