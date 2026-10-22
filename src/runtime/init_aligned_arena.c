typedef unsigned int u32;

typedef struct {
    u32 start;
    u32 cursor;
    u32 size;
    u32 reserved;
} Arena;

void func_8004CA80(Arena *arena, u32 start, u32 size) {
    u32 adjustment = 16 - (start & 15);

    if (adjustment != 16) {
        arena->start = start + adjustment;
    } else {
        arena->start = start;
    }
    arena->size = size;
    arena->reserved = 0;
    arena->cursor = arena->start;
}
