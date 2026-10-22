typedef unsigned int u32;

typedef struct {
    u32 word_0;
    u32 flags;
    unsigned char pad_8[8];
    u32 state;
} Task;

extern void func_800520E0(void);

void func_80051A30(const unsigned char *object) {
    Task *task = *(Task *const *)(object + 0x274);

    if (task->state == 1) {
        task->flags |= 0x10;
        func_800520E0();
    }
}
