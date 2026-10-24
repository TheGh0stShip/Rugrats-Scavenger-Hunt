typedef struct {
    unsigned char pad0000[0x11E0];
    void *buf0;
    void *buf1;
    unsigned short id;
    unsigned char pad11EA[0x16];
} stream_channel_t;

extern stream_channel_t *D_800C4300;
extern unsigned short D_800C4304;
extern unsigned short D_800C4306;
extern int D_800C4308;
extern void *D_800C430C;
extern void *func_800430AC(int size);
extern void func_80043CE4(void);

int func_800431A0(int count, int arg1, int arg2, void *callback)
{
    int i;

    D_800C4304 = 0;
    D_800C4306 = 0;
    D_800C4308 = arg1;
    D_800C4300 = func_800430AC(count * 0x1200);
    if (D_800C4300 == 0)
        return 0;
    for (i = 0; i < count; i++) {
        D_800C4300[i].id = 0xFFFF;
        D_800C4300[i].buf0 = func_800430AC(0x1400);
        if (D_800C4300[i].buf0 == 0)
            return 0;
        D_800C4300[i].buf1 = func_800430AC(0x20);
        if (D_800C4300[i].buf1 == 0)
            return 0;
    }
    D_800C4304 = count;
    D_800C430C = callback;
    if (callback == 0)
        D_800C430C = func_80043CE4;
    return 1;
}
