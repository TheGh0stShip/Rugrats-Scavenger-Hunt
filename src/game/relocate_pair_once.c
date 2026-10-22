typedef unsigned short u16;
typedef unsigned int u32;

typedef struct RelocPair { u32 first; u32 second; u16 flags; } RelocPair;

void func_80019A10(RelocPair *pair) {
    if (!(pair->flags & 0x4000)) {
        pair->first = (u32)pair + pair->first;
        pair->second = (u32)pair + pair->second;
        pair->flags |= 0x4000;
    }
}
