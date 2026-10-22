typedef unsigned int u32;
typedef unsigned short u16;

typedef struct {
    u32 word_0;
    u32 word_4;
    u32 word_8;
    u16 halfword_c;
    u16 halfword_e;
    u32 word_10;
} Record;

void func_8004B880(Record *record, u32 word_4, u32 word_8, u32 word_10) {
    record->word_0 = 0;
    record->word_4 = word_4;
    record->word_8 = word_8;
    record->halfword_c = 0;
    record->halfword_e = 0;
    record->word_10 = word_10;
}
