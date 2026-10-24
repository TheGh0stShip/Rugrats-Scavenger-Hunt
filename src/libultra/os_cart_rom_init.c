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

extern PiHandle D_800C5AA0;
extern PiHandle *D_8005722C;

extern int func_8004F7D0(void *address, u32 *value);
extern void func_80051050(void *address, int size);
extern u32 func_80048900(void);
extern void func_80048920(u32 mask);

PiHandle *func_8004FEC0(void)
{
    u32 domain = 0;
    u32 save_mask;

    if (D_800C5AA0.base_address == 0xB0000000) {
        return &D_800C5AA0;
    }

    D_800C5AA0.type = 0;
    D_800C5AA0.base_address = 0xB0000000;
    func_8004F7D0(0, &domain);
    D_800C5AA0.latency = domain & 0xFF;
    D_800C5AA0.pulse = (domain >> 8) & 0xFF;
    D_800C5AA0.page_size = (domain >> 16) & 0xF;
    D_800C5AA0.rel_duration = (domain >> 20) & 0xF;
    D_800C5AA0.domain = 0;
    D_800C5AA0.speed = 0;
    func_80051050(D_800C5AA0.transfer_info, 0x60);

    save_mask = func_80048900();
    D_800C5AA0.next = D_8005722C;
    D_8005722C = &D_800C5AA0;
    func_80048920(save_mask);

    return &D_800C5AA0;
}
