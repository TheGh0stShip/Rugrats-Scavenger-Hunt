typedef int s32;
typedef unsigned int u32;

s32 func_8003AEA0(signed char *s) {
    signed char *start = s;
    u32 w;

    while (((u32)s & 3) != 0) {
        if (*s == 0) {
            return s - start;
        }
        s++;
    }
    for (;;) {
        w = *(u32 *)s;
        if ((w & 0xFF000000) == 0) {
            return s - start;
        }
        if ((w & 0x00FF0000) == 0) {
            return (s + 1) - start;
        }
        if ((w & 0x0000FF00) == 0) {
            return (s + 2) - start;
        }
        if ((w & 0x000000FF) == 0) {
            return (s + 3) - start;
        }
        s += 4;
    }
}
