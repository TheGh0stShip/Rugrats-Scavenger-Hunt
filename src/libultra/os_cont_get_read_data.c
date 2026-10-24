typedef signed char s8;
typedef unsigned char u8;
typedef unsigned short u16;

typedef struct OSContPad {
    u16 button;
    s8 stick_x;
    s8 stick_y;
    u8 error;
} OSContPad;

typedef struct ContReadFormat {
    u8 dummy;
    u8 txsize;
    u8 rxsize;
    u8 command;
    u16 button;
    s8 stick_x;
    s8 stick_y;
} ContReadFormat;

extern u8 D_800C45FC;
extern u8 D_800C4600[];

void func_8004DBB8(OSContPad *data) {
    u8 *input = D_800C4600;
    ContReadFormat format;
    int i;

    for (i = 0; i < D_800C45FC; i++, input += sizeof(format), data++) {
        format = *(ContReadFormat *)input;
        data->error = (format.rxsize & 0xC0) >> 4;
        if (data->error == 0) {
            data->button = format.button;
            data->stick_x = format.stick_x;
            data->stick_y = format.stick_y;
        }
    }
}
