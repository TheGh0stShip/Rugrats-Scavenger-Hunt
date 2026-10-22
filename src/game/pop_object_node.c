typedef struct ObjectNode ObjectNode;

struct ObjectNode {
    ObjectNode *next;
};

typedef struct {
    unsigned char pad[0x2C];
    ObjectNode *free_nodes;
} ObjectState;

extern ObjectState *D_80055AF4;

ObjectNode *func_80040BA8(void) {
    ObjectNode *node = D_80055AF4->free_nodes;

    if (node != 0) {
        D_80055AF4->free_nodes = node->next;
        node->next = 0;
    }
    return node;
}
