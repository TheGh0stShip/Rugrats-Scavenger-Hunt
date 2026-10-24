typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct MessageQueue MessageQueue;

typedef struct PifRam {
    u32 words[15];
    u32 status;
} PifRam;

typedef struct ContRamWriteResponse {
    u8 dummy;
    u8 txsize;
    u8 rxsize;
    u8 command;
    u16 address;
    u8 data[32];
    u8 data_crc;
} ContRamWriteResponse;

extern u8 D_800C4640;
extern PifRam D_800C4650;

extern void func_8004DE24(void);
extern void func_8004DE90(void);
extern int func_8004DD10(int direction, void *buffer);
extern int func_80048DD0(MessageQueue *queue, void **message, int flags);
extern u8 func_8004EDE8(u8 *data);
extern int func_8004F5E0(MessageQueue *queue, int channel);
extern void func_8004F440(int channel, u16 address, u8 *buffer);

int func_8004F220(MessageQueue *queue, int channel, u16 address, u8 *buffer,
                  int force)
{
    int result = 0;
    int i;
    u8 *input;
    ContRamWriteResponse response;
    int retries;

    input = (u8 *)D_800C4650.words;
    retries = 2;

    if (force != 1 && address < 7 && address != 0) {
        return 0;
    }

    func_8004DE24();
    D_800C4640 = 3;
    func_8004F440(channel, address, buffer);
    result = func_8004DD10(1, &D_800C4650);
    func_80048DD0(queue, 0, 1);

    do {
        result = func_8004DD10(0, &D_800C4650);
        func_80048DD0(queue, 0, 1);
        input = (u8 *)D_800C4650.words;

        if (channel != 0) {
            for (i = 0; i < channel; i++) {
                input++;
            }
        }

        response = *(ContRamWriteResponse *)input;
        result = (response.rxsize & 0xC0) >> 4;
        if (result == 0) {
            if (func_8004EDE8(buffer) != response.data_crc) {
                result = func_8004F5E0(queue, channel);
                if (result != 0) {
                    func_8004DE90();
                    return result;
                }
                result = 4;
            }
        } else {
            result = 1;
        }
    } while (result == 4 && retries-- >= 0);

    func_8004DE90();
    return result;
}
