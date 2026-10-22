typedef signed int s32;

void func_8003DF80(s32 **words, s32 offset, s32 count) {
    s32 i;

    for (i = 0; i < count; i++) {
        if (words[i] != 0) {
            words[i] = (s32 *)((char *)words[i] + offset);
        }
    }
}
