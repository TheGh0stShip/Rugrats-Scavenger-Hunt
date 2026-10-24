struct E { unsigned short a, pad, b, pad2; char rest[8]; };
struct H { unsigned short n; char pad[0x16]; struct E e[1]; };

int func_800328CC(struct H *h) {
    int i, m;
    struct E *e;
    i = 0;
    m = 0;
    e = h->e;
    for (; i < h->n; i++) {
        int v = e[i].b + e[i].a;
        if (m < v) m = v;
    }
    return m;
}
