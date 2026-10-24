typedef struct RuntimeNode {
    struct RuntimeNode *next;
    char pad4[0x10];
    void (*callback)(struct RuntimeNode *);
} RuntimeNode;

extern int D_80052F14;
extern void func_80022844(void);

void func_80008174(RuntimeNode **list) {
    RuntimeNode *node = *list;

    if (D_80052F14 >= 0) {
        func_80022844();
    } else {
        while (node != 0) {
            if (node->callback != 0) {
                node->callback(node);
            }
            node = node->next;
        }
    }
}
