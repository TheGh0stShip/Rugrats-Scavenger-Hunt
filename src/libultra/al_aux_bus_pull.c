typedef signed short s16;
typedef signed int s32;
typedef unsigned int u32;

typedef struct Acmd {
    u32 word0;
    u32 word1;
} Acmd;

typedef struct ALFilter ALFilter;
typedef Acmd *(*ALCmdHandler)(void *, s16 *, s32, s32, Acmd *);

struct ALFilter {
    void *source;
    ALCmdHandler handler;
};

typedef struct ALAuxBus {
    char padding_00[0x14];
    s32 source_count;
    char padding_18[4];
    ALFilter **sources;
} ALAuxBus;

#define CLEAR_BUFFER(packet, destination, count) { \
    Acmd *_a = (Acmd *)(packet); \
    _a->word0 = 0x02000000 | (destination); \
    _a->word1 = (u32)(count); \
}

Acmd *func_8004AA30(void *filter, s16 *output, s32 output_count,
                    s32 sample_offset, Acmd *commands)
{
    Acmd *ptr = commands;
    ALAuxBus *bus = filter;
    ALFilter **sources = bus->sources;
    s32 i;

    CLEAR_BUFFER(ptr++, 0x6C0, output_count << 1);
    CLEAR_BUFFER(ptr++, 0x800, output_count << 1);
    for (i = 0; i < bus->source_count; i++) {
        ptr = sources[i]->handler(sources[i], output, output_count,
                                  sample_offset, ptr);
    }
    return ptr;
}
