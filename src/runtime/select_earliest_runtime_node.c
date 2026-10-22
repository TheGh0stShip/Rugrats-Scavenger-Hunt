typedef signed int s32;

typedef struct RuntimeNode RuntimeNode;

struct RuntimeNode {
    RuntimeNode *next;
    unsigned char pad_4[0xC];
    s32 value;
};

typedef struct {
    RuntimeNode *head;
    unsigned char pad_4[0x1C];
    s32 origin;
} RuntimeState;

extern char D_800038E0[];
extern char D_800038E4[];
extern void func_8004B870(const char *file, const char *expression, int line);

s32 func_8004D138(RuntimeState *state, RuntimeNode **selected) {
    RuntimeNode *node;
    s32 best = 0x7FFFFFFF;

    if (state->head == 0) {
        func_8004B870(D_800038E0, D_800038E4, 0x133);
    }
    *selected = 0;
    node = state->head;
    while (node != 0) {
        if (node->value - state->origin < best) {
            *selected = node;
            best = node->value - state->origin;
        }
        node = node->next;
    }
    return (*selected)->value;
}
