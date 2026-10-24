typedef unsigned char u8;
typedef int s32;

typedef struct {
    u8 reserved[4];
    s32 identifier;
} RuntimeSlot;

extern RuntimeSlot *D_8006D698[2];
extern RuntimeSlot *D_8006D688;
extern s32 D_8006D684;

void func_8001F510(s32 identifier)
{
    s32 index;
    RuntimeSlot *slot;

    for (index = 0; D_8006D698[index] != 0;) {
        if (D_8006D698[index]->identifier == identifier) {
            break;
        }
        index++;
        if (index >= 2) {
            break;
        }
    }
    if (index < 2) {
        slot = D_8006D698[index];
        if (slot != 0) {
            D_8006D688 = slot;
            D_8006D684 = index;
        }
    }
}
