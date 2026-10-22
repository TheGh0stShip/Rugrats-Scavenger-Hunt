typedef unsigned int u32;

void func_80005514(void *object, u32 value) {
    ((u32 *)object)[0x108 / 4] = value;
}
