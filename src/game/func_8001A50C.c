typedef unsigned char u8;
typedef signed char s8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;

struct Node_8001A3EC {
    struct Node_8001A3EC *next; /* 0x00 */
    u8 pad_04[8];
    u32 tag;                    /* 0x0C */
};

struct Pool_8005B1E8 {
    u8 pad_00[0xC];
    struct Node_8001A3EC *head; /* 0x0C */
    u8 pad_10[0x10];
};

extern u32 D_8005B1E0;
extern struct Pool_8005B1E8 D_8005B1E8[];
extern void func_8001A3EC(struct Node_8001A3EC *);

void func_8001A50C(u32 tag) {
    u32 i;

    for (i = 0; i < D_8005B1E0; i++) {
        struct Node_8001A3EC *node = D_8005B1E8[i].head;
        while (node != 0) {
            struct Node_8001A3EC *next = node->next;
            if ((node->tag & ~0xF) == tag) {
                func_8001A3EC(node);
            }
            node = next;
        }
    }
}

