typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct MessageQueue MessageQueue;

typedef struct PifRam {
    u32 words[15];
    u32 status;
} PifRam;

typedef struct Pfs {
    int status;
    MessageQueue *queue;
    int channel;
    u8 pad[0x59];
    u8 activebank;
} Pfs;

typedef struct ContRamWriteFormat {
    u8 dummy;
    u8 txsize;
    u8 rxsize;
    u8 command;
    u16 address;
    u8 data[32];
    u8 data_crc;
} ContRamWriteFormat;

extern PifRam D_800C4690[];
extern u8 D_800C4790[];
extern PifRam D_800C47B0[];
extern u8 D_800C48B0[];
extern int D_80057110[];

extern u8 func_8004EDA0(u16 address);
extern int func_8004EE50(MessageQueue *queue, int channel, u16 address,
                         u8 *buffer);
extern int func_8004F220(MessageQueue *queue, int channel, u16 address,
                         u8 *buffer, int force);

static __inline__ void make_motor_data(int channel, u16 address, u8 *buffer,
                                       PifRam *motor_data)
{
    u8 *output;
    ContRamWriteFormat request;
    int i;

    output = (u8 *)motor_data->words;
    for (i = 0; i < 15; i++) {
        motor_data->words[i] = 0;
    }
    motor_data->status = 1;
    request.dummy = 0xFF;
    request.txsize = 35;
    request.rxsize = 1;
    request.command = 3;
    request.address = (address << 5) | func_8004EDA0(address);
    request.data_crc = 0xFF;
    for (i = 0; i < 32; i++) {
        request.data[i] = *buffer++;
    }
    if (channel != 0) {
        for (i = 0; i < channel; i++) {
            *output++ = 0;
        }
    }
    *(ContRamWriteFormat *)output = request;
    output += sizeof(ContRamWriteFormat);
    output[0] = 0xFE;
}

int func_8004E8D0(MessageQueue *queue, Pfs *pfs, int channel)
{
    int i;
    int result;
    u8 temporary[32];

    pfs->queue = queue;
    pfs->channel = channel;
    pfs->status = 0;
    pfs->activebank = 0x80;
    for (i = 0; i < 32; i++) {
        temporary[i] = 0xFE;
    }
    result = func_8004F220(queue, channel, 0x400, temporary, 0);
    if (result == 2) {
        result = func_8004F220(queue, channel, 0x400, temporary, 0);
    }
    if (result != 0) {
        return result;
    }
    result = func_8004EE50(queue, channel, 0x400, temporary);
    if (result == 2) {
        result = 4;
    }
    if (result != 0) {
        return result;
    }
    if (temporary[31] == 0xFE) {
        return 11;
    }
    for (i = 0; i < 32; i++) {
        temporary[i] = 0x80;
    }
    result = func_8004F220(queue, channel, 0x400, temporary, 0);
    if (result == 2) {
        result = func_8004F220(queue, channel, 0x400, temporary, 0);
    }
    if (result != 0) {
        return result;
    }
    result = func_8004EE50(queue, channel, 0x400, temporary);
    if (result == 2) {
        result = 4;
    }
    if (result != 0) {
        return result;
    }
    if (temporary[31] != 0x80) {
        return 11;
    }
    if (D_80057110[channel] == 0) {
        for (i = 0; i < 32; i++) {
            D_800C4790[i] = 1;
            D_800C48B0[i] = 0;
        }
        make_motor_data(channel, 0x600, D_800C4790, &D_800C4690[channel]);
        make_motor_data(channel, 0x600, D_800C48B0, &D_800C47B0[channel]);
        D_80057110[channel] = 1;
    }
    return 0;
}
