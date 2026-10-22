typedef unsigned int u32;

typedef struct ResourceHeader {
    u32 words[6];
} ResourceHeader;

extern void func_8001330C(u32 index, u32 offset, u32 size, void *destination);

u32 func_80013544(u32 index) {
    ResourceHeader header;

    func_8001330C(index, 0, sizeof(header), &header);
    return header.words[2];
}
