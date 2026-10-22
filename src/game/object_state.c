typedef unsigned int u32;

void func_80005500(u32 value, void *object) {
    u32 *words = (u32 *)object;

    words[0xFC / 4] = value;
    words[0x100 / 4] = 0;
}
