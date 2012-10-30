
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
}

#define MSBMATCH(a,b) (((a)>>31) == ((b)>>31))
#define IS_UNDERFLOW(l,u) (((l>>30)==1) && ((u>>30)==2))


void ad_init(adecoder* dec, uint8_t* data);
void ad_read_bits(adecoder* dec);
void ad_apply_range(adecoder* dec, uint32_t lower, uint32_t upper,uint32_t total);

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

int main()
{
}
