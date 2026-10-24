typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef struct {
    unsigned char pad0000[0x166];
    short unk166;
    unsigned char pad0168[0x28];
    void *dma;
    u32 rom;
    void *ring;
    u32 ring_size;
    u32 data[0x400];
    u16 frames;
    u16 rate;
    u32 words;
    u32 remaining;
    unsigned char queue[0x18];
    void *msg;
    u32 bit_position;
    u32 consumed;
    u32 fetched;
    u32 read;
    u32 write;
    u8 pending;
    u8 state;
    u8 max_frames;
} stream_t;

extern void func_80051050(void *address, int length);
extern void func_80047EB0(void *queue, void *msg, int count);

void func_80043D90(stream_t *s, u32 rom, void *ring, u32 ring_size, void *dma)
{
    func_80051050(s, 0x190);
    s->unk166 = 0x28;
    s->dma = dma;
    s->rom = rom;
    s->ring = ring;
    s->ring_size = ring_size;
    s->remaining = 0;
    func_80047EB0(s->queue, &s->msg, 1);
    if (rom == 0 || dma == 0) {
        s->state = 0;
    } else {
        s->state = 1;
    }
    s->pending = 0;
    s->frames = s->words = s->rate = 0;
    s->bit_position = 0;
    s->fetched = 0;
    s->consumed = 0;
    s->write = 0;
    s->read = 0;
    s->max_frames = 2;
}
