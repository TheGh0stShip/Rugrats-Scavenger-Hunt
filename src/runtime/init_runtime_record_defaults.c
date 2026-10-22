typedef unsigned int u32;

extern u32 D_8004C8A0[];
extern u32 D_8004C9A0[];
extern void func_8004B880(void *record, u32 word_4, u32 word_8, u32 word_10);

void func_80049ED8(u32 *record) {
    func_8004B880(record, (u32)D_8004C8A0, (u32)D_8004C9A0, 3);
    record[5] = 0;
    record[6] = 1;
}
