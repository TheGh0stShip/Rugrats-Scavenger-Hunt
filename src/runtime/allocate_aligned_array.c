typedef unsigned char u8;
typedef int s32;

typedef struct {
    u8 *base;
    u8 *current;
    s32 length;
    s32 allocation_count;
} Arena;

void *func_8004CAC0(const char *file, s32 line, Arena *heap, s32 count,
                    s32 size)
{
    s32 byte_count = (count * size + 15) & ~15;
    u8 *allocation = 0;

    if (heap->current + byte_count <= heap->base + heap->length) {
        allocation = heap->current;
        heap->current += byte_count;
    }
    return allocation;
}
