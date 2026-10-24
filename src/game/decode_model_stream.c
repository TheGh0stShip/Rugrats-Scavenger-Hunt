typedef unsigned char u8;
typedef unsigned int u32;

extern u32 D_80052ECC;
extern u8 *D_80052EC8;
extern void func_8001330C(u32 index, u32 offset, u32 size, void *destination);
extern void func_800166F0(u8 *src, u8 *dst, u32 *size);
extern void func_80019A10(void *data, u32 size);

void func_80013570(u8 *header, u32 id) {
    u32 *entry;
    u32 size;
    u8 *destination;
    u32 offset;
    u8 *base;

    base = header + 0x18;
    id <<= 4;
    id += *(u32 *)(*(u8 **)(base + 0x18) + 0x1C);
    entry = (u32 *)id;
    offset = *(u32 *)(header + 0x10);
    offset += entry[0];
    size = entry[1];
    if (D_80052ECC < size) {
        D_80052ECC = size;
    }
    func_8001330C(*(u32 *)(base + 0x44), offset, size, D_80052EC8);
    destination = header + *(u32 *)(header + 4);
    func_800166F0(D_80052EC8, destination, &size);
    func_80019A10(destination, size);
}
