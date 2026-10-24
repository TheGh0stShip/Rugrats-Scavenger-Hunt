typedef unsigned char u8;
typedef signed int s32;

struct Node_80059DC0 {
    struct Node_80059DC0 *next;
};

struct Owner_800169F4 {
    u8 pad_00[0x20];
    struct Node_80059DC0 *head;
};

extern struct Node_80059DC0 *D_80059DC0;
extern char D_8000134C[];
extern void func_8001F480(char *);

void func_800169F4(struct Owner_800169F4 *arg0) {
    struct Node_80059DC0 *node = arg0->head;

    if (node == (void *)0) {
        func_8001F480(D_8000134C);
        return;
    }
    {
        struct Node_80059DC0 *next = node->next;
        struct Node_80059DC0 *free = D_80059DC0;
        D_80059DC0 = node;
        arg0->head = next;
        node->next = free;
    }
}

