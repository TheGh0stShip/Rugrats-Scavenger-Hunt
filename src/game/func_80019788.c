typedef unsigned char u8;
typedef int s32;

struct Entry_8005A990 {
    s32 tag;
    u8 pad_04[0x14];
    u8 data[1];
};

extern struct Entry_8005A990 *D_8005A990[];
extern s32 D_80001630;

u8 *func_80019788(s32 idx)
{
    struct Entry_8005A990 *entry = D_8005A990[idx];

    if (entry != 0) {
        if (entry->tag == D_80001630) {
            return entry->data;
        }
    } else {
        return 0;
    }
}
