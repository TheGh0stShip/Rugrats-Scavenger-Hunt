typedef int s32;
typedef void *(*DmaNew)(void **state);

typedef struct LoadFilter {
    char filter[0x14];
    void *state;
    void *loop_state;
    char reserved_1c[0x14];
    void *dma;
    void *dma_state;
    char reserved_38[4];
    s32 last_sample;
    s32 first;
    s32 memory_input;
} LoadFilter;

extern void func_8004B880(void *filter, void *handler, void *set_param, s32 type);
extern void *func_8004CAC0(s32 unused_0, s32 unused_1, void *heap,
                           s32 count, s32 size);
extern void func_80049F20(void);
extern void func_8004A700(void);

void func_80049CE4(LoadFilter *filter, DmaNew dma_new, void *heap) {
    func_8004B880(filter, func_80049F20, func_8004A700, 0);
    filter->state = func_8004CAC0(0, 0, heap, 1, 0x20);
    filter->loop_state = func_8004CAC0(0, 0, heap, 1, 0x20);
    filter->dma = dma_new(&filter->dma_state);
    filter->last_sample = 0;
    filter->first = 1;
    filter->memory_input = 0;
}
