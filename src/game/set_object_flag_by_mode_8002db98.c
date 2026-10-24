extern int D_800529F8;
struct O { char pad[0x18C]; int f; };
void func_8002DB98(struct O *o) {
    int pad[2];
    if (D_800529F8 == 0) o->f |= 0x10; else o->f |= 4;
}
