typedef struct ObjectNode ObjectNode;

struct ObjectNode {
    ObjectNode *next;
};

typedef struct {
    unsigned char pad[0x2C];
    ObjectNode *free_nodes;
} ObjectState;

extern ObjectState *D_80055AF4;

void func_80040BD4(ObjectNode *node) {
    node->next = D_80055AF4->free_nodes;
    D_80055AF4->free_nodes = node;
}
