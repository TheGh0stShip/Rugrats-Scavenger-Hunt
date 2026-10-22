typedef struct WordBuffer {
    unsigned char pad[0x14];
    int count;
    unsigned char pad18[4];
    int *words;
} WordBuffer;

int func_8004AB0C(WordBuffer *buffer, int mode, int value) {
    int *words = buffer->words;

    if (mode == 2) {
        int index = buffer->count;
        buffer->count = index + 1;
        words[index] = value;
    }
    return 0;
}

__asm__(".word 0");
