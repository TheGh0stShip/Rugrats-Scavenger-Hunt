typedef unsigned short u16;
typedef struct ResourceRecord {
    unsigned char pad_00[0x44]; void *key;
    unsigned char pad_48[0x50]; u16 scale; u16 pad_9A; u16 value;
    unsigned char pad_9E[0x0A]; u16 source;
    unsigned char tail[0x92];
} ResourceRecord;
extern int D_800C3FD4;
extern ResourceRecord *D_800C3FDC;

int func_8003C1D0(void *key, int scale)
{
    int matches;
    int i;
    ResourceRecord *record;
    if (key == 0) return 0;
    if (scale <= 0) scale = 1;
    else if (scale > 256) scale = 256;
    matches = 0;
    record = D_800C3FDC;
    for (i = 0; i < D_800C3FD4; i++, record++) {
        if (record->key == key) {
            record->scale = scale;
            record->value = (record->source * scale) >> 7;
            matches++;
        }
    }
    return matches;
}
