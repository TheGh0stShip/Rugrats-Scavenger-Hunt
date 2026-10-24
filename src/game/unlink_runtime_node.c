typedef struct RuntimeNode {
    struct RuntimeNode *link0;
    struct RuntimeNode *link1;
    char pad8[0x10];
    void (*callback)(void);
} RuntimeNode;

typedef struct RuntimeList {
    RuntimeNode *link0;
    RuntimeNode *link1;
} RuntimeList;

void func_8000809C(RuntimeNode *node, RuntimeList *list) {
    if (node->link1 != 0) {
        node->link1->link0 = node->link0;
    }
    if (node->link0 != 0) {
        node->link0->link1 = node->link1;
    }
    if (node == list->link0) {
        list->link0 = node->link0;
    }
    if (node == list->link1) {
        list->link1 = node->link1;
    }
    if (node->callback != 0) {
        node->callback();
    }
}
