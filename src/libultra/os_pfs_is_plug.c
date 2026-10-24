typedef int s32;
typedef unsigned char u8;
typedef unsigned short u16;

typedef struct ContStatus {
    u16 type;
    u8 status;
    u8 error;
} ContStatus;

extern u8 D_800C45FC;
extern u8 D_800C4650[];

extern void func_8004DE24(void);
extern void func_8004E3EC(u8 command);
extern s32 func_8004DD10(s32 direction, void *buffer);
extern s32 func_80048DD0(void *queue, void *message, s32 flags);
extern void func_8004E490(u8 *pattern, ContStatus *data);
extern void func_8004DE90(void);

s32 func_8004E270(void *queue, u8 *pattern) {
    s32 result = 0;
    void *message;
    u8 bit_pattern;
    ContStatus status[4];
    s32 channel;
    u8 bits = 0;
    s32 crc_errors = 3;

    func_8004DE24();
    do {
        func_8004E3EC(0);
        result = func_8004DD10(1, D_800C4650);
        func_80048DD0(queue, &message, 1);
        result = func_8004DD10(0, D_800C4650);
        func_80048DD0(queue, &message, 1);
        func_8004E490(&bit_pattern, status);
        for (channel = 0; channel < D_800C45FC; channel++) {
            if ((status[channel].status & 4) == 0) {
                crc_errors--;
                break;
            }
        }
        if (channel == D_800C45FC) {
            crc_errors = 0;
        }
    } while (crc_errors > 0);
    for (channel = 0; channel < D_800C45FC; channel++) {
        if (status[channel].error == 0 && (status[channel].status & 1)) {
            bits |= 1 << channel;
        }
    }
    func_8004DE90();
    *pattern = bits;
    return result;
}
