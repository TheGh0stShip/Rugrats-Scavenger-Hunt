typedef struct Link Link;

struct Link {
    Link *next;
    Link *previous;
};

void func_8004CA4C(Link *node) {
    if (node->next != 0) {
        node->next->previous = node->previous;
    }
    if (node->previous != 0) {
        node->previous->next = node->next;
    }
}

__asm__("nop\n\tnop\n\tnop");
