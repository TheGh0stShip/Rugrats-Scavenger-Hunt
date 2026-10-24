typedef signed short s16; typedef signed int s32; typedef unsigned int u32;
typedef struct Acmd{u32 word0,word1;}Acmd;
typedef struct Book{s32 order,predictors;s16 book[1];}Book;
typedef struct Wave{unsigned char*base;s32 length;unsigned char type,flags;short pad;struct{void*loop;Book*book;}info;}Wave;
typedef struct Loader{char pad00[0x1c];struct{u32 start,end,count;}loop;Wave*table;s32 book_size;void*dma;void*dma_state;s32 sample,last_sample,first,memory_input;}Loader;
extern Acmd*func_8004A8AC(Acmd*,Loader*,s32,s32,s16,s16,u32);
#define LOAD_BOOK(packet,count,address){Acmd*_a=(Acmd*)(packet);_a->word0=0x0b000000|((count)&0xffffff);_a->word1=((u32)(address)&0x1fffffff);}
#define MOVE(packet,input,output,count){Acmd*_a=(Acmd*)(packet);_a->word0=0x0a000000|((input)&0xffffff);_a->word1=(((output)&0xffff)<<16)|((count)&0xffff);}
#define CLEAR(packet,address,count){Acmd*_a=(Acmd*)(packet);_a->word0=0x02000000|((address)&0xffffff);_a->word1=(count);}

Acmd*func_80049F20(void*filter,s16*output,s32 output_count,s32 sample_offset,Acmd*commands)
{
Acmd*ptr=commands;s16 input;s32 total,frames,bytes,overflow,zero_start,over,samples,operation,left,boundary,decoded=0,looped=0;Loader*loader=filter;
if(output_count==0)return ptr;input=0;LOAD_BOOK(ptr++,loader->book_size,loader->table->info.book->book);
looped=(output_count+loader->sample>loader->loop.end)&&(loader->loop.count!=0);if(looped)samples=loader->loop.end-loader->sample;else samples=output_count;
if(loader->last_sample)left=16-loader->last_sample;else left=0;total=samples-left;if(total<0)total=0;frames=(total+15)>>4;bytes=frames*9;
if(looped){ptr=func_8004A8AC(ptr,loader,total,bytes,*output,input,loader->first);if(loader->last_sample)*output+=loader->last_sample<<1;else*output+=32;loader->last_sample=loader->loop.start&15;loader->memory_input=(s32)loader->table->base+9*((s32)(loader->loop.start>>4)+1);loader->sample=loader->loop.start;boundary=*output;
while(output_count>samples){output_count-=samples;operation=(boundary+((frames+1)<<5))&~31;boundary+=samples<<1;if((loader->loop.count!=-1)&&(loader->loop.count!=0))loader->loop.count--;samples=output_count<(loader->loop.end-loader->loop.start)?output_count:loader->loop.end-loader->loop.start;total=samples-16+loader->last_sample;if(total<0)total=0;frames=(total+15)>>4;bytes=frames*9;ptr=func_8004A8AC(ptr,loader,total,bytes,operation,input,loader->first|2);MOVE(ptr++,operation+(loader->last_sample<<1),boundary,samples<<1);}
loader->last_sample=(output_count+loader->last_sample)&15;loader->sample+=output_count;loader->memory_input+=9*frames;return ptr;}
samples=frames<<4;overflow=loader->memory_input+bytes-((s32)loader->table->base+loader->table->length);if(overflow<0)overflow=0;over=(overflow/9)<<4;if(over>samples+left)over=samples+left;bytes-=overflow;
if((over-(over&15))<output_count){decoded=1;ptr=func_8004A8AC(ptr,loader,samples-over,bytes,*output,input,loader->first);if(loader->last_sample)*output+=loader->last_sample<<1;else*output+=32;loader->last_sample=(output_count+loader->last_sample)&15;loader->sample+=output_count;loader->memory_input+=9*frames;}else{loader->last_sample=0;loader->memory_input+=9*frames;}
if(over){loader->last_sample=0;if(decoded)zero_start=(left+samples-over)<<1;else zero_start=0;CLEAR(ptr++,zero_start+*output,over<<1);}return ptr;
}
