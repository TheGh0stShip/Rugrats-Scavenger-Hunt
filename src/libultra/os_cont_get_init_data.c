typedef unsigned char u8;
typedef unsigned short u16;

typedef struct ContStatus {
    u16 type;
    u8 status;
    u8 error;
} ContStatus;

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
extern u8 D_800C4600[];

void func_8004E05C(u8 *pattern, ContStatus *data) {
    u8 *input;
    ContRequest request;
    int i;
    u8 bits = 0;

    input = D_800C4600;
    for (i = 0; i < D_800C45FC; i++, input += sizeof(request), data++) {
        request = *(ContRequest *)input;
        data->error = (request.rxsize & 0xC0) >> 4;
        if (data->error == 0) {
            data->type = (request.type_low << 8) | request.type_high;
            data->status = request.status;
            bits |= 1 << i;
        }
    }
    *pattern = bits;
}
