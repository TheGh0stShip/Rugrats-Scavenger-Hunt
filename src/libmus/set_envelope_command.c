#include "mus_channel.h"

typedef unsigned char u8;

typedef struct {
    u8 pad00[0x18];
    u8 *envelopes;
} Song;

static u8 *set_envelope(channel_t *channel, u8 *data)
{
    u8 value;

    value = *data++;
    if (value == 0) {
        value = 1;
    }
    channel->fBF = value;
    channel->f64 = 1024 / value;
    channel->fC0 = *data++;
    value = *data++;
    channel->fC6 = value;
    channel->fC1 = *data++;
    channel->f58 = (1.0 / (float)value) *
                   (float)(channel->fC1 - channel->fC0);
    value = *data++;
    channel->fC7 = value;
    channel->fC2 = *data++;
    channel->f5C = (1.0 / (float)value) *
                   (float)(channel->fC2 - channel->fC1);
    value = *data++;
    channel->fC8 = value;
    channel->f60 = 1.0 / (float)value;
    return data;
}

u8 *func_8003B620(channel_t *channel, u8 *data)
{
    int index;

    index = *data++;
    if (index & 0x80) {
        index &= 0x7F;
        index <<= 8;
        index |= *data++;
    }
    set_envelope(channel,
                 &((Song *)channel->song_bank)->envelopes[index * 7]);
    return data;
}
