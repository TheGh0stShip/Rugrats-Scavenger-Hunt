typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;

typedef struct PiHandle PiHandle;

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
};

extern PiHandle *D_80057230[2];
extern char D_80003990;
extern char D_80003994;

extern void func_8004B870(char *expression, char *file, s32 line);

s32 func_80050BD0(PiHandle *handle, u32 device_address, u32 *value)
{
    register u32 status;
    register u32 domain;
    u32 address;
    PiHandle *current;

    if (value == 0) {
        func_8004B870(&D_80003990, &D_80003994, 43);
    }

    status = *(volatile u32 *)0xA4600010;
    while (status & 3) {
        status = *(volatile u32 *)0xA4600010;
    }

    domain = handle->domain;
    current = D_80057230[domain];
    if (current != handle) {
        if (domain == 0) {
            if (current->latency != handle->latency) {
                *(volatile u32 *)0xA4600014 = handle->latency;
            }
            if (current->page_size != handle->page_size) {
                *(volatile u32 *)0xA460001C = handle->page_size;
            }
            if (current->rel_duration != handle->rel_duration) {
                *(volatile u32 *)0xA4600020 = handle->rel_duration;
            }
            if (current->pulse != handle->pulse) {
                *(volatile u32 *)0xA4600018 = handle->pulse;
            }
        } else {
            if (current->latency != handle->latency) {
                *(volatile u32 *)0xA4600024 = handle->latency;
            }
            if (current->page_size != handle->page_size) {
                *(volatile u32 *)0xA460002C = handle->page_size;
            }
            if (current->rel_duration != handle->rel_duration) {
                *(volatile u32 *)0xA4600030 = handle->rel_duration;
            }
            if (current->pulse != handle->pulse) {
                *(volatile u32 *)0xA4600028 = handle->pulse;
            }
        }
        D_80057230[domain] = handle;
    }

    address = handle->base_address | device_address;
    *value = *(volatile u32 *)(address | 0xA0000000);
    return 0;
}
