typedef int s32;
typedef unsigned char u8;
typedef unsigned short u16;

typedef struct ContStatus {
    u16 type;
    u8 status;
    u8 error;
} ContStatus;

extern u8 D_800C4650[];
extern void func_8004F6C0(int channel);
extern s32 func_8004DD10(s32 direction, void *buffer);
extern s32 func_80048DD0(void *queue, void *message, s32 flags);
extern void func_8004F750(int channel, ContStatus *data);

s32 func_8004F5E0(void *queue, int channel) {
    s32 result = 0;
    void *message;
    ContStatus data;

    func_8004F6C0(channel);
    result = func_8004DD10(1, D_800C4650);
    func_80048DD0(queue, &message, 1);
    result = func_8004DD10(0, D_800C4650);
    func_80048DD0(queue, &message, 1);
    func_8004F750(channel, &data);
    if ((data.status & 1) && (data.status & 2)) {
        return 2;
    } else if (data.error != 0 || !(data.status & 1)) {
        return 1;
    } else if (data.status & 4) {
        return 4;
    }
    return result;
}
