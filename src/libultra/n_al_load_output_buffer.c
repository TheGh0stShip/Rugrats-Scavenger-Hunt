#include "n_audio_internal.h"

typedef struct {
    ALFilter filter;
    void *state;
    float ratio;
    int upitch;
    float delta;
    int first;
    void *control_list;
    void *control_tail;
    int motion;
} N_ALResampler;

typedef struct {
    unsigned int input;
    unsigned int output;
    short ffcoef;
    short fbcoef;
    short gain;
    float rsinc;
    float rsval;
    int rsdelta;
    float rsgain;
    void *lp;
    N_ALResampler *rs;
} N_ALDelay;

typedef struct {
    ALFilter filter;
    short *base;
    short *input;
    unsigned int length;
    N_ALDelay *delay;
    unsigned char section_count;
    void *param_handler;
} N_ALFx;

extern float func_8004C808(N_ALDelay *delay, int count);
extern Acmd *func_80041A00(N_ALFx *fx, short *current, int buffer, int count, Acmd *commands);
extern unsigned int func_800495D0(void *address);

Acmd *func_800417F0(N_ALFx *fx, N_ALDelay *delay, int buffer, Acmd *commands)
{
    Acmd *result = commands;
    int ratio;
    int count;
    int temporary_buffer = 0x2E0;
    short *output;
    float input_count;
    float float_ratio;
    float delta;
    int alignment = 0;
    int length;
    int count_in = FIXED_SAMPLE;

    if (delay->rs != 0) {
        length = delay->output - delay->input;
        delta = func_8004C808(delay, FIXED_SAMPLE);
        delta /= length;
        delta = (int)(delta * 32768.0f);
        delta = delta / 32768.0f;
        float_ratio = 1.0 - delta;

        input_count = delay->rs->delta + (float_ratio * (float)count_in);
        count = (int)input_count;
        delay->rs->delta = input_count - (float)count;

        output = &fx->input[-(delay->output - delay->rsdelta)];
        alignment = ((int)output & 7) >> 1;
        result = func_80041A00(fx, output - alignment, temporary_buffer, count + alignment, result);
        ratio = (int)(float_ratio * 32768.0f);
        {
            Acmd *command = result++;
            unsigned int output_bits;

            command->words.w0 = _SHIFTL(A_RESAMPLE, 24, 8) |
                                _SHIFTL(func_800495D0(delay->rs->state), 0, 24);
            output_bits = buffer >> 8;
            command->words.w1 = _SHIFTL(delay->rs->first, 30, 2) |
                                _SHIFTL(ratio, 14, 16) |
                                _SHIFTL(temporary_buffer + (alignment << 1), 2, 12) |
                                _SHIFTL(output_bits, 0, 2);
        }
        delay->rs->first = 0;
        delay->rsdelta += count - count_in;
    } else {
        output = &fx->input[-delay->output];
        result = func_80041A00(fx, output, buffer, FIXED_SAMPLE, result);
    }
    return result;
}
