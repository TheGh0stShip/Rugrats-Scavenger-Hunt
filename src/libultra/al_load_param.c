typedef signed short s16;
typedef signed int s32;
typedef unsigned char u8;
typedef unsigned int u32;
typedef struct ALFilter { void *source; void *handler; void *set_parameter; } ALFilter;
typedef struct ALRawLoop { u32 start; u32 end; u32 count; } ALRawLoop;
typedef struct ALADPCMLoop { u32 start; u32 end; u32 count; s16 state[16]; } ALADPCMLoop;
typedef struct ALADPCMBook { s32 order; s32 predictors; s16 book[1]; } ALADPCMBook;
typedef struct ALWaveTable {
    u8 *base;
    s32 length;
    u8 type;
    u8 flags;
    union {
        struct { ALADPCMLoop *loop; ALADPCMBook *book; } adpcm;
        struct { ALRawLoop *loop; } raw;
    } info;
} ALWaveTable;
typedef struct ALLoadFilter {
    ALFilter filter;
    char padding_0c[8];
    void *state;
    void *loop_state;
    ALRawLoop loop;
    ALWaveTable *table;
    s32 book_size;
    void *dma;
    void *dma_state;
    s32 sample;
    s32 last_sample;
    s32 first;
    s32 memory_input;
} ALLoadFilter;
extern void func_80049F20(void);
extern void func_8004A364(void);
extern void func_8004CB10(void *destination, void *source, s32 size);

s32 func_8004A700(void *filter, s32 parameter_id, void *parameter)
{
    ALLoadFilter *loader = filter;
    ALFilter *base = filter;

    switch (parameter_id) {
    case 5:
        loader->table = parameter;
        loader->memory_input = (s32)loader->table->base;
        loader->sample = 0;
        switch (loader->table->type) {
        case 0:
            base->handler = func_80049F20;
            loader->table->length = 9 * (loader->table->length / 9);
            loader->book_size = 2 * loader->table->info.adpcm.book->order *
                                loader->table->info.adpcm.book->predictors * 8;
            if (loader->table->info.adpcm.loop) {
                loader->loop.start = loader->table->info.adpcm.loop->start;
                loader->loop.end = loader->table->info.adpcm.loop->end;
                loader->loop.count = loader->table->info.adpcm.loop->count;
                func_8004CB10(loader->table->info.adpcm.loop->state,
                              loader->loop_state, 32);
            } else {
                loader->loop.start = loader->loop.end = loader->loop.count = 0;
            }
            break;
        case 1:
            base->handler = func_8004A364;
            if (loader->table->info.raw.loop) {
                loader->loop.start = loader->table->info.raw.loop->start;
                loader->loop.end = loader->table->info.raw.loop->end;
                loader->loop.count = loader->table->info.raw.loop->count;
            } else {
                loader->loop.start = loader->loop.end = loader->loop.count = 0;
            }
            break;
        }
        break;
    case 4:
        loader->last_sample = 0;
        loader->first = 1;
        loader->sample = 0;
        if (loader->table) {
            loader->memory_input = (s32)loader->table->base;
            if (loader->table->type == 0) {
                if (loader->table->info.adpcm.loop) {
                    loader->loop.count = loader->table->info.adpcm.loop->count;
                }
            } else if (loader->table->type == 1) {
                if (loader->table->info.raw.loop) {
                    loader->loop.count = loader->table->info.raw.loop->count;
                }
            }
        }
        break;
    }
}
