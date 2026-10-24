typedef struct { char pad[0xF4]; int f4; char pad2[0x1AC-0xF8]; unsigned int flags[4]; } Obj;
extern int D_80057D5C;
extern int func_800093AC(void);
extern Obj *func_800055A4(int);
extern int func_80025FDC(Obj *);
extern void func_80009398(int, unsigned int);
int func_80009AF4(void) {
    int found = 0;
    int i, j;
    Obj *o;
    unsigned int f;
    if (func_800093AC() == 0) {
    for (i = 0; i < D_80057D5C; i++) {
        o = func_800055A4(i);
        if ((unsigned)(func_80025FDC(o) - 1) < 3) {
            for (j = 0; j < 4; j++) {
                f = o->flags[j]; if (!(f & 0x80000000)) { found = 1; func_80009398(o->f4, f); o->flags[j] |= 0x80000000; break; }
            }
            if (found) break;
        }
    }
    }
    return found;
}
