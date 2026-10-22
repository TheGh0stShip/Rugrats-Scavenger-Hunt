typedef struct ListNode ListNode;

struct ListNode {
    ListNode *next;
};

void func_80048764(ListNode **head) {
    *head = (*head)->next;
}
