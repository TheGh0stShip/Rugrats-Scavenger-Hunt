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

int func_8004FCC0(void *handle, IoMessage *message, int direction)
{
    register int result;

    if (!D_80057210.active) {
        return -1;
    }

    message->handle = handle;

    if (direction == 0) {
        message->type = 15;
    } else {
        message->type = 16;
    }

    if (message->priority == 1) {
        result = func_8004FD60(func_8004FEA0(), message, 0);
    } else {
        result = func_80048F00(func_8004FEA0(), message, 0);
    }

    return result;
}
