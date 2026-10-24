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
    u32 c1_error_count;
    s32 c1_error_sector[4];
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

extern PiHandle *D_800C5B94;
extern u32 D_80056050;

extern s32 func_8004FAF0(PiHandle *handle, s32 direction, u32 device_address,
                         void *dram_address, u32 size);
extern void func_800505AC(void);
extern void func_8005067C(void);

s32 func_800500A0(void)
{
    u32 status = 0;
    volatile u32 pi_status;
    u32 manager_status;
    TransferInfo *info = &D_800C5B94->transfer_info;
    BlockInfo *block = &info->block[info->block_number];

    pi_status = *(volatile u32 *)0xA4600010;
    if (pi_status & 1) {
        D_80056050 &= ~0x800;
        block->error_status = 0x1D;
        func_8005067C();
        return 1;
    }

    pi_status = *(volatile u32 *)0xA4600010;
    while (pi_status & 3) {
        pi_status = *(volatile u32 *)0xA4600010;
    }
    status = *(volatile u32 *)0xA5000508;
    if (status & 0x02000000) {
        pi_status = *(volatile u32 *)0xA4600010;
        while (pi_status & 3) {
            pi_status = *(volatile u32 *)0xA4600010;
        }
        *(volatile u32 *)0xA5000510 = info->control_shadow | 0x01000000;
        block->error_status = 0;
        return 0;
    }

    if (info->command_type == 2) {
        return 1;
    }

    if (status & 0x08000000) {
        pi_status = *(volatile u32 *)0xA4600010;
        while (pi_status & 3) {
            pi_status = *(volatile u32 *)0xA4600010;
        }
        status = *(volatile u32 *)0xA5000508;
        block->error_status = 0x16;
        func_8005067C();
        *(volatile u32 *)0xA4600010 = 2;
        D_80056050 |= 0x00100401;
        return 1;
    }

    if (info->command_type == 1) {
        if ((status & 0x40000000) == 0) {
            if (info->sector_number + 1 != info->transfer_mode * 85) {
                block->error_status = 0x18;
                func_800505AC();
                return 1;
            }

            *(volatile u32 *)0xA4600010 = 2;
            D_80056050 |= 0x00100401;
            block->error_status = 0;
            func_8005067C();
            return 1;
        } else {
            block->dram_address = (void *)((u32)block->dram_address + block->sector_size);
            info->sector_number++;
            func_8004FAF0(D_800C5B94, 1, 0x05000400,
                          block->dram_address, block->sector_size);
            return 1;
        }
    } else if (info->command_type == 0) {
        if (info->transfer_mode == 3) {
            if (info->sector_number > (s32)block->c1_error_count + 17) {
                block->error_status = 0;
                func_800505AC();
                return 1;
            }

            if ((status & 0x40000000) == 0) {
                block->error_status = 0x17;
                func_800505AC();
                return 1;
            }
        } else {
            block->dram_address = (void *)((u32)block->dram_address + block->sector_size);
        }

        manager_status = *(volatile u32 *)0xA5000510;
        if (((manager_status & 0x00200000) && (manager_status & 0x00400000)) ||
            (manager_status & 0x02000000)) {
            if (block->c1_error_count > 3) {
                if (info->transfer_mode != 3 || info->sector_number > 0x52) {
                    block->error_status = 0x17;
                    func_800505AC();
                    return 1;
                }
            } else {
                s32 error_number = block->c1_error_count;
                block->c1_error_sector[error_number] = info->sector_number + 1;
            }

            block->c1_error_count++;
        }

        if (status & 0x10000000) {
            if (info->sector_number + 1 != 88) {
                block->error_status = 0x18;
                func_800505AC();
            }

            if (info->transfer_mode == 2 && info->block_number == 0) {
                info->block_number = 1;
                info->sector_number = -1;
                info->block[1].dram_address =
                    (void *)((u32)info->block[1].dram_address - info->block[1].sector_size);
                block->error_status = 0x16;
            } else {
                *(volatile u32 *)0xA4600010 = 2;
                D_80056050 |= 0x00100401;
                info->command_type = 2;
                block->error_status = 0;
            }

            func_8004FAF0(D_800C5B94, 0, 0x05000000,
                          block->c2_address, block->sector_size * 4);
            return 1;
        }

        if (info->sector_number == -1 && info->transfer_mode == 2 &&
            info->block_number == 1) {
            BlockInfo *first_block = &info->block[0];

            if (first_block->c1_error_count == 0) {
                if (((u32 *)first_block->c2_address)[0] |
                    ((u32 *)first_block->c2_address)[1] |
                    ((u32 *)first_block->c2_address)[2] |
                    ((u32 *)first_block->c2_address)[3]) {
                    first_block->error_status = 0x18;
                    func_800505AC();
                    return 1;
                }
            }

            first_block->error_status = 0;
            func_8005067C();
        }

        info->sector_number++;
        if (status & 0x40000000) {
            if (info->sector_number > 0x54) {
                block->error_status = 0x18;
                func_800505AC();
                return 1;
            }

            func_8004FAF0(D_800C5B94, 0, 0x05000400,
                          block->dram_address, block->sector_size);
            block->error_status = 0;
            return 1;
        } else if (info->sector_number <= 0x54) {
            block->error_status = 0x18;
            func_800505AC();
            return 1;
        }

        return 1;
    } else {
        block->error_status = 4;
        func_800505AC();
        return 1;
    }
}
