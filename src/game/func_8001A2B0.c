/* Target: func_8001A2B0  VRAM 0x8001A2B0  ROM 0x1AEB0
 * Size: 0x13C  Compiler: SN GCC 2.8 (-O2), reviewed scheduling normalization */
typedef unsigned char u8;
typedef signed int s32;
typedef unsigned int u32;

typedef struct HeapBlock {
    struct HeapBlock *next;      /* 0x00 physical next */
    struct HeapBlock *prev;      /* 0x04 physical prev */
    struct HeapBlock *nextFree;  /* 0x08 (pool index when allocated) */
    struct HeapBlock *prevFree;  /* 0x0C (-1 when allocated) */
} HeapBlock;

typedef struct HeapPool {
    u8 *start;                   /* 0x00 */
    u8 *end;                     /* 0x04 */
    u8 pad_08[4];
    HeapBlock *freeList;         /* 0x0C */
    u32 freeBytes;               /* 0x10 */
    u8 pad_14[0xC];
} HeapPool;

extern u32 D_8005B1E0;
extern HeapPool D_8005B1E8[];
extern u32 D_8005B204;
extern void func_8001A250(void);

void *func_8001A2B0(u32 size) {
    u32 i;
    u32 avail;
    HeapPool *pool;
    HeapBlock *b;
    HeapBlock *n;

    size += 0x1F;
    size &= ~0xF;
    for (i = 0; i < D_8005B1E0; i++) {
        b = D_8005B1E8[i].freeList;
        while (b != 0) {
            pool = &D_8005B1E8[i];
            if (b->next != 0) {
                avail = (u8 *)b->next - (u8 *)b;
            } else {
                avail = pool->end - (u8 *)b;
            }
            if (avail < size) {
                b = b->nextFree;
                continue;
            }
            if (size < avail) {
                n = (HeapBlock *)((u8 *)b + size);
                n->prev = b;
                n->next = b->next;
                n->nextFree = b->nextFree;
                n->prevFree = b;
                b->next = n;
                b->nextFree = n;
                if (n->next != 0) {
                    n->next->prev = n;
                }
                if (n->nextFree != 0) {
                    n->nextFree->prevFree = n;
                }
            }
            if (b->prevFree != 0) {
                b->prevFree->nextFree = b->nextFree;
            } else {
                pool->freeList = b->nextFree;
            }
            if (b->nextFree != 0) {
                b->nextFree->prevFree = b->prevFree;
            }
            b->nextFree = (HeapBlock *)i;
            b->prevFree = (HeapBlock *)-1;
            pool->freeBytes -= size;
            return b + 1;
        }
    }
    D_8005B204 = size - avail;
    func_8001A250();
    return 0;
}
