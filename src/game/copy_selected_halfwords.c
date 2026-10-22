typedef unsigned short u16;

void func_8001E060(u16 *destination, u16 *source, unsigned int count) {
    unsigned int index = 0;

    if (count != 0) {
        do {
            *destination++ = *source++;
            *destination++ = *source++;
            *destination++ = *source;
            source += 6;
            index++;
        } while (index < count);
    }
}
