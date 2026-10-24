typedef unsigned char u8;
typedef struct ResourceRecord {
    unsigned char pad_00[0x44]; void *key;
    unsigned char pad_48[0x83]; u8 value; u8 marker;
    unsigned char tail[0x6F];
} ResourceRecord;
extern int D_800C3FD4;
extern ResourceRecord *D_800C3FDC;

int func_8003C25C(void *key, int value)
{
    int matches;
    int i;
    ResourceRecord *record;
    if (key == 0) return 0;
    if (value < 0) value = 0;
    else if (value >= 128) value = 127;
    matches = 0;
    record = D_800C3FDC;
    for (i = 0; i < D_800C3FD4; i++, record++) {
        if (record->key == key) {
            matches++;
            record->value = value;
            record->marker = 0xFF;
        }
    }
    return matches;
}
