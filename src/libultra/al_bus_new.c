typedef int s32;

typedef struct AudioBus {
    char filter[0x14];
    s32 source_count;
    s32 max_sources;
    void *sources;
} AudioBus;

extern void func_8004B880(void *filter, void *handler, void *set_param, s32 type);
extern void func_8004AA30(void);
extern void func_8004AB0C(void);
extern void func_8004B8A0(void);
extern void func_8004B9E0(void);

void func_80049E20(AudioBus *bus, void *sources, s32 max_sources) {
    func_8004B880(bus, func_8004AA30, func_8004AB0C, 6);
    bus->source_count = 0;
    bus->max_sources = max_sources;
    bus->sources = sources;
}

void func_80049E7C(AudioBus *bus, void *sources, s32 max_sources) {
    func_8004B880(bus, func_8004B8A0, func_8004B9E0, 7);
    bus->source_count = 0;
    bus->max_sources = max_sources;
    bus->sources = sources;
}
