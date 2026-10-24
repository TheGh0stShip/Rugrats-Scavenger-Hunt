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

typedef struct PifRam {
    u32 words[15];
    u32 status;
} PifRam;

extern u8 D_800C45FC;
extern PifRam D_800C4600;

void func_8004E110(u8 command) {
    u8 *output;
    ContRequest request;
    int i;

    for (i = 0; i < 15; i++) {
        D_800C4600.words[i] = 0;
    }
    D_800C4600.status = 1;
    output = (u8 *)D_800C4600.words;
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
