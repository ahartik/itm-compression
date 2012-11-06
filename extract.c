#include<stdint.h>
#include"model.h"
#define O_ACCMODE      0003
#define O_RDONLY         00
#define O_WRONLY         01
#define O_RDWR           02
#define O_CREAT        0100 /* not fcntl */
#define O_EXCL         0200 /* not fcntl */
#define O_NOCTTY       0400 /* not fcntl */
#define O_TRUNC       01000 /* not fcntl */
#define O_APPEND      02000
#define O_NONBLOCK    04000
#define O_NDELAY    O_NONBLOCK
#define O_SYNC         04010000
#define O_FSYNC      O_SYNC
#define O_ASYNC      020000


typedef struct
{
    uint32_t lower;
    uint32_t upper;
    uint32_t code;
    uint8_t* data;
    int offset;
}adecoder;

int ad_get_bit(adecoder* t)
{
    int r = ((*t->data)>>t->offset)&1;
    t->offset++;

    if(t->offset == 8){
        t->data++;
        t->offset = 0;
    }
    return r;
}

static inline uint64_t div_u64_rem(uint64_t dividend, uint32_t divisor, uint32_t *remainder)
{
        union {
                uint64_t v64;
                uint32_t v32[2];
        } d = { dividend };
        uint32_t upper;

        upper = d.v32[1];
        d.v32[1] = 0;
        if (upper >= divisor) {
                d.v32[1] = upper / divisor;
                upper %= divisor;
        }
        asm ("divl %2" : "=a" (d.v32[0]), "=d" (*remainder) :
                "rm" (divisor), "" (d.v32[0]), "1" (upper));
        return d.v64;
}

static inline uint64_t do_div(uint64_t a,uint32_t b)
{
    uint32_t rem;
    return div_u64_rem(a,b,&rem);
}


#define MSBMATCH(a,b) (((a)>>31) == ((b)>>31))
#define IS_UNDERFLOW(l,u) (((l>>30)==1) && ((u>>30)==2))


void ad_init(adecoder* dec, uint8_t* data);
void ad_read_bits(adecoder* dec);
void ad_apply_range(adecoder* dec, uint32_t lower, uint32_t upper,uint32_t total);
uint32_t ad_read_prob(adecoder* dec, uint32_t total);

void ad_init(adecoder* dec,uint8_t* data)
{
    dec->lower = 0;
    dec->upper = ~0;
    dec->code = 0;
    dec->offset = 0;
    dec->data = data;
    for(int i=0;i<32;i++)
    {
        dec->code<<=1;
        dec->code|=ad_get_bit(dec);
    }
}
void ad_read_bits(adecoder* dec)
{
    while(MSBMATCH(dec->lower,dec->upper))
    {
        dec->lower <<= 1;
        dec->upper <<= 1;
        dec->upper |= 1;
        dec->code <<= 1;
        dec->code |= ad_get_bit(dec);
    }
    while(IS_UNDERFLOW(dec->lower, dec->upper))
    {
        dec->lower <<= 1;
        dec->upper <<= 1;
        dec->upper |= 1;
        dec->lower &= ~(1<<31);
        dec->upper |=  (1<<31);

        dec->code <<= 1;
        dec->code ^= 1<<31;
        dec->code |= ad_get_bit(dec);
    }
}

void ad_apply_range(adecoder* dec,uint32_t l,uint32_t u,uint32_t total)
{
    uint64_t range = (uint64_t)(dec->upper) - dec->lower+1;
    uint64_t scaled_low = (l * range) / TOTALPROB;
    uint64_t scaled_up = (u * range) / TOTALPROB;
    dec->upper = dec->lower + scaled_up - 1;
    dec->lower = dec->lower + scaled_low;
    ad_read_bits(dec);
}

uint32_t ad_read_prob(adecoder* dec, uint32_t total)
{
    uint32_t range = (dec->upper-dec->lower)+1;
    uint64_t unscaled = (uint64_t)(dec->code-dec->lower)+1;
    unscaled = unscaled * total;
    unscaled --;
    if (range == 0)
        unscaled>>= 32;
    else
        unscaled = do_div(unscaled,range);
    return (uint32_t)unscaled;
}

#include"model.c"
#include"data.h"

char ex1_output[2*EX1_DATA_LEN];

void ex1_extract()
{
    adecoder dec;
    ad_init(&dec, ex1_encoded);
    int in = openfile("ex1_side.dat",O_RDONLY,0);
    char* outs = ex1_output;
    for(int t = 0; t < EX1_DATA_LEN; t++)
    {
        uint32_t prob = nextProb(in);
        uint32_t p = ad_read_prob(&dec, TOTALPROB);
        //printf("%f %f\n", (double)prob/TOTALPROB, (double)p/TOTALPROB);
        uint32_t l,u;
        char out;
        if (p>=prob) l=prob, u=TOTALPROB, out='1';
        else l=0, u=prob, out='0';
        ad_apply_range(&dec, l, u, TOTALPROB);
        *outs++ = out;
        *outs++ = '\n';
    }

    int out = openfile("c/ex1_class.dat",O_WRONLY|O_TRUNC|O_CREAT, 0644);
    writedata(out, ex1_output, 2*EX1_DATA_LEN);
}
#include "model2.c"
uint32_t ex2_res[4*EX2_ROWS];
char ex2_output[EX2_ROWS * 100];
char* output;
void printrec(int x, int s) {
    if (x==0) return;
    printrec(x/10, s+1);
    *output++ = '0'+x%10;
    if (s==2) *output++ = '.';
}
void printval(int x) {
    printrec(x,0);
}
void ex2_extract()
{
    adecoder dec;
    ad_init(&dec, ex2_encoded);
    double prev = 5225;
    ld var = 100;
    const int learn = 5;
    ex2_res[0] = prev;
    for(int t = 1; t < 4*EX2_ROWS; t++)
    {
        uint32_t p = ad_read_prob(&dec, TOTALPROB);
        uint32_t s = prob2sym(p, prev, var);
        uint32_t low = sym2prob(s, prev, var);
        uint32_t hi = sym2prob(s+1, prev, var);
        ad_apply_range(&dec, low, hi, TOTALPROB);
        ex2_res[t] = s;

        double dx = s - prev;
        var = (learn*var + dx*dx) / (learn+1);
        prev = s;
    }
    output = ex2_output;
    for(int i=0; i<EX2_ROWS; ++i) {
        for(int j=0; j<4; ++j) {
            int v = ex2_res[i+j*EX2_ROWS];
            printval(v);
            *output++ = ',';
        }
        output[-1] = 10;
    }

    int outf = openfile("c/four_stocks.csv",O_WRONLY|O_TRUNC|O_CREAT, 0644);
    writedata(outf, ex2_output, (int)(output-ex2_output));
}
void _start()
{
    ex1_extract();
    ex2_extract();
    //exit
    asm ("xor %ebx, %ebx;mov $1, %eax;int $128;");
}
