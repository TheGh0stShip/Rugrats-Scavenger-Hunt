extern int D_80057D5C;
struct O { char pad[0xF8]; int f; };
extern struct O *D_80057DA8[];
struct O *func_8002FEFC(int id) {
    int i;
    struct O *r;
    for (i = 0; i < D_80057D5C; i++) {
        if (D_80057DA8[i]->f == id) {
            r = D_80057DA8[i];
            goto out;
        }
    }
    r = 0;
out:
    __asm__("" : : "r"(r), "r"(i));
    return r;
}
