typedef unsigned int u32;

typedef struct StreamState {
    unsigned char pad_0000[0x11A8];
    int countdown;
    unsigned char pad_11AC[0x28];
    u32 current;
    u32 end;
    unsigned char pad_11DC;
    signed char state;
} StreamState;

u32 func_800441CC(StreamState *stream, u32 amount)
{
    if (stream->state == 3) {
        u32 remaining;
        u32 consumed;

        if (stream->countdown > 0) {
            stream->countdown -= amount;
            if (stream->countdown <= 0) {
                stream->state = 4;
                stream->countdown = 0;
            }
        }
        remaining = stream->end - stream->current;
        consumed = amount < remaining ? amount : remaining;
        stream->current += consumed;
        return consumed;
    }
    return 0;
}
