typedef struct RuntimeNode {
    struct RuntimeNode *next;
    char pad4[0xC];
    void (*callback)(struct RuntimeNode *);
} RuntimeNode;

extern int D_80052F14;

void func_80008118(RuntimeNode **list) {
    RuntimeNode *node = *list;

    if (D_80052F14 < 0) {
        while (node != 0) {
            if (node->callback != 0) {
                node->callback(node);
            }
            node = node->next;
        }
    }
}
