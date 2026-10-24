typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

typedef struct PiHandle PiHandle;

typedef struct {
    s32 error_status;
    void *dram_address;
    void *c2_address;
    u32 sector_size;
    u8 rest[0x14];
} BlockInfo;

typedef struct {
    u32 command_type;
    u16 transfer_mode;
    u16 block_number;
    s32 sector_number;
    u8 pad_0C[4];
    u32 control_shadow;
    u8 pad_14[4];
    BlockInfo block[2];
} TransferInfo;

struct PiHandle {
    PiHandle *next;
    u8 type;
    u8 latency;
    u8 page_size;
    u8 rel_duration;
    u8 pulse;
    u8 domain;
    u8 pad_0A[2];
    u32 base_address;
    u32 speed;
    TransferInfo transfer_info;
};

typedef struct {
    u16 type;
    u8 priority;
    u8 pad_03;
    void *return_queue;
    void *dram_address;
    u32 device_address;
    u32 size;
    PiHandle *handle;
} IoMessage;

typedef struct {
    s32 active;
    void *thread;
    void *command_queue;
    void *event_queue;
    void *access_queue;
    s32 (*dma)(s32, u32, void *, u32);
    s32 (*edma)(PiHandle *, s32, u32, void *, u32);
} DeviceManager;

extern s32 func_80048DD0(void *queue, void *message, s32 flags);
extern s32 func_80048F00(void *queue, void *message, s32 flags);
extern void func_80050AF0(u32 mask);
extern void func_80050B40(u32 mask);
extern void func_80050B80(void);
extern s32 func_80050BD0(PiHandle *handle, u32 address, u32 *value);
extern s32 func_80050D60(PiHandle *handle, u32 address, u32 value);

void func_80050730(void *argument)
{
    IoMessage *message;
    void *event_message;
    void *dummy;
    s32 result;
    DeviceManager *manager;
    s32 send_second = 0;

    manager = argument;
    message = 0;
    result = 0;

    for (;;) {
        func_80048DD0(manager->command_queue, &message, 1);

        if (message->handle != 0 && message->handle->type == 2 &&
            (message->handle->transfer_info.command_type == 0 ||
             message->handle->transfer_info.command_type == 1)) {
            BlockInfo *block;
            TransferInfo *info;

            info = &message->handle->transfer_info;
            block = &info->block[info->block_number];
            info->sector_number = -1;

            if (info->transfer_mode != 3) {
                block->dram_address = (void *)((u32)block->dram_address - block->sector_size);
            }

            if (info->transfer_mode == 2 && message->handle->transfer_info.command_type == 0) {
                send_second = 1;
            } else {
                send_second = 0;
            }

            func_80048DD0(manager->access_queue, &dummy, 1);
            func_80050AF0(0x00100401);
            func_80050D60(message->handle, 0x05000510,
                          info->control_shadow | 0x80000000);

read_block:
            func_80048DD0(manager->event_queue, &event_message, 1);
            info = &message->handle->transfer_info;
            block = &info->block[info->block_number];

            if (block->error_status == 0x1D) {
                u32 status;

                func_80050D60(message->handle, 0x05000510,
                              info->control_shadow | 0x10000000);
                func_80050D60(message->handle, 0x05000510, info->control_shadow);
                func_80050BD0(message->handle, 0x05000508, &status);

                if (status & 0x02000000) {
                    func_80050D60(message->handle, 0x05000510,
                                  info->control_shadow | 0x01000000);
                }

                block->error_status = 4;
                *(volatile u32 *)0xA4600010 = 2;
                func_80050B40(0x00100C01);
            }

            func_80048F00(message->return_queue, message, 0);

            if (send_second == 1 &&
                message->handle->transfer_info.block[0].error_status == 0) {
                send_second = 0;
                goto read_block;
            }

            func_80048F00(manager->access_queue, 0, 0);
            if (message->handle->transfer_info.block_number == 1) {
                func_80050B80();
            }
        } else {
            switch (message->type) {
            case 11:
                func_80048DD0(manager->access_queue, &dummy, 1);
                result = manager->dma(0, message->device_address,
                                      message->dram_address, message->size);
                break;
            case 12:
                func_80048DD0(manager->access_queue, &dummy, 1);
                result = manager->dma(1, message->device_address,
                                      message->dram_address, message->size);
                break;
            case 15:
                func_80048DD0(manager->access_queue, &dummy, 1);
                result = manager->edma(message->handle, 0, message->device_address,
                                       message->dram_address, message->size);
                break;
            case 16:
                func_80048DD0(manager->access_queue, &dummy, 1);
                result = manager->edma(message->handle, 1, message->device_address,
                                       message->dram_address, message->size);
                break;
            case 10:
                func_80048F00(message->return_queue, message, 0);
                result = -1;
                break;
            default:
                result = -1;
                break;
            }

            if (result == 0) {
                func_80048DD0(manager->event_queue, &event_message, 1);
                func_80048F00(message->return_queue, message, 0);
                func_80048F00(manager->access_queue, 0, 0);
            }
        }
    }
}
