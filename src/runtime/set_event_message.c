typedef unsigned int u32;

typedef struct EventState EventState;

struct EventState {
    void *queue;
    void *message;
};

extern EventState D_800C44D0[];
extern u32 func_80048900(void);
extern void func_80048920(u32 mask);

void func_80049030(int event, void *queue, void *message)
{
    register u32 saved_mask;
    EventState *state;

    saved_mask = func_80048900();
    state = &D_800C44D0[event];
    state->queue = queue;
    state->message = message;
    func_80048920(saved_mask);
}
