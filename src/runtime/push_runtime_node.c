typedef struct RuntimeNode RuntimeNode;

struct RuntimeNode {
    RuntimeNode *next;
};

typedef struct {
    unsigned char pad[0x2C];
    RuntimeNode *nodes;
} RuntimeState;

extern RuntimeState *D_80056340;

void func_8004CFF8(RuntimeNode *node) {
    node->next = D_80056340->nodes;
    D_80056340->nodes = node;
}
