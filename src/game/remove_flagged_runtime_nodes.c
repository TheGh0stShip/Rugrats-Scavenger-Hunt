typedef struct RuntimeNode {
    struct RuntimeNode *next;
    char pad4[8];
    unsigned int flags;
} RuntimeNode;

extern void func_8000809C(RuntimeNode *, RuntimeNode **);

void func_800081E0(RuntimeNode **list) {
    RuntimeNode *node;
    RuntimeNode *next;

    node = *list;
    while (node != 0) {
        next = node->next;
        if (node->flags & 1) {
            func_8000809C(node, list);
        }
        node = next;
    }
}
