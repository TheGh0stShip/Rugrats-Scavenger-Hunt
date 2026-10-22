typedef signed int s32;

s32 func_80026064(s32 *object, s32 value) {
    s32 i;

    for (i = 0; i < 3; i++, object++) {
        if (object[114] == 0) {
            object[114] = value;
            return 1;
        }
    }
    return 0;
}
