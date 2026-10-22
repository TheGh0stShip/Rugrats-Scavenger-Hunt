typedef unsigned int u32;

typedef struct {
    u32 active;
    void *thread;
    void *command_queue;
} DeviceManager;

extern DeviceManager D_80057210;

void *func_8004FEA0(void) {
    if (D_80057210.active != 0) {
        return D_80057210.command_queue;
    }
    return 0;
}
