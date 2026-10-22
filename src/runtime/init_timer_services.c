typedef unsigned int u32;
typedef unsigned long long u64;

typedef struct Timer Timer;

struct Timer {
    Timer *next;
    Timer *previous;
    u64 interval;
    u64 value;
    void *queue;
    void *message;
};

extern u64 D_800C4558;
extern u32 D_800C4550;
extern u32 D_800C4554;
extern Timer *D_80056080;

void func_800492C0(void)
{
    D_800C4558 = 0;
    D_800C4550 = 0;
    D_800C4554 = 0;
    D_80056080->next = D_80056080->previous = D_80056080;
    D_80056080->interval = D_80056080->value = 0;
    D_80056080->queue = 0;
    D_80056080->message = 0;
}
