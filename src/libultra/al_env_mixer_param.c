typedef signed short s16;
typedef signed int s32;

typedef struct ALFilter ALFilter;
typedef struct ALParam ALParam;

struct ALFilter {
    ALFilter *source;
    void *handler;
    s32 (*set_parameter)(void *filter, s32 parameter, void *value);
};

struct ALParam {
    ALParam *next;
};

typedef struct ALEnvMixer {
    ALFilter filter;
    char padding_0c[0x0E];
    s16 volume;
    char padding_1c[0x1C];
    s32 first;
    ALParam *control_list;
    ALParam *control_tail;
    char padding_44[4];
    s32 motion;
} ALEnvMixer;

s32 func_8004B08C(void *filter, s32 parameter_id, void *parameter)
{
    ALFilter *base = filter;
    ALEnvMixer *mixer = filter;

    switch (parameter_id) {
    case 3:
        if (mixer->control_tail) {
            mixer->control_tail->next = parameter;
        } else {
            mixer->control_list = parameter;
        }
        mixer->control_tail = parameter;
        break;
    case 4:
        mixer->first = 1;
        mixer->motion = 0;
        mixer->volume = 1;
        if (base->source) {
            base->source->set_parameter(base->source, 4, parameter);
        }
        break;
    case 9:
        mixer->motion = 1;
        if (base->source) {
            base->source->set_parameter(base->source, 9, parameter);
        }
        break;
    case 1:
        base->source = parameter;
        break;
    default:
        if (base->source) {
            base->source->set_parameter(base->source, parameter_id, parameter);
        }
        break;
    }
    return 0;
}
