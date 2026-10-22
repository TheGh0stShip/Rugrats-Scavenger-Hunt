typedef signed int s32;

s32 func_80026040(s32 *base, s32 index) {
    s32 *slot = (s32 *)((unsigned char *)base + index * 4 - 0x428);

    if (*slot != 0) {
        return 0;
    }
    *slot = index - 0x16C;
    return index - 0x16C;
}
