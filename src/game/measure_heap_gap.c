typedef unsigned int u32;
typedef signed int s32;

typedef struct HeapNode {
    u32 unused0;
    u32 unused4;
    struct HeapNode *next;
} HeapNode;

typedef struct HeapState {
    u32 unused0;
    u32 end;
    u32 unused8;
    HeapNode *head;
    u32 unused10;
    u32 largest_gap;
    u32 scans;
} HeapState;

extern HeapState D_8005B1E8;

s32 func_8001A250(s32 unused) {
    HeapNode *node = D_8005B1E8.head;

    D_8005B1E8.largest_gap = 0;
    if (node != 0) {
        HeapState *state = &D_8005B1E8;

        __asm__("" : : "r"(unused));
        do {
            register u32 end __asm__("$2") = (u32)node->next;
            u32 gap;

            if (end != 0) {
                end -= 0x10;
            } else {
                end = state->end;
                end -= 0x10;
            }
            gap = end - (u32)node;
            __asm__("" : "+r"(gap));
            state->scans++;
            if (state->largest_gap < gap) {
                state->largest_gap = gap;
            }
            node = node->next;
        } while (node != 0);
    }
    return 0;
}
