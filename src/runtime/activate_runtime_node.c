typedef struct Link Link;

struct Link {
    Link *next;
    Link *previous;
};

typedef struct {
    unsigned char pad[0x14];
    Link *active;
} RuntimeLists;

extern void func_8004CA4C(Link *node);
extern void func_8004CA2C(Link *node, Link **head);

void func_8004D064(RuntimeLists *lists, Link *node) {
    func_8004CA4C(node);
    func_8004CA2C(node, &lists->active);
}
