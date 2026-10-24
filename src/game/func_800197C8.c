/* Target: func_800197C8  VRAM 0x800197C8  ROM 0x1A3C8
 * Size: 0x248  Compiler: SN GCC 2.8 (-O2), reviewed scheduling normalization */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

typedef struct { u32 w0; u32 w1; } Gfx;

struct SubEntry {
    s32 unk0;
    s32 offset;
    s32 unk8;
};

struct Sub {
    u16 unk0;
    u16 count;
    u8 pad_04[0xC];
    s32 unk10;
    s32 unk14;
    u8 pad_18[2];
    s16 unk1A;
    s32 unk1C;
    struct SubEntry entries[1];
};

struct ResHeader {
    u8 pad_00[4];
    u16 unk4;
    u8 pad_06[2];
    s32 unk8;
    s32 unkC;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    s32 unk20;
    s32 unk24;
    s32 unk28;
    struct ResHeader *self;
    u8 flags;
    u8 unk31;
    u8 pad_32[0x12];
    s32 index;
    s32 unk48;
    s32 unk4C;
    u16 unk50;
    u16 unk52;
    s32 unk54;
    s32 unk58;
};

struct Res {
    u8 pad_00[0x18];
    struct ResHeader hdr;
};

extern u8 D_80059F30[];
extern struct Res *D_8005A148[];
extern struct Res *D_8005A990[];
extern u8 D_80099B33;
extern struct Res *func_80013358(s32);
extern void func_80019A4C(void *, void *, u16, void *);
extern void func_8001E034(s32, s32, s32);

#define RELOC(h, f) if ((h)->f != 0) (h)->f = (s32)(h) + (h)->f

struct ResHeader *func_800197C8(s32 idx) {
    struct ResHeader *h;
    struct Sub *sub;
    struct Res *r;
    s32 i;

    s32 unused[2];

    if (D_80059F30[idx] != 0) {
        D_8005A990[idx] = D_8005A148[idx];
        D_8005A148[idx] = 0;
        D_80059F30[idx] = 0;
    }
    if (D_8005A990[idx] != 0) {
        return &D_8005A990[idx]->hdr;
    }
    r = func_80013358(idx);
    D_8005A990[idx] = r;
    h = &r->hdr;
    RELOC(h, unk8);
    RELOC(h, unkC);
    RELOC(h, unk10);
    RELOC(h, unk14);
    RELOC(h, unk1C);
    RELOC(h, unk20);
    RELOC(h, unk28);
    h->self = h;
    h->index = idx;
    h->unk54 = (s32)h + h->unk54;
    h->unk58 = (s32)h + h->unk58;
    h->flags |= 1;
    h->unk48 = (s32)h + h->unk48;
    h->unk4C = (s32)h + h->unk4C;
    h->unk31 = D_80099B33 - 1;
    if (h->unk18 != 0) {
        h->unk18 = (s32)h + h->unk18;
        sub = (struct Sub *)h->unk18;
        sub->unk1A = -1;
        for (i = 0; i < ((struct Sub *)h->unk18)->count; i++) {
            struct SubEntry *e = &((struct Sub *)h->unk18)->entries[i];
            e->offset = h->unk18 + e->offset;
        }
        sub = (struct Sub *)h->unk18;
        sub->unk10 = (s32)sub + sub->unk10;
        sub = (struct Sub *)h->unk18;
        RELOC(sub, unk14);
        sub = (struct Sub *)h->unk18;
        RELOC(sub, unk1C);
    }
    func_80019A4C(h, (void *)h->unk54, h->unk50, (void *)h->unk48);
    func_80019A4C(h, (void *)h->unk58, h->unk52, (void *)h->unk4C);
    if (h->unk24 != 0) {
        h->unk10 = (s32)h + h->unk24;
        h->unk1C = h->unk10 + h->unk4 * 16;
        func_8001E034(h->unk10, h->unkC, h->unk4 * 4);
    }
    return h;
}
