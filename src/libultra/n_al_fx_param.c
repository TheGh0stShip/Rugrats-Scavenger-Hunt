#include "n_audio_internal.h"

typedef struct {
    short cutoff;
    short gain;
    long long coefficients[4];
    void *state;
    int first;
} N_ALLowPass;

typedef struct {
    unsigned int input;
    unsigned int output;
    short feedforward;
    short feedback;
    short gain;
    float resample_increment;
    float resample_value;
    int resample_delta;
    float resample_gain;
    N_ALLowPass *low_pass;
    void *resampler;
} N_ALDelay;

typedef struct {
    ALFilter filter;
    short *base;
    short *input;
    unsigned int length;
    N_ALDelay *delay;
    unsigned char section_count;
    void *parameter_handler;
} N_ALFx;

extern N_ALSynth *n_syn;
extern void func_800496F0(N_ALLowPass *filter);

int func_80041600(void *filter, int parameter_id, void *parameter)
{
    N_ALFx *fx = filter;
    int parameter_number = (parameter_id - 2) % 8;
    int section = (parameter_id - 2) / 8;
    int value = *(int *)parameter;

    switch (parameter_number) {
    case 0:
        fx->delay[section].input = (unsigned int)value & 0xFFFFFFF8;
        break;
    case 1:
        fx->delay[section].output = (unsigned int)value & 0xFFFFFFF8;
        break;
    case 3:
        fx->delay[section].feedforward = (short)value;
        break;
    case 2:
        fx->delay[section].feedback = (short)value;
        break;
    case 4:
        fx->delay[section].gain = (short)value;
        break;
    case 5:
        fx->delay[section].resample_increment =
            ((((float)value) / 1000) * 2.0) / n_syn->outputRate;
        break;
    case 6:
        fx->delay[section].resample_gain =
            (((float)value) / 173123.404906676) *
            (fx->delay[section].output - fx->delay[section].input);
        break;
    case 7:
        if (fx->delay[section].low_pass != 0) {
            fx->delay[section].low_pass->cutoff = (short)value;
            func_800496F0(fx->delay[section].low_pass);
        }
        break;
    }
    return 0;
}
