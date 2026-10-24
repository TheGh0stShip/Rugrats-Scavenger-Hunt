typedef unsigned char u8;
typedef unsigned int u32;

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
    u32 speed;
    u8 transfer_info[0x60];
};

extern PiHandle D_800C5B20;
extern PiHandle *D_8005722C;
extern PiHandle *D_800C5B94;

extern void func_80051050(void *address, int size);
extern u32 func_80048900(void);
extern void func_80048920(u32 mask);

PiHandle *func_8004FFA0(void)
{
    u32 save_mask;

    D_800C5B20.type = 2;
    D_800C5B20.base_address = 0xA5000000;
    D_800C5B20.latency = 3;
    D_800C5B20.pulse = 6;
    D_800C5B20.page_size = 6;
    D_800C5B20.rel_duration = 2;
    D_800C5B20.domain = 1;

    *(volatile u32 *)0xA4600024 = D_800C5B20.latency;
    *(volatile u32 *)0xA4600028 = D_800C5B20.pulse;
    *(volatile u32 *)0xA460002C = D_800C5B20.page_size;
    *(volatile u32 *)0xA4600030 = D_800C5B20.rel_duration;
    D_800C5B20.speed = 0;

    func_80051050(D_800C5B20.transfer_info, 0x60);

    save_mask = func_80048900();
    D_800C5B20.next = D_8005722C;
    D_8005722C = &D_800C5B20;
    D_800C5B94 = &D_800C5B20;
    func_80048920(save_mask);

    return &D_800C5B20;
}
