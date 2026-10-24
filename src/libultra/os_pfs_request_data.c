typedef unsigned char u8;
typedef unsigned int u32;

typedef struct ContRequest {
    u8 dummy;
    u8 txsize;
    u8 rxsize;
    u8 command;
    u8 type_high;
    u8 type_low;
    u8 status;
    u8 padding;
} ContRequest;

extern u8 D_800C45FC;
extern u8 D_800C4640;
extern u32 D_800C468C;

void func_8004E3EC(u8 command) {
    u8 *output = (u8 *)&D_800C468C - 0x3C;
    ContRequest request;
    int i;

    D_800C4640 = command;
    D_800C468C = 1;
    request.dummy = 0xFF;
    request.txsize = 1;
    request.rxsize = 3;
    request.command = command;
    request.type_high = 0xFF;
    request.type_low = 0xFF;
    request.status = 0xFF;
    request.padding = 0xFF;
    for (i = 0; i < D_800C45FC; i++) {
        *(ContRequest *)output = request;
        output += sizeof(request);
    }
    *output = 0xFE;
}
