typedef unsigned int u32;

typedef struct {
    u32 start;
    u32 cursor;
    u32 size;
    u32 reserved;
} Arena;

void *func_8004CAC0(int unused_0, int unused_1, Arena *arena, u32 count,
                    u32 element_size) {
    register u32 arena_size __asm__("$3") = arena->size;
    u32 end = arena->start + arena_size;
    register u32 work __asm__("$4") = ~15;
    register u32 cursor __asm__("$5") = arena->cursor;
    u32 allocation_size;
    register void *result __asm__("$8");

    allocation_size = (count * element_size + 15) & work;
    work = cursor + allocation_size;
    result = 0;
    if (end < work) {
        goto done;
    }
    result = (void *)cursor;
    __asm__("" : "=r"(result) : "0"(result));
    arena->cursor = work;
done:
    return result;
}
