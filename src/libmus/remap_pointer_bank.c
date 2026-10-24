typedef unsigned char u8;
typedef unsigned int u32;

typedef struct {
    u8 *base;
    u32 length;
    u8 type;
    u8 flags;
    u8 pad0A[2];
    struct {
        void *loop;
        void *book;
    } adpcm;
} WaveTable;

typedef struct {
    u8 name[16];
    u32 flags;
    u32 wave_bank_name[3];
    int count;
    u8 *base_note;
    float *detune;
    WaveTable **wave_list;
} PointerBank;

extern void func_8003DF80(void *, void *, int);
extern void func_80047E80(void);

void func_8003DA50(char *pointer_data, char *wave_data)
{
    int i;
    PointerBank *bank;
    u8 *encoded_detune;
    u8 detune_byte;
    float *detune;
    float value;
    u32 base;

    bank = (PointerBank *)pointer_data;
    if (bank->flags & 0x80000000) {
        return;
    }
    bank->flags |= 0x80000000;

    func_8003DF80(&bank->base_note, pointer_data, 3);
    func_8003DF80(&bank->wave_list[0], pointer_data, bank->count);

    for (i = 0; i < bank->count; i++) {
        detune = &bank->detune[i];
        encoded_detune = (u8 *)detune;
        detune_byte = *encoded_detune;
        value = (float)((detune_byte & 0x80) ? detune_byte - 256 : detune_byte);
        *detune = value / 100.0;

        detune_byte = bank->base_note[i] - 0x30;
        value = (float)((detune_byte & 0x80) ? detune_byte - 256 : detune_byte);
        *detune += value;

        if (!bank->wave_list[i]->flags) {
            base = (u32)bank->wave_list[i]->base;
            if ((base & 0xFF000000) != 0xFF000000) {
                base += (u32)wave_data;
                bank->wave_list[i]->base = (u8 *)base;
            }
            bank->wave_list[i]->flags = 1;
            if (bank->wave_list[i]->adpcm.loop) {
                bank->wave_list[i]->adpcm.loop =
                    (void *)((u32)bank->wave_list[i]->adpcm.loop + (u32)pointer_data);
            }
            if (bank->wave_list[i]->type == 0) {
                bank->wave_list[i]->adpcm.book =
                    (void *)((u32)bank->wave_list[i]->adpcm.book + (u32)pointer_data);
            }
        }
    }
    func_80047E80();
}
