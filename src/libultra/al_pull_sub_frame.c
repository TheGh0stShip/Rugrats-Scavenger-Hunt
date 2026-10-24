typedef signed short s16; typedef signed int s32; typedef unsigned short u16; typedef unsigned int u32; typedef double f64;
typedef struct Acmd { u32 word0, word1; } Acmd;
typedef struct Filter Filter;
struct Filter { Filter *source; Acmd *(*handler)(void *,s16 *,s32,s32,Acmd *); void *set_parameter; s16 input,output; s32 type; };
typedef struct Env { Filter filter; void *state; s16 pan,volume,cvolL,cvolR,dryamt,wetamt;u16 lratl;s16 lratm,ltgt;u16 rratl;s16 rratm,rtgt;s32 delta,segEnd,first;void *ctrlList,*ctrlTail;Filter **sources;s32 motion;} Env;
extern s16 eqpower[];
extern s16 func_8004B564(f64,f64,s32,u16 *);
extern u32 osVirtualToPhysical(void *);
extern void func_8004B870(char *,char *,s32);
#define SETBUF(p,f,i,o,c) {Acmd*_a=(Acmd*)(p);_a->word0=0x08000000|((f)<<16)|((i)&0xffff);_a->word1=(((o)&0xffff)<<16)|((c)&0xffff);}
#define SETVOL(p,f,v,r,m) {Acmd*_a=(Acmd*)(p);_a->word0=0x09000000|((f)<<16)|((v)&0xffff);_a->word1=(((r)&0xffff)<<16)|((m)&0xffff);}
#define ENVMIX(p,f,a) {Acmd*_a=(Acmd*)(p);_a->word0=0x03000000|((f)<<16);_a->word1=(a);}
Acmd *func_8004B160(void *filter,s16 *input,s16 *output,s32 count,s32 offset,Acmd *commands)
{
 Acmd *ptr=commands; Env *env=filter; Filter *source=env->filter.source;
 if(env->motion!=1||!count)return ptr;
 if (!source) func_8004B870("EX", "env.c", 367);
 ptr=source->handler(source,input,count,offset,commands);
 SETBUF(ptr++,0,*input,1088+*output,count<<1);
 SETBUF(ptr++,8,1408+*output,1728+*output,2048+*output);
 if(env->first){env->first=0;
  env->ltgt=(env->volume*eqpower[env->pan])>>15;
  env->lratm=func_8004B564((f64)env->cvolL,(f64)env->ltgt,env->segEnd,&env->lratl);
  env->rtgt=(env->volume*eqpower[127-env->pan])>>15;
  env->rratm=func_8004B564((f64)env->cvolR,(f64)env->rtgt,env->segEnd,&env->rratl);
  SETVOL(ptr++,6,env->cvolL,0,0); SETVOL(ptr++,4,env->cvolR,0,0);
  SETVOL(ptr++,2,env->ltgt,env->lratm,env->lratl); SETVOL(ptr++,0,env->rtgt,env->rratm,env->rratl);
  SETVOL(ptr++,8,env->dryamt,0,env->wetamt); ENVMIX(ptr++,9,osVirtualToPhysical(env->state));
 }else ENVMIX(ptr++,8,osVirtualToPhysical(env->state));
 *input+=count<<1;env->delta+=count;return ptr;
}
