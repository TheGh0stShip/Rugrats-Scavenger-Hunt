#include "mus_channel.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed char s8;

typedef struct {
    u8 pad00[0x18];
    u8 *env_table;
    u8 pad1C[4];
    u16 *wave_table;
} song_t;

typedef struct {
    u8 pad00[0x14];
    u16 *wave_table;
} effect_t;

typedef struct {
    u8 pad00[0x28];
    float *detune;
    void **wave_list;
} sample_bank_t;

typedef struct {
    u16 wave;
    u16 adsr;
    u8 pan;
    u8 pitch;
} drum_t;

typedef struct {
    u8 pad00[0x1C];
} voice_t;

typedef struct {
    u8 *(*func)(channel_t *, u8 *);
} command_entry_t;

extern command_entry_t D_80055A30[];
extern voice_t *D_800C3FD8;
extern int D_800C3FE8;

extern void func_8003D014(channel_t *, int);
extern void func_8003D2F0(channel_t *);
extern void func_8003D554(channel_t *);
extern void func_80040450(voice_t *, int);
extern void func_80040610(voice_t *, int, int);
extern void func_80040750(voice_t *);

static u8 *set_envelope(channel_t *cp, u8 *ptr)
{
    u8 value;

    value = *ptr++;
    if (value == 0) {
        value = 1;
    }
    cp->fBF = value;
    cp->f64 = 1024 / value;
    cp->fC0 = *ptr++;
    value = *ptr++;
    cp->fC6 = value;
    cp->fC1 = *ptr++;
    cp->f58 = (1.0 / (float)value) * (float)(cp->fC1 - cp->fC0);
    value = *ptr++;
    cp->fC7 = value;
    cp->fC2 = *ptr++;
    cp->f5C = (1.0 / (float)value) * (float)(cp->fC2 - cp->fC1);
    value = *ptr++;
    cp->fC8 = value;
    cp->f60 = 1.0 / (float)value;
    return ptr;
}

void func_8003CACC(channel_t *cp, int index)
{
    u8 *ptr;
    u8 command;

    ptr = cp->pdata;
    while (ptr != 0 && (command = *ptr) >= 0x80) {
        ptr = D_80055A30[command & 0x7F].func(cp, ptr + 1);
    }
    cp->pdata = ptr;

    if (ptr != 0) {
        int note;
        int wave;
        sample_bank_t *bank;

        cp->f4C = cp->f50;
        note = *cp->pdata++;

        if (cp->fD2 != 0) {
            cp->fBB = *cp->pdata++;
            if (cp->fBB >= 0x80) {
                cp->fBB &= 0x7F;
                cp->fD2 = 0;
                cp->fD3 = cp->fBB;
            }
        } else {
            cp->fBB = cp->fD3;
        }

        if (cp->fAC != 0) {
            if (cp->fB7 == 0) {
                cp->f9A = cp->fAC;
            } else {
                cp->fB7 = 0;
                command = *cp->pdata++;
                if (command < 0x80) {
                    cp->f9A = command;
                } else {
                    cp->f9A = ((command & 0x7F) << 8) + *cp->pdata++;
                }
            }
        } else {
            command = *cp->pdata++;
            if (command < 0x80) {
                cp->f9A = command;
            } else {
                cp->f9A = ((command & 0x7F) << 8) + *cp->pdata++;
            }
        }

        cp->f40 = cp->f3C;
        cp->f3C += cp->f9A << 8;
        cp->fAA = 0;
        cp->fD1 = 0;
        cp->fD0 = cp->fCF;

        if (cp->song_bank != 0 && cp->f84 == 0) {
            song_t *song = (song_t *)cp->song_bank;
            if (song->wave_table[cp->fAE] == 0xFFFF) {
                note = 0x60;
            }
        }

        if (note != 0x60) {
            bank = (sample_bank_t *)cp->f7C;
            if (cp->f84 != 0) {
                cp->fAE = ((drum_t *)cp->f84)[note].wave;
                cp->fBD = ((drum_t *)cp->f84)[note].pan >> 1;
                set_envelope(cp, &((song_t *)cp->song_bank)->env_table[((drum_t *)cp->f84)[note].adsr * 7]);
                note = ((drum_t *)cp->f84)[note].pitch;
            }

            if (cp->fD6 == 0) {
                func_8003D2F0(cp);
            }
            if (cp->fD4 != 0) {
                func_8003D554(cp);
            }

            wave = cp->fAE;
            if (cp->song_bank != 0) {
                wave = ((song_t *)cp->song_bank)->wave_table[wave];
            } else {
                wave = ((effect_t *)cp->sample_bank)->wave_table[wave];
            }

            if (cp->fD7 == 0) {
                cp->f08 = bank->wave_list[wave];
                if (cp->fC9 != 0 && cp->fA0 != 0) {
                    cp->fA0 = 0;
                    func_80040610(&D_800C3FD8[index], 0, D_800C3FE8);
                } else {
                    func_8003D014(cp, index);
                }
            }

            cp->f2C = (float)note + bank->detune[wave];
            command = cp->fB9 * (1 - cp->fBA);
            cp->f2C += (float)((command & 0x80) ? (command - 256) : command);

            if (cp->fCA != cp->pan_dirty) {
                u8 value = cp->pan;
                value += ((0x80 - value) * cp->fCA) >> 7;
                cp->pan_dirty = cp->fCA;
                func_80040450(&D_800C3FD8[index], value);
            }
        } else if (cp->fC3 < 4) {
            cp->fC3 = 4;
            cp->fC5 = 1;
            cp->f54 = cp->f0C;
            cp->fCD = cp->fC4;
        }
    } else if (cp->fC9 != 0) {
        cp->fC9 = 0;
        func_80040610(&D_800C3FD8[index], 0, D_800C3FE8);
        func_80040750(&D_800C3FD8[index]);
    }
}
