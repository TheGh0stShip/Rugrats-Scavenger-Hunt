typedef unsigned int u32;

void func_8000550C(u32 value, void *object) {
    ((u32 *)object)[0x100 / 4] = value;
}
