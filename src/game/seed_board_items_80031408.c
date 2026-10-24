/* func_80031408: seeds board item cells and distributes the reserved pieces.
 * SN Systems GCC 2.8 (-O2); register allocation finished by the reviewed
 * scheduling normalizer. */
typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;
typedef signed int s32;
typedef unsigned int u32;

struct Cell_80031408 {
    u8 pad_00[0x4];
    s16 kind;
    u8 pad_06[0x2];
    s16 tag;
    u8 pad_0A[0x6];
};

struct Row_80031408 {
    u8 pad_00[0xA];
    s16 count;
    u16 used;
    u8 pad_0E[0x2];
    struct Cell_80031408 items[32];
};

struct Board_80031408 {
    u8 pad_00[0x4];
    s16 nrows;
    u8 pad_06[0x200E];
    struct Row_80031408 rows[1];
};

struct Slot_80031408 {
    struct Cell_80031408 *item;
    s32 idx;
};

struct Player_80031408 {
    u8 pad_00[0x1AC];
    u32 cards[4];
};

struct Fx_80031408 {
    u8 pad_00[0x4];
    s16 unk4;
    u8 pad_06[0x6];
    s32 unkC;
    s32 unk10;
};

extern struct Board_80031408 *D_800581F0;
extern struct Slot_80031408 *D_800B936C;
extern s32 D_80054810;
extern s16 D_800B9364;
extern s32 D_80057D5C;
extern s32 D_80052F18;
extern struct Fx_80031408 D_800C33C0;

extern struct Slot_80031408 *func_8001A2B0(s32 size);
extern void func_8001A3EC(void *p);
extern s32 func_8003A840(void);
extern void func_80031328(s32 val);
extern struct Player_80031408 *func_800055A4(s32 idx);

void func_80031408(void) {
    s32 i, j, r, placed, n;
    struct Board_80031408 *b;
    struct Row_80031408 *row;
    struct Row_80031408 *row2;
    struct Cell_80031408 *it;
    struct Player_80031408 *p;
    struct Slot_80031408 *slots;
    struct Slot_80031408 *slots2;

    D_800B936C = slots = func_8001A2B0(0x320);
    D_80054810 = 0;
    {
        s32 k, m;

        k = 0;
        if (k < D_800581F0->nrows - 1) {
            for (; k < D_800581F0->nrows - 1; k++) {
                b = D_800581F0;
                __asm__("" : "=r"(slots2) : "0"(slots));
                row = &b->rows[k];
                m = 0;
                if (m < row->count) {
                    __asm__("" : "=r"(row2) : "0"(row));
                    for (; m < row->count; m++) {
                        if ((u32)(row->items[m].kind - 0x127) < 0x51) {
                            it = &row2->items[m];
                            slots2[D_80054810].item = it;
                            it->tag = -1;
                            slots2[D_80054810].idx = k;
                            D_80054810++;
                        } else {
                            b->rows[k].items[m].tag = -1;
                        }
                    }
                }
            }
        }
    }

    D_800B9364 = *((u16 *)&D_80054810 + 1);
    for (i = 0; i < D_800581F0->nrows - 1; i++) {
        placed = 0;
        do {
            do {
                r = func_8003A840() % D_800581F0->rows[i].count;
            } while (D_800581F0->rows[i].items[r].tag >= 0 ||
                     D_800581F0->rows[i].items[r].kind < 0x127);
            placed++;
            D_800581F0->rows[i].items[r].tag = 0x178;
            D_800581F0->rows[i].used++;
            D_800B9364--;
        } while (placed <= 0);
    }

    for (i = 0; i < D_80057D5C; i++) {
        for (j = 0; j < 3; j++) {
            func_80031328(j + 0x179);
        }
    }

    for (n = 0; n < D_80057D5C; n++) {
        p = func_800055A4(n);
        for (j = 0; j < 4; j++) {
            func_80031328(p->cards[j] & 0x7FFFFFFF);
        }
    }

    if (D_80052F18 == 0) {
        for (i = 0; i < 4; i++) {
            func_80031328(i + 0x17C);
        }
    }

    D_800C33C0.unkC = -1;
    D_800C33C0.unk10 = -1;
    D_800C33C0.unk4 = 0xFF;
    func_8001A3EC(D_800B936C);
}
