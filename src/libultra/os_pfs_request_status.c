typedef unsigned char u8;
typedef unsigned int u32;

typedef struct ContStatusRequest {
    u8 txsize;
    u8 rxsize;
    u8 command;
    u8 type_high;
    u8 type_low;
    u8 status;
} ContStatusRequest;

extern u8 D_800C4640;
extern u8 D_800C4650[];
extern u32 D_800C468C;

void func_8004F6C0(int channel)
{
    u32 *pif_status = &D_800C468C;
    u8 *output = (u8 *)pif_status - 0x3C;
    ContStatusRequest request;
    int i = 0;

    *pif_status = 1;
    request.txsize = 1;
    request.rxsize = 3;
    D_800C4640 = 0;
    request.command = 0;
    request.type_high = 0xFF;
    request.type_low = 0xFF;
    request.status = 0xFF;

    for (; i < channel; i++) {
        *output++ = 0;
    }

    *(ContStatusRequest *)output = request;
    output += sizeof(request);
    *output = 0xFE;
}
