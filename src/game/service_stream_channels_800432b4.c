typedef struct {
    unsigned char pad0000[0x11A0];
    unsigned short pos;
    unsigned short rate;
    int unk11A4;
    unsigned char pad11A8[0x34];
    unsigned char unk11DC;
    unsigned char status;
    unsigned char mode;
    unsigned char pad11DF;
    void *buf0;
    void *buf1;
    unsigned short id;
    unsigned char playing;
    unsigned char pad11EB;
    int unk11EC;
    float time;
    int unk11F4;
    unsigned short left;
    unsigned short right;
    unsigned char pad11FC[4];
} stream_channel_t;

extern stream_channel_t *D_800C4300;
extern unsigned short D_800C4304;
extern unsigned short D_800C4306;
extern int D_800C4308;
extern void *D_800C430C;

extern void func_80043E54(stream_channel_t *ch);
extern void func_80044238(stream_channel_t *ch);

void func_800432B4(void)
{
    int i;
    stream_channel_t *ch;

    for (i = 0; i < D_800C4304; i++) {
        if (D_800C4300[i].id == 0xFFFF)
            continue;
        func_80043E54(&D_800C4300[i]);
        switch (D_800C4300[i].status) {
        case 2:
            ch = &D_800C4300[i];
            ch->playing = 1;
            ch->unk11EC = 0;
            D_800C4300[i].time = (float)((D_800C4300[i].rate << 15) / D_800C4308) / 32768.0f;
            D_800C4300[i].unk11F4 = 0;
            func_80044238(&D_800C4300[i]);
            break;
        case 4:
            D_800C4300[i].id = 0xFFFF;
            break;
        }
    }
}
