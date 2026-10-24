typedef unsigned char u8;
typedef unsigned short u16;

typedef struct ContStatus {
    u16 type;
    u8 status;
    u8 error;
} ContStatus;

typedef struct ContRequestShort {
    u8 txsize;
    u8 rxsize;
    u8 command;
    u8 type_high;
    u8 type_low;
    u8 status;
} ContRequestShort;

extern u8 D_800C4650[];

void func_8004F750(int channel, ContStatus *data) {
    u8 *input = D_800C4650;
    ContRequestShort request;
    int i;

    for (i = 0; i < channel; i++) {
        input++;
    }
    request = *(ContRequestShort *)input;
    data->error = (request.rxsize & 0xC0) >> 4;
    if (data->error == 0) {
        data->type = (request.type_low << 8) | request.type_high;
        data->status = request.status;
    }
}
