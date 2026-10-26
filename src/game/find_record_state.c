typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;

struct RecordState {
    s32 reserved;
    s16 slots[30];
};

extern u8 *D_800581F0;
extern struct RecordState D_80057F70[];
extern s16 func_8000E02C(s32 id, s32 index);

s32 func_80009194(s16 id, s32 *outStatus) {
    s32 status = 0;
    s32 index = 0;
    s32 result = 0;

    if (*(s16 *)(D_800581F0 + id * 0x210 + 0x201E) > 0) {
        do {
            if (func_8000E02C(id, index) != -1) {
                if (D_80057F70[id].slots[index] != 0) {
                    result = index;
                    if ((u16)D_80057F70[id].slots[index] - 0x17C < 4) {
                        status = 2;
                        break;
                    }
                } else if (status <= 0) {
                    result = index;
                    status = 1;
                }
            }
            index++;
        } while (index <
                 *(s16 *)(D_800581F0 + id * 0x210 + 0x201E));
    }
    if (outStatus != 0) {
        *outStatus = status;
    }
    return result;
}
