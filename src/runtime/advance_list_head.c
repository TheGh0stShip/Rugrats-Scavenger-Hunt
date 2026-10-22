typedef struct ListNode ListNode;

struct ListNode {
    ListNode *next;
};

ListNode *func_80048764(ListNode **head) {
    ListNode *node = *head;
    register ListNode *next __asm__("$25") = node->next;
    __asm__("" : "=r"(next) : "0"(next));
    *head = next;
    return node;
}
