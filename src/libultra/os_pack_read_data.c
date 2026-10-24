typedef signed char s8;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct ContReadFormat {
    u8 dummy;
    u8 txsize;
    u8 rxsize;
    u8 command;
    u16 button;
    s8 stick_x;
    s8 stick_y;
} ContReadFormat;

typedef struct PifRam {
    u32 words[15];
    u32 status;
} PifRam;

extern u8 D_800C45FC;
extern PifRam D_800C4600;

void func_8004DC54(void) {
    u8 *output = (u8 *)D_800C4600.words;
    ContReadFormat format;
    int i;

    for (i = 0; i < 15; i++) {
        D_800C4600.words[i] = 0;
    }
    D_800C4600.status = 1;
    format.dummy = 0xFF;
    format.txsize = 1;
    format.rxsize = 4;
    format.command = 1;
    format.button = 0xFFFF;
    format.stick_x = -1;
    format.stick_y = -1;
    for (i = 0; i < D_800C45FC; i++) {
        *(ContReadFormat *)output = format;
        output += sizeof(format);
    }
    *output = 0xFE;
}
