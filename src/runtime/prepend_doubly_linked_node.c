typedef struct Link Link;

struct Link {
    Link *next;
    Link *previous;
};

void func_8004CA2C(Link *node, Link **head) {
    node->next = *head;
    node->previous = (Link *)head;
    if (*head != 0) {
        (*head)->previous = node;
    }
    *head = node;
}
