typedef struct {
    unsigned char pad0000[0x11E8];
    unsigned short id;
    unsigned char pad11EA[0x16];
} stream_channel_t;

extern stream_channel_t *D_800C4300;
extern unsigned short D_800C4304;

void func_800434F8(unsigned short handle)
{
    int i;

    if (handle == 0xFFFF)
        return;
    for (i = 0; i < D_800C4304; i++) {
        if (D_800C4300[i].id == handle) {
            D_800C4300[i].id = 0xFFFF;
            return;
        }
    }
}
