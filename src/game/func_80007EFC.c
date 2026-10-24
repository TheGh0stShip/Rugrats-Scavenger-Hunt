/* Target: func_80007EFC  VRAM 0x80007EFC  ROM 0x8AFC
 * Size: 0xFC  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: high */
typedef int s32;

struct Node_80007EFC {
    struct Node_80007EFC *next;
    struct Node_80007EFC *prev;
};
struct List_80007EFC {
    struct Node_80007EFC *head;
    struct Node_80007EFC *tail;
};

void func_80007EFC(struct Node_80007EFC *node, struct List_80007EFC *list, s32 mode, struct Node_80007EFC *ref) {
    switch (mode) {
    case 1:
        node->next = ref->next;
        node->prev = ref;
        ref->next = node;
        if (node->next != 0) {
            node->next->prev = node;
        }
        if (ref == list->tail) {
            list->tail = node;
        }
        break;
    case 2:
        node->next = ref;
        node->prev = ref->prev;
        ref->prev = node;
        if (node->prev != 0) {
            node->prev->next = node;
        }
        if (ref == list->head) {
            list->head = node;
        }
        break;
    case 3:
        node->next = list->head;
        node->prev = 0;
        if (list->tail == 0) {
            list->tail = node;
        }
        list->head = node;
        if (node->next != 0) {
            node->next->prev = node;
        }
        break;
    case 4:
        node->next = 0;
        node->prev = list->tail;
        if (list->tail != 0) {
            list->tail->next = node;
        }
        list->tail = node;
        if (list->head == 0) {
            list->head = node;
        }
        break;
    }
}
