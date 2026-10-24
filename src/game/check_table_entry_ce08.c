typedef struct { unsigned short h; short p; int q; int r; } Ent;
typedef struct { char pad[0x20]; Ent arr[1]; } Tab;
typedef struct { char pad[0x18]; Tab *t; } Obj;
extern int func_8000CDC8(void);
int func_8000CE08(Obj *o, int i) { Tab *t; int r = 0; if ((t = o->t) != 0) { t = (Tab *)((char *)t + i * 12); r = t->arr[0].h; if (r == 0) r = func_8000CDC8(); } return r; }
