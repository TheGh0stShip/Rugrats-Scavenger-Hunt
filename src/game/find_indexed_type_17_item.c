typedef unsigned char u8;
typedef unsigned short u16;

typedef struct {
    u16 index;
    u16 type;
    u8 reserved[4];
} IndexedItemReference;

typedef struct {
    u8 data[8];
} IndexedItem;

IndexedItem *func_800149A4(IndexedItemReference *references, u16 count,
                           IndexedItem *items)
{
    u16 index;

    for (index = 0; index < count; index++) {
        IndexedItem *item = &items[references[index].index];

        if (references[index].type == 0x11) {
            return item;
        }
    }
    return 0;
}
