typedef signed int s32;
typedef float f32;

typedef struct ALFilter ALFilter;

struct ALFilter {
    ALFilter *source;
    void *handler;
    s32 (*set_parameter)(void *filter, s32 parameter, void *value);
};

typedef struct ALResampler {
    ALFilter filter;
    char padding_0c[0x0C];
    f32 ratio;
    s32 unity_pitch;
    f32 delta;
    s32 first;
    char padding_28[8];
    s32 motion;
} ALResampler;

s32 func_8004BBFC(void *filter, s32 parameter_id, void *parameter)
{
    ALFilter *base = filter;
    ALResampler *resampler = filter;
    union {
        f32 floating;
        s32 integer;
    } data;

    switch (parameter_id) {
    case 1:
        base->source = parameter;
        break;
    case 4:
        resampler->delta = 0.0f;
        resampler->first = 1;
        resampler->motion = 0;
        resampler->unity_pitch = 0;
        if (base->source) {
            base->source->set_parameter(base->source, 4, 0);
        }
        break;
    case 9:
        resampler->motion = 1;
        if (base->source) {
            base->source->set_parameter(base->source, 9, 0);
        }
        break;
    case 7:
        data.integer = (s32)parameter;
        resampler->ratio = data.floating;
        break;
    case 8:
        resampler->unity_pitch = 1;
        break;
    default:
        if (base->source) {
            base->source->set_parameter(base->source, parameter_id, parameter);
        }
        break;
    }
    return 0;
}
