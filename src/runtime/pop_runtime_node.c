typedef struct RuntimeNode RuntimeNode;

struct RuntimeNode {
    RuntimeNode *next;
};

typedef struct {
    unsigned char pad[0x2C];
    RuntimeNode *nodes;
} RuntimeState;

extern RuntimeState *D_80056340;

RuntimeNode *func_8004CFCC(void) {
    RuntimeState *state = D_80056340;
    RuntimeNode *node = state->nodes;
    RuntimeNode *result = 0;

    if (node != 0) {
        result = node;
        state->nodes = result->next;
        result->next = 0;
    }
    return result;
}
