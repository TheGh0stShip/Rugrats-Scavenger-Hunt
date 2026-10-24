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

extern void func_80043D90(stream_channel_t *ch, int a, void *buf, int size, void *cb);
extern void func_800435B4(unsigned short handle, signed char pan, unsigned short volume);

unsigned short func_800433F4(int a, signed char pan, unsigned short volume)
{
    int i;

    for (i = 0; i < D_800C4304; i++) {
        if (D_800C4300[i].id == 0xFFFF) {
            D_800C4300[i].id = D_800C4306++ & 0xFFF;
            func_80043D90(&D_800C4300[i], a, D_800C4300[i].buf0, 0xA00, D_800C430C);
            func_800435B4(D_800C4300[i].id, pan, volume);
            D_800C4300[i].mode = 4;
            return D_800C4300[i].id;
        }
    }
    return 0xFFFF;
}
