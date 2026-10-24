typedef signed short s16;
typedef signed int s32;
typedef unsigned char u8;
typedef struct ALLink { struct ALLink *next; struct ALLink *previous; } ALLink;
typedef struct ALHeap ALHeap;
typedef struct ALFilter { void *source; void *handler; void *set_parameter; s16 input; s16 output; s32 type; } ALFilter;
typedef struct ALSave { ALFilter filter; s32 dram_output; s32 first; } ALSave;
typedef struct ALBus { ALFilter filter; s32 source_count; s32 maximum_sources; ALFilter **sources; } ALBus;
typedef struct ALAuxBus { ALFilter filter; s32 source_count; s32 maximum_sources; ALFilter **sources; char effects[0x2C]; } ALAuxBus;
typedef struct ALParam { struct ALParam *next; char bytes[0x18]; } ALParam;
typedef struct ALLoadFilter { char bytes[0x48]; } ALLoadFilter;
typedef struct ALResampler { char bytes[0x34]; } ALResampler;
typedef struct ALEnvMixer { ALFilter filter; char bytes[0x38]; } ALEnvMixer;
typedef struct PVoice {
    ALLink node;
    void *virtual_voice;
    ALFilter *channel_knob;
    ALLoadFilter decoder;
    ALResampler resampler;
    ALEnvMixer envelope;
    s32 offset;
} PVoice;
typedef struct ALSynth {
    void *head;
    ALLink free_list;
    ALLink allocated_list;
    ALLink lame_list;
    s32 parameter_samples;
    s32 current_samples;
    void *dma;
    ALHeap *heap;
    ALParam *parameter_list;
    ALBus *main_bus;
    ALAuxBus *auxiliary_bus;
    ALFilter *output_filter;
    s32 physical_voice_count;
    s32 maximum_auxiliary_buses;
    s32 output_rate;
    s32 maximum_output_samples;
} ALSynth;
typedef struct ALSynConfig {
    s32 maximum_virtual_voices;
    s32 maximum_physical_voices;
    s32 maximum_updates;
    s32 maximum_effect_buses;
    void *dma;
    ALHeap *heap;
    s32 output_rate;
    u8 effect_type;
    char padding_1d[3];
    s32 *parameters;
} ALSynConfig;

extern void *func_8004CAC0(void *file, s32 line, ALHeap *heap, s32 count, s32 size);
extern void func_80049ED8(ALSave *save);
extern void func_80049E20(ALAuxBus *bus, ALFilter **sources, s32 count);
extern void func_80049E7C(ALBus *bus, ALFilter **sources, s32 count);
extern void func_8004D1F0(ALSynth *driver, s16 bus, ALSynConfig *config, ALHeap *heap);
extern s32 func_8004B9E0(void *filter, s32 parameter, void *value);
extern void func_8004CA2C(ALLink *item, ALLink *list);
extern void func_80049CE4(ALLoadFilter *loader, void *dma, ALHeap *heap);
extern s32 func_8004A700(void *filter, s32 parameter, void *value);
extern void func_80049D94(ALResampler *resampler, ALHeap *heap);
extern s32 func_8004BBFC(void *filter, s32 parameter, void *value);
extern void func_80049C2C(ALEnvMixer *mixer, ALHeap *heap);
extern s32 func_8004B08C(void *filter, s32 parameter, void *value);
extern s32 func_8004AB0C(void *filter, s32 parameter, void *value);
extern s32 func_8004C9A0(void *filter, s32 parameter, void *value);

void func_8004CB50(ALSynth *driver, ALSynConfig *config)
{
    s32 i;
    PVoice *voice;
    PVoice *voices;
    ALHeap *heap = config->heap;
    ALSave *save;
    ALFilter **sources;
    ALParam *parameters;
    ALParam *parameter;

    driver->head = 0;
    driver->physical_voice_count = config->maximum_physical_voices;
    driver->current_samples = 0;
    driver->parameter_samples = 0;
    driver->output_rate = config->output_rate;
    driver->maximum_output_samples = 160;
    driver->dma = config->dma;
    save = func_8004CAC0(0, 0, heap, 1, sizeof(ALSave));
    func_80049ED8(save);
    driver->output_filter = (ALFilter *)save;
    driver->auxiliary_bus = func_8004CAC0(0, 0, heap, 1, sizeof(ALAuxBus));
    driver->maximum_auxiliary_buses = 1;
    sources = func_8004CAC0(0, 0, heap, config->maximum_physical_voices, sizeof(ALFilter *));
    func_80049E20(driver->auxiliary_bus, sources, config->maximum_physical_voices);
    driver->main_bus = func_8004CAC0(0, 0, heap, 1, sizeof(ALBus));
    sources = func_8004CAC0(0, 0, heap, config->maximum_physical_voices, sizeof(ALFilter *));
    func_80049E7C(driver->main_bus, sources, config->maximum_physical_voices);
    if (config->effect_type != 0) {
        func_8004D1F0(driver, 0, config, heap);
    } else {
        func_8004B9E0(driver->main_bus, 2, &driver->auxiliary_bus[0]);
    }
    driver->free_list.next = 0;
    driver->free_list.previous = 0;
    driver->lame_list.next = 0;
    driver->lame_list.previous = 0;
    driver->allocated_list.next = 0;
    driver->allocated_list.previous = 0;
    voices = func_8004CAC0(0, 0, heap, config->maximum_physical_voices, sizeof(PVoice));
    for (i = 0; i < config->maximum_physical_voices; i++) {
        voice = &voices[i];
        func_8004CA2C(&voice->node, &driver->free_list);
        voice->virtual_voice = 0;
        func_80049CE4(&voice->decoder, driver->dma, heap);
        func_8004A700(&voice->decoder, 1, 0);
        func_80049D94(&voice->resampler, heap);
        func_8004BBFC(&voice->resampler, 1, &voice->decoder);
        func_80049C2C(&voice->envelope, heap);
        func_8004B08C(&voice->envelope, 1, &voice->resampler);
        func_8004AB0C(driver->auxiliary_bus, 2, &voice->envelope);
        voice->channel_knob = &voice->envelope.filter;
    }
    func_8004C9A0(save, 1, driver->main_bus);
    parameters = func_8004CAC0(0, 0, heap, config->maximum_updates, sizeof(ALParam));
    driver->parameter_list = 0;
    for (i = 0; i < config->maximum_updates; i++) {
        parameter = &parameters[i];
        parameter->next = driver->parameter_list;
        driver->parameter_list = parameter;
    }
    driver->heap = heap;
}
