typedef unsigned char u8;
typedef unsigned int u32;

typedef struct HeapBlock {
    struct HeapBlock *next;
    struct HeapBlock *previous;
    struct HeapBlock *next_free;
    struct HeapBlock *previous_free;
} HeapBlock;

typedef struct HeapState {
    u32 unused0;
    u32 unused4;
    u32 unused8;
    HeapBlock *first_free;
    u32 free_bytes;
} HeapState;

extern HeapState D_8005B1E8;

void func_8001A3EC(void *allocation) {
    register u8 *address __asm__("$6") = allocation;

    if (address != 0) {
        __asm__ volatile("" ::: "memory");
        if (((u32)address & 0xF) == 0) {
            register HeapBlock *block __asm__("$5") = (HeapBlock *)(address - 0x10);

            if (((u32)block->previous_free & 0xF) != 0) {
            register HeapState *state __asm__("$4") = &D_8005B1E8;
            register u32 block_end __asm__("$2") = (u32)block->next;
            register u32 free_bytes __asm__("$3") = state->free_bytes;
            register u32 bin __asm__("$7");

            free_bytes += block_end - (u32)block;
            state->free_bytes = free_bytes;
            bin = (u32)block->next_free;
            block->next_free = *(HeapBlock **)((u8 *)state + bin * 0x20 + 0xC);
            *(HeapBlock **)((u8 *)state + bin * 0x20 + 0xC) = block;
            block->previous_free = 0;
            if (block->next_free != 0) {
                block->next_free->previous_free = block;
            }

            {
                register HeapBlock *next __asm__("$3") = block->next;

                if (next != 0 && ((u32)next->previous_free & 0xF) == 0) {
                    next->previous_free->next_free = next->next_free;
                    if (next->next_free != 0) {
                        next->next_free->previous_free = next->previous_free;
                    }
                    block->next = next->next;
                    if (next->next != 0) {
                        next->next->previous = block;
                    }
                    __asm__("" : : "r"(next));
                }
            }

            if (block->previous != 0 && ((u32)block->previous->previous_free & 0xF) == 0) {
                register HeapBlock *previous __asm__("$6") = block->previous;

                register u8 *base __asm__("$3") = (u8 *)&D_8005B1E8;
                register u32 offset __asm__("$2") = bin * 0x20;
                register HeapBlock *next_free __asm__("$4") = block->next_free;

                *(HeapBlock **)(base + offset + 0xC) = next_free;
                block->next_free->previous_free = 0;
                previous->next = block->next;
                if (block->next != 0) {
                    block->next->previous = previous;
                }
                __asm__("" : : "r"(block));
            }
            }
        }
    }
}
