typedef signed short s16; typedef signed int s32; typedef unsigned int u32;
typedef struct Acmd { u32 word0; u32 word1; } Acmd;
typedef struct ALWaveTable { unsigned char *base; s32 length; } ALWaveTable;
typedef struct ALLoadFilter {
    char pad00[0x1C]; struct {u32 start,end,count;} loop; ALWaveTable *table;
    s32 book_size; u32 (*dma)(s32,s32,void *); void *dma_state;
    s32 sample,last_sample,first,memory_input;
} ALLoadFilter;
#define SET(packet,input,output,count) { Acmd *_a=(Acmd *)(packet);_a->word0=0x08000000|((input)&0xffff);_a->word1=(((output)&0xffff)<<16)|((count)&0xffff); }
#define LOAD(packet,address) { Acmd *_a=(Acmd *)(packet);_a->word0=0x04000000;_a->word1=(address); }
#define MOVE(packet,input,output,count) { Acmd *_a=(Acmd *)(packet);_a->word0=0x0A000000|((input)&0xffffff);_a->word1=(((output)&0xffff)<<16)|((count)&0xffff); }
#define CLEAR(packet,address,count) { Acmd *_a=(Acmd *)(packet);_a->word0=0x02000000|((address)&0xffffff);_a->word1=(count); }

Acmd *func_8004A364(void *filter, s16 *output, s32 output_count,
                    s32 sample_offset, Acmd *commands)
{
    Acmd *ptr=commands; s32 bytes,address,dram_align,dmem_align,overflow;
    s32 zero_start,samples,operation; ALLoadFilter *loader=filter;
    if(output_count==0)return ptr;
    if((output_count+loader->sample>loader->loop.end)&&(loader->loop.count!=0)){
        samples=loader->loop.end-loader->sample; bytes=samples<<1;
        if(samples>0){address=loader->dma(loader->memory_input,bytes,loader->dma_state);dram_align=address&7;bytes+=dram_align;SET(ptr++,*output,0,bytes+8-(bytes&7));LOAD(ptr++,address-dram_align);}else dram_align=0;
        *output+=dram_align; loader->memory_input=(s32)loader->table->base+(loader->loop.start<<1);loader->sample=loader->loop.start;operation=*output;
        while(output_count>samples){operation+=samples<<1;output_count-=samples;if((loader->loop.count!=-1)&&(loader->loop.count!=0))loader->loop.count--;samples=output_count<(loader->loop.end-loader->loop.start)?output_count:loader->loop.end-loader->loop.start;bytes=samples<<1;address=loader->dma(loader->memory_input,bytes,loader->dma_state);dram_align=address&7;bytes+=dram_align;if(operation&7)dmem_align=8-(operation&7);else dmem_align=0;SET(ptr++,operation+dmem_align,0,bytes+8-(bytes&7));LOAD(ptr++,address-dram_align);if(dram_align||dmem_align)MOVE(ptr++,operation+dram_align+dmem_align,operation,samples<<1);}
        loader->sample+=output_count;loader->memory_input+=output_count<<1;return ptr;
    }
    bytes=output_count<<1;overflow=loader->memory_input+bytes-((s32)loader->table->base+loader->table->length);if(overflow<0)overflow=0;if(overflow>bytes)overflow=bytes;
    if(overflow<bytes){if(output_count>0){bytes-=overflow;address=loader->dma(loader->memory_input,bytes,loader->dma_state);dram_align=address&7;bytes+=dram_align;SET(ptr++,*output,0,bytes+8-(bytes&7));LOAD(ptr++,address-dram_align);}else dram_align=0;*output+=dram_align;loader->sample+=output_count;loader->memory_input+=output_count<<1;}else loader->memory_input+=output_count<<1;
    if(overflow){zero_start=(output_count<<1)-overflow;if(zero_start<0)zero_start=0;CLEAR(ptr++,zero_start+*output,overflow);}
    return ptr;
}
