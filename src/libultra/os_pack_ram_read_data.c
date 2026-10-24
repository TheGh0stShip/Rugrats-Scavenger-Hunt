typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct PifRam {
    u32 words[15];
    u32 status;
} PifRam;

typedef struct ContRamReadFormat {
    u8 dummy;
    u8 txsize;
    u8 rxsize;
    u8 command;
    u16 address;
    u8 data[32];
    u8 data_crc;
} ContRamReadFormat;

extern PifRam D_800C4650;
extern u8 func_8004EDA0(u16 address);

void func_8004F084(int channel, u16 address)
{
    u8 *output;
    ContRamReadFormat request;
    int i;

    output = (u8 *)D_800C4650.words;
    D_800C4650.status = 1;
    request.dummy = 0xFF;
    request.txsize = 3;
    request.rxsize = 33;
    request.command = 2;
    request.address = (address << 5) | func_8004EDA0(address);
    request.data_crc = 0xFF;

    for (i = 0; i < 32; i++) {
        request.data[i] = 0xFF;
    }

    if (channel != 0) {
        for (i = 0; i < channel; i++) {
            *output++ = 0;
        }
    }

    *(ContRamReadFormat *)output = request;
    output += sizeof(request);
    *output = 0xFE;
}
