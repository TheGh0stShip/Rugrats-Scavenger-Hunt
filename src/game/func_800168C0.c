typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Node_80059DC0 {
    struct Node_80059DC0 *next;
    s32 unk4;
    s32 unk8;
    s32 unkC;
};
struct Owner_800168C0 {
    u8 pad_00[0x1C];
    s32 unk1C;
    struct Node_80059DC0 *head;
    struct Node_80059DC0 *tail;
};
extern struct Node_80059DC0 *D_80059DC0;
extern char D_800012E0[];
extern char D_80001328[];
extern void func_8001F480(char *);

void func_800168C0(s32 arg0, struct Owner_800168C0 *arg1, s32 arg2, s32 arg3) {
    struct Node_80059DC0 *node;

    if (arg1->unk1C == 0) {
        func_8001F480(D_800012E0);
        return;
    }
    node = D_80059DC0;
    if (node == (void *)0) {
        func_8001F480(D_80001328);
        return;
    }
    node->unk8 = arg2;
    node->unkC = arg3;
    node->unk4 = arg0;
    if (arg1->head == (void *)0) {
        arg1->head = node;
    } else {
        arg1->tail->next = node;
    }
    arg1->tail = D_80059DC0;
    D_80059DC0 = D_80059DC0->next;
    arg1->tail->next = (void *)0;
}

