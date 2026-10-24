typedef signed short s16;
typedef signed int s32;
typedef unsigned int u32;
typedef struct Acmd { u32 word0; u32 word1; } Acmd;
typedef struct ALPlayer ALPlayer;
typedef struct ALFilter ALFilter;
struct ALPlayer {
    ALPlayer *next;
    void *client_data;
    s32 (*handler)(ALPlayer *player);
    s32 call_time;
    s32 samples_left;
};
struct ALFilter {
    ALFilter *source;
    Acmd *(*handler)(void *, s16 *, s32, s32, Acmd *);
    s32 (*set_parameter)(void *, s32, void *);
};
typedef struct ALSynth {
    ALPlayer *head;
    char padding_04[0x18];
    s32 parameter_samples;
    s32 current_samples;
    char padding_24[0x14];
    ALFilter *output_filter;
    s32 physical_voices;
    s32 auxiliary_buses;
    s32 output_rate;
    s32 maximum_output_samples;
} ALSynth;
typedef struct ALGlobals { ALSynth driver; } ALGlobals;
extern ALGlobals *audio_globals;
extern s32 func_8004D138(ALSynth *driver, ALPlayer **client);
extern s32 func_8004D0A4(ALSynth *driver, s32 time);
extern void func_8004D010(ALSynth *driver);

Acmd *func_8004CE1C(Acmd *commands, s32 *command_count, s16 *output_buffer,
                    s32 output_length)
{
    ALPlayer *client;
    ALFilter *output;
    ALSynth *driver = &audio_globals->driver;
    s16 temporary = 0;
    Acmd *command_end = commands;
    Acmd *command;
    s32 count;
    s16 *buffer = output_buffer;

    if (driver->head == 0) {
        *command_count = 0;
        return commands;
    }
    for (driver->parameter_samples = func_8004D138(driver, &client);
         driver->parameter_samples - driver->current_samples < output_length;
         driver->parameter_samples = func_8004D138(driver, &client)) {
        driver->parameter_samples &= ~0xF;
        client->samples_left += func_8004D0A4(driver, client->handler(client));
    }
    driver->parameter_samples &= ~0xF;
    while (output_length > 0) {
        count = driver->maximum_output_samples < output_length
                    ? driver->maximum_output_samples : output_length;
        command = command_end;
        command->word0 = 0x07000000;
        command->word1 = 0;
        command++;
        output = driver->output_filter;
        output->set_parameter(output, 6, buffer);
        command_end = output->handler(output, &temporary, count,
                                      driver->current_samples, command);
        output_length -= count;
        buffer += count << 1;
        driver->current_samples += count;
    }
    *command_count = command_end - commands;
    func_8004D010(driver);
    return command_end;
}
