typedef struct node_s {
    struct node_s *prev;
    struct node_s *next;
    int count;
} node_t;

extern int D_800C4088;
extern char D_800C4090[];
extern node_t *D_800C4070;
extern node_t *D_800C4074;
extern int func_80048DD0(void *mq, void *msg, int flags);

void func_800427C4(void)
{
    void *msg;
    node_t *node;
    node_t *next;

    while (D_800C4088) {
        func_80048DD0(D_800C4090, &msg, 0);
        D_800C4088--;
    }
    node = D_800C4070;
    while (node) {
        if (--node->count == 0) {
            next = node->next;
            if (next)
                next->prev = node->prev;
            if (node->prev)
                node->prev->next = node->next;
            else
                D_800C4070 = node->next;
            node->prev = 0;
            node->next = D_800C4074;
            D_800C4074 = node;
            node = next;
        } else {
            node = node->next;
        }
    }
}
