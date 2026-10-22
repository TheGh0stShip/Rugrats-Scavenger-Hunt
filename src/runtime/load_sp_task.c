typedef unsigned int u32;

typedef struct {
    u32 type;
    u32 flags;
    void *ucode_boot;
    u32 ucode_boot_size;
    void *ucode;
    u32 ucode_size;
    void *ucode_data;
    u32 ucode_data_size;
    void *dram_stack;
    u32 dram_stack_size;
    void *output_buffer;
    void *output_buffer_size;
    void *data;
    u32 data_size;
    void *yield_data;
    u32 yield_data_size;
} SpTask;

extern void func_80052680(void *source, void *destination, u32 size);
extern u32 func_800495D0(void *address);
extern void func_80047E00(void *address, u32 size);
extern void func_80051FA0(u32 status);
extern int func_80051FB0(u32 address);
extern int func_80051FE0(int direction, u32 device_address,
                         void *dram_address, u32 size);
extern int func_80051F80(void);

static SpTask temporary_task;

extern __inline__ SpTask *physical_task(SpTask *input)
{
    SpTask *task;

    task = &temporary_task;
    func_80052680(input, task, sizeof(SpTask));

    if (task->ucode != 0) {
        task->ucode = (void *)func_800495D0(task->ucode);
    }
    if (task->ucode_data != 0) {
        task->ucode_data = (void *)func_800495D0(task->ucode_data);
    }
    if (task->dram_stack != 0) {
        task->dram_stack = (void *)func_800495D0(task->dram_stack);
    }
    if (task->output_buffer != 0) {
        task->output_buffer = (void *)func_800495D0(task->output_buffer);
    }
    if (task->output_buffer_size != 0) {
        task->output_buffer_size =
            (void *)func_800495D0(task->output_buffer_size);
    }
    if (task->data != 0) {
        task->data = (void *)func_800495D0(task->data);
    }
    if (task->yield_data != 0) {
        task->yield_data = (void *)func_800495D0(task->yield_data);
    }

    return task;
}

void func_80051D40(SpTask *input)
{
    SpTask *task;

    task = physical_task(input);

    if (task->flags & 1) {
        task->ucode_data = task->yield_data;
        task->ucode_data_size = task->yield_data_size;
        input->flags &= ~1;
        if (task->flags & 4) {
            task->ucode = *(void **)(0xA0000000 |
                                     ((u32)input->yield_data + 0xBFC));
        }
    }

    func_80047E00(task, sizeof(SpTask));
    func_80051FA0(0x2B00);

    while (func_80051FB0(0x04001000) == -1) {
    }

    while (func_80051FE0(1, 0x04000FC0, task, sizeof(SpTask)) == -1) {
    }

    while (func_80051F80()) {
    }

    while (func_80051FE0(1, 0x04001000, task->ucode_boot,
                         task->ucode_boot_size) == -1) {
    }
}

void func_80051F4C(void)
{
    while (func_80051F80()) {
    }

    func_80051FA0(0x125);
}
