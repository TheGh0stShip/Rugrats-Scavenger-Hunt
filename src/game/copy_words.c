typedef unsigned int u32;
void func_8001E034(u32 *destination, const u32 *source, u32 count) {
    u32 i;
    for (i = 0; i < count; i++) *destination++ = *source++;
}
