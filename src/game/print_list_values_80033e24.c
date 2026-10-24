struct L { struct L *next; int pad; unsigned short v; };
extern char D_80002BD4[];
extern void func_800510F0(char *, int);
void func_80033E24(struct L **head) {
    struct L *p;
    for (p = *head; p != 0; p = p->next) {
        func_800510F0(D_80002BD4, p->v);
    }
}
