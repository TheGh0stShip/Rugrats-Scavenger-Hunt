typedef struct channel_s {
    unsigned int flags; unsigned char *pdata; void *f08; int f0C; int stopping; int f14; int f18; int stopping_speed;
    float f20, f24, f28, f2C, freqoffset; unsigned char *f34; unsigned char *f38; int f3C, f40, handle, f48;
    float f4C, f50; int f54; float f58, f5C, f60; int f64; float f68, f6C, f70;
    void *song_bank; void *sample_bank; void *f7C; unsigned char *f80; void *f84; unsigned char *f88; unsigned char *f8C;
    float base_freq; int f94; short volume_scale; unsigned short f9A, volume; short temscale; unsigned short fA0, fA2, fA4, fx_number, base_volume, fAA, fAC, fAE; short pan_scale;
    unsigned short fB2, fB4; unsigned char fB6, fB7, fB8, fB9, fBA, fBB, fBC, fBD, pan_changed, fBF, fC0, fC1, fC2, fC3, fC4, fC5, fC6, fC7, fC8, fC9, fCA, pan, pan_dirty, fCD, fCE, fCF, fD0;
    signed char fD1; unsigned char fD2, fD3, fD4, fD5, fD6, fD7, fD8, fD9, fDA, fDB;
    float fDC; unsigned char *fE0[4]; unsigned char *fF0[4]; unsigned char *f100[4]; unsigned short f110[4]; unsigned short f118[4]; unsigned char f120[4]; unsigned char f124[4]; float f128[4]; unsigned char pad138[4];
} channel_t;
