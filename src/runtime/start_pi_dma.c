typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct {
    u16 type;
    u8 priority;
    u8 pad_03;
    void *return_queue;
    void *dram_address;
    u32 device_address;
    u32 size;
    void *handle;
} IoMessage;

typedef struct {
    int active;
} DeviceManager;

extern DeviceManager D_80057210;
extern void *func_8004FEA0(void);
extern int func_8004FD60(void *queue, void *message, int flags);
extern int func_80048F00(void *queue, void *message, int flags);

int func_80050FA0(IoMessage *message, int priority, int direction,
                   u32 device_address, void *dram_address, u32 size,
                   void *return_queue)
{
    register int result;

    if (!D_80057210.active) {
        return -1;
    }

    if (direction == 0) {
        message->type = 11;
    } else {
        message->type = 12;
    }

    message->priority = priority;
    message->return_queue = return_queue;
    message->dram_address = dram_address;
    message->device_address = device_address;
    message->size = size;
    message->handle = 0;

    if (priority == 1) {
        result = func_8004FD60(func_8004FEA0(), message, 0);
    } else {
        result = func_80048F00(func_8004FEA0(), message, 0);
    }

    return result;
}
