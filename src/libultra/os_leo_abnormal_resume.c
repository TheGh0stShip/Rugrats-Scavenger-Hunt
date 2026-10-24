typedef unsigned char u8;
typedef unsigned int u32;

typedef struct PiHandle PiHandle;

typedef struct {
    u8 pad_00[0x10];
    u32 bm_ctl_shadow;
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

extern PiHandle *D_800C5B94;
extern u32 D_80056050;

extern void func_8005067C(void);

void func_800505AC(void)
{
    TransferInfo *info = &D_800C5B94->transfer_info;
    u32 pi_status;

    pi_status = *(volatile u32 *)0xA4600010;
    while (pi_status & 3) {
        pi_status = *(volatile u32 *)0xA4600010;
    }
    *(volatile u32 *)0xA5000510 = info->bm_ctl_shadow | 0x10000000;

    pi_status = *(volatile u32 *)0xA4600010;
    while (pi_status & 3) {
        pi_status = *(volatile u32 *)0xA4600010;
    }
    *(volatile u32 *)0xA5000510 = info->bm_ctl_shadow;

    func_8005067C();
    *(volatile u32 *)0xA4600010 = 2;
    D_80056050 |= 0x00100401;
}
