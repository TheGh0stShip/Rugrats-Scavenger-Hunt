typedef struct Link Link;

struct Link {
    Link *next;
    Link *previous;
};

typedef struct {
    unsigned char pad_0[4];
    Link *available;
    unsigned char pad_8[0xC];
    Link *active;
} RuntimeLists;

extern void func_8004CA4C(Link *node);
extern void func_8004CA2C(Link *node, Link **head);

void func_8004D010(RuntimeLists *lists) {
    Link *node = lists->active;

    while (node != 0) {
        func_8004CA4C(node);
        func_8004CA2C(node, &lists->available);
        node = lists->active;
    }
}
