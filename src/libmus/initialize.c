#include "mus_channel.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct {
    u32 control_flag;
    int channels;
    void *scheduler;
    int thread_priority;
    u8 *heap;
    int heap_length;
    u8 *pointer_bank;
    u8 *wave_bank;
    void *effect_bank;
    int fifo_length;
    int synth_updates;
    int output_rate;
    int command_count;
    int retrace_count;
    int dma_buffer_count;
    int dma_buffer_size;
    void *disk_handle;
} MusicConfig;

typedef struct {
    short priority;
    short fx_bus;
    u8 unity_pitch;
    u8 pad[3];
} VoiceConfig;

typedef struct {
    u8 bytes[0x1C];
} Voice;

typedef struct Player {
    struct Player *next;
    void *client_data;
    int (*handler)(void *);
} Player;

extern int D_80000300;
extern void *D_800C40B0;
extern u32 D_800C4030;
extern int D_800C3FD4;
extern Voice *D_800C3FD8;
extern channel_t *D_800C3FDC;
extern channel_t *D_800C3FE0;
extern int D_800C3FE4;
extern int D_800C3FE8;
extern void *D_800C3FF8;
extern void *D_800C3FFC;
extern void *D_800C4008;
extern void *D_800C400C;
extern int D_800C4010;
extern int D_800C4004;
extern int D_800C3FF0;
extern u32 D_800C3FF4;
extern Player D_800C3FC0;

extern void func_80043060(void *, int);
extern void func_80042AA0(void *);
extern void *func_800430AC(int);
extern void func_8003C660(int);
extern void func_8003C2E4(void *, void *);
extern void func_8003C4AC(void *);
extern void func_80042C50(MusicConfig *, int, int);
extern void func_8003B9F4(int, int);
extern int func_8003C7DC(void *);
extern void func_8003EEC0(Player *);
extern void func_8003DC7C(channel_t *);
extern void func_8003EF90(Voice *, VoiceConfig *);
extern int func_800430DC(void);

int func_8003B76C(MusicConfig *config)
{
    VoiceConfig voice_config;
    int i;

    D_800C40B0 = config->disk_handle;
    D_800C4030 = config->control_flag;
    D_800C3FD4 = config->channels + 4;
    if (D_80000300 == 0) {
        D_800C3FE4 = 50;
    } else {
        D_800C3FE4 = 60;
    }
    D_800C3FE8 = 1000000 / D_800C3FE4;

    func_80043060(config->heap, config->heap_length);
    func_80042AA0(config->scheduler);
    D_800C3FD8 = func_800430AC((D_800C3FD4 - 4) * 0x1C);
    D_800C3FDC = func_800430AC(D_800C3FD4 * sizeof(channel_t));
    D_800C3FE0 = D_800C3FDC + 4;
    func_8003C660(config->fifo_length);

    D_800C3FF8 = 0;
    D_800C3FFC = 0;
    if (config->pointer_bank && config->wave_bank) {
        func_8003C2E4(config->pointer_bank, config->wave_bank);
    }

    D_800C400C = 0;
    D_800C4008 = 0;
    if (config->effect_bank) {
        func_8003C4AC(config->effect_bank);
    }

    D_800C4010 = 0;
    D_800C4004 = 2;
    func_80042C50(config, D_800C3FE4, 2);
    func_8003B9F4(3, 0x7FFF);

    D_800C3FF0 = 1;
    D_800C3FF4 = 0x12345678;
    D_800C3FC0.next = 0;
    D_800C3FC0.handler = func_8003C7DC;
    D_800C3FC0.client_data = &D_800C3FC0;
    func_8003EEC0(&D_800C3FC0);

    for (i = 0; i < D_800C3FD4; i++) {
        D_800C3FDC[i].fC9 = 0;
        func_8003DC7C(&D_800C3FDC[i]);
        voice_config.unity_pitch = 0;
        voice_config.priority = config->thread_priority;
        voice_config.fx_bus = 0;
        if (i >= 4) {
            func_8003EF90(&D_800C3FD8[i - 4], &voice_config);
        }
    }
    return func_800430DC();
}
