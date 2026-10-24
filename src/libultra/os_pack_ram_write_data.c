typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct PifRam {
    u32 words[15];
    u32 status;
} PifRam;

typedef struct ContRamWriteFormat {
    u8 dummy;
    u8 txsize;
    u8 rxsize;
    u8 command;
    u16 address;
    u8 data[32];
    u8 data_crc;
} ContRamWriteFormat;

extern PifRam D_800C4650;
extern u8 func_8004EDA0(u16 address);

void func_8004F440(int channel, u16 address, u8 *buffer)
{
    u8 *output;
    ContRamWriteFormat request;
    int i;

    output = (u8 *)D_800C4650.words;
    D_800C4650.status = 1;
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
    output += sizeof(request);
    *output = 0xFE;
}
