struct E { unsigned short pad, a, pad2, b; char rest[8]; };
struct H { unsigned short n; char pad[0x16]; struct E e[1]; };

int func_80032914(struct H *h) {
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
