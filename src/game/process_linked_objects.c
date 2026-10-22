typedef struct Node { struct Node *next; } Node;
extern void func_8000809C(Node *, Node **);

void func_80008240(Node **head) {
    Node *node = *head;
    while (node != 0) {
        func_8000809C(node, head);
        node = node->next;
    }
}
