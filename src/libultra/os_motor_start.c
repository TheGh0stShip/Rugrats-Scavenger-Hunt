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

extern u8 D_800C4640;
extern PifRam D_800C4650;
extern PifRam D_800C4690[];
extern u8 D_800C4790[];
extern int D_80057110[];

extern void func_8004DE24(void);
extern void func_8004DE90(void);
extern int func_8004DD10(int direction, void *buffer);
extern int func_80048DD0(MessageQueue *queue, void **message, int flags);
extern u8 func_8004EDE8(u8 *data);

int func_8004E710(Pfs *pfs)
{
    int result;
    int i;
    u8 *input;
    ContRamWriteFormat response;

    input = (u8 *)D_800C4650.words;
    if (D_80057110[pfs->channel] == 0) {
        return 5;
    }

    func_8004DE24();
    D_800C4640 = 3;
    result = func_8004DD10(1, &D_800C4690[pfs->channel]);
    func_80048DD0(pfs->queue, 0, 1);
    result = func_8004DD10(0, &D_800C4650);
    func_80048DD0(pfs->queue, 0, 1);

    if (pfs->channel != 0) {
        for (i = 0; i < pfs->channel; i++) {
            input++;
        }
    }

    response = *(ContRamWriteFormat *)input;
    result = (response.rxsize & 0xC0) >> 4;
    if (result == 0 && func_8004EDE8(D_800C4790) != response.data_crc) {
        result = 4;
    }

    func_8004DE90();
    return result;
}
