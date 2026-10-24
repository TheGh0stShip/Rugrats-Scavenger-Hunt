typedef struct {
    unsigned char pad0000[0x11E8];
    unsigned short id;
    unsigned char pad11EA[0xE];
    unsigned short left;
    unsigned short right;
    unsigned char pad11FC[4];
} stream_channel_t;

extern stream_channel_t *D_800C4300;
extern unsigned short D_800C4304;

void func_800435B4(unsigned short handle, signed char pan, unsigned short volume)
{
    int i;

    if (handle == 0xFFFF)
        return;
    for (i = 0; i < D_800C4304; i++) {
        if (D_800C4300[i].id != handle)
            continue;
        if (pan >= 0 && pan < 64) {
            D_800C4300[i].left = volume * ((pan * 0.25f) / 64.0f + 0.75f);
            D_800C4300[i].right = volume * ((pan * 0.75f) / 64.0f);
        } else if (pan > 64) {
            D_800C4300[i].left = volume * (((0x7F - pan) * 0.75f) / 63.0f);
            D_800C4300[i].right = volume * (((pan - 0x40) * 0.25f) / 63.0f + 0.75f);
        } else {
            D_800C4300[i].left = volume * 0.75f;
            D_800C4300[i].right = volume * 0.75f;
        }
    }
}
