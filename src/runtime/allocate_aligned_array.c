typedef unsigned int u32;

typedef struct {
    u32 start;
    u32 cursor;
    u32 size;
    u32 reserved;
} Arena;

void *func_8004CAC0(int unused_0, int unused_1, Arena *arena, u32 count,
                    u32 element_size) {
    u32 allocation_size = (count * element_size + 15) & ~15;
    u32 new_cursor = arena->cursor + allocation_size;
    void *result = 0;

    if (new_cursor <= arena->start + arena->size) {
        result = (void *)arena->cursor;
    }
    arena->cursor = new_cursor;
    return result;
}
