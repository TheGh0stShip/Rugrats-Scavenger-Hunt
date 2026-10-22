typedef unsigned int u32;
typedef unsigned char u8;

typedef struct ResourcePool {
    u32 size;
    u8 *end;
    void *head;
    void *tail;
    u32 free_size;
    u32 largest_size;
    u32 reserved;
} ResourcePool;

extern int D_8005B1E0;
extern ResourcePool D_8005B1E8;

void init_aligned_resource_pool(void *base, u32 size) {
    u32 *words;

    D_8005B1E0 = 0;
    if (((u32)base & 0xF) == 0) {
        words = base;
        words[0] = 0;
        words[1] = 0;
        words[2] = 0;
        words[3] = 0;
        D_8005B1E8.head = base;
        D_8005B1E8.tail = base;
        D_8005B1E8.size = size;
        D_8005B1E8.end = (u8 *)base + size;
        D_8005B1E8.free_size = size;
        D_8005B1E8.largest_size = size;
        D_8005B1E8.reserved = 0;
        D_8005B1E0 = 1;
    }
}
