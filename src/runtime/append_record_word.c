typedef unsigned int u32;

typedef struct {
    unsigned char pad_0[0x14];
    u32 count;
    unsigned char pad_18[4];
    u32 *words;
} Record;

int func_8004B9E0(Record *record, int mode, u32 value) {
    u32 *words = record->words;

    switch (mode) {
        case 2:
            words[record->count++] = value;
            break;
    }
    return 0;
}
