typedef signed short s16;
typedef signed int s32;
typedef unsigned int u32;
typedef struct Acmd { u32 word0; u32 word1; } Acmd;

typedef struct ALFilter ALFilter;
typedef Acmd *(*ALCmdHandler)(void *, s16 *, s32, s32, Acmd *);

struct ALFilter {
    ALFilter *source;
    ALCmdHandler handler;
    void *setParam;
    s16 inp;
    s16 outp;
    s32 type;
};

typedef struct ALSave {
    ALFilter filter;
    s32 dramout;
    s32 first;
} ALSave;

extern void func_8004B870(char *category, char *file, s32 line);

Acmd *func_8004C8A0(void *filter, s16 *outp, s32 outCount, s32 sampleOffset, Acmd *p)
{
    Acmd *ptr = p;
    ALSave *f = (ALSave *)filter;
    ALFilter *source = f->filter.source;

    if (source == 0) {
        func_8004B870("EX", "save.c", 34);
    }
    ptr = (*source->handler)(source, outp, outCount, sampleOffset, ptr);

    { Acmd *_a = (Acmd *)ptr++; _a->word0 = 0x08000000; _a->word1 = (((0) & 0xFFFF) << 16) | ((outCount << 1) & 0xFFFF); }
    { Acmd *_a = (Acmd *)ptr++; _a->word0 = 0x0D000000; _a->word1 = (((0x440) & 0xFFFF) << 16) | ((0x580) & 0xFFFF); }
    { Acmd *_a = (Acmd *)ptr++; _a->word0 = 0x08000000; _a->word1 = (((0) & 0xFFFF) << 16) | ((outCount << 2) & 0xFFFF); }
    { Acmd *_a = (Acmd *)ptr++; _a->word0 = 0x06000000; _a->word1 = (u32)(f->dramout); }
    return ptr;
}
