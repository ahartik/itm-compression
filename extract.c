
#include<stdint.h>


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
    uint64_t scaled_low = (l * range) / total;
    uint64_t scaled_up = (u * range) / total;
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

void _start()
{
    adecoder dec;
    ad_init(&dec, ex1_encoded);
    for(int t = 0; t < EX1_DATA_LEN; t++)
    {
        uint32_t prob = nextProb();
        uint32_t p = ad_read_prob(&dec, totalprob);
        //printf("%f %f\n", (double)prob/totalprob, (double)p/totalprob);
        if (p >= prob)
        {
            //puts("1");
            ex1_output[t*2] = '1';
            ad_apply_range(&dec, prob, totalprob, totalprob);
        }
        else 
        {
            //puts("0");
            ex1_output[t*2] = '0';
            ad_apply_range(&dec, 0, prob, totalprob);
        }
        ex1_output[t*2+1] = '\n';
    }
    writedata(1, ex1_output, 2*EX1_DATA_LEN);
    //printf("%s", ex1_output);
    asm ("mov $1, %eax;int $128;");
}
