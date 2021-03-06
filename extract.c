#include<stdint.h>
#include<stdlib.h>
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
                "rm" (divisor), "0" (d.v32[0]), "1" (upper));
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
//    uint64_t scaled_low = (l * range) / TOTALPROB;
//    uint64_t scaled_up = (u * range) / TOTALPROB;
    uint64_t scaled_low = do_div(l * range, total);
    uint64_t scaled_up = do_div(u * range, total);
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
uint32_t ex2_low[4*EX2_ROWS];
char ex2_output[EX2_ROWS * 100];
char* output;
void printval(int x) {
    if (x<0) {
        *output++ = '-';
        x = -x;
    }
    if (x==0) return;
    printval(x/10);
    *output++ = '0'+x%10;
}
void ex2_extract()
{
    adecoder dec;
    ad_init(&dec, ex2_encoded);
    double prev = START_VALUE/100;
    ld var = 1;
    const int learn = LEARN_RATE;
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
        prev = s;
        if (fabs(dx)>9) continue;
        var = (learn*var + dx*dx) / (learn+1);
    }
    uint32_t counts[100];
    for(int i=0; i<100; ++i) counts[i] = 1;
    uint32_t csum = 100;
    for(int t=0; t<4*EX2_ROWS; ++t) {
        uint32_t p = ad_read_prob(&dec, csum);
        uint32_t out,sum;
//        printf("k: %d\n", p);
        for(out=sum=0; sum<=p; sum += counts[out++]);
        --out;
        ad_apply_range(&dec, sum-counts[out], sum, csum);
        ex2_low[t] = out;

        counts[out] += LOW_LEARN_RATE;

        const int K = 4*LOW_UNLEARN;
        if (t>=K) {
            int old = ex2_low[t-K];
            counts[old] -= LOW_LEARN_RATE;
        } else csum += LOW_LEARN_RATE;
    }
    output = ex2_output;
    for(int i=0; i<EX2_ROWS; ++i) {
        for(int j=0; j<4; ++j) {
            int v = ex2_res[i+j*EX2_ROWS];
            int v2 = ex2_low[4*i+j];
            printval(v);
            *output++ = '.';
            *output++ = '0' + v2/10;
            *output++ = '0' + v2%10;
            *output++ = ',';
        }
        output[-1] = 10;
    }

    int outf = openfile("c/four-stocks.csv",O_WRONLY|O_TRUNC|O_CREAT, 0644);
    writedata(outf, ex2_output, (int)(output-ex2_output));
}

#if 0
#include"model3_tree.h"
#else
#pragma pack(push,1)
typedef struct {
    signed char to[2];
    unsigned var : 4;
    short split : 12;
} Ex3Node;
#pragma pack(pop)
#include"model3_tree2.h"
int ex3_class(int* x) {
    int i=0;
    while(i>=0) {
        int var = ex3_tree[i].var;
        int split = ex3_tree[i].split;
        i = ex3_tree[i].to[x[var] >= split];
    }
    return -i;
}
#endif

char ex3_side_s[2<<20];
char* input;
int readint() {
    int r=0;
    int f=1;
    while(1) {
        char c = *input++;
//      printf("c %c %d\n", c, c);
        if (c>='0' && c<='9') {
            r = 10*r + c-'0';
        } else if (c=='-') {
            f=-1;
        } else {
            break;
        }
    }
    return f*r;
}
#define EX3_SIZE 58000
char ex3_output[1<<20];
void ex3_extract() {
    int in = openfile("shuttle.side",O_RDONLY,0);
    readdata(in, ex3_side_s, sizeof(ex3_side_s));
    input = ex3_side_s;
    int x[9];
    output = ex3_output;
    for(int i=0; i<EX3_SIZE; ++i) {
        for(int j=0; j<9; ++j) x[j] = readint();
//      for(int j=0; j<9; ++j) printf("%d ",x[j]);putchar(10);
        int c = ex3_class(x);
        *output++ = '0' + c;
        *output++ = '\n';
    }
    int outf = openfile("c/shuttle.class",O_WRONLY|O_TRUNC|O_CREAT, 0644);
    writedata(outf, ex3_output, 2*EX3_SIZE);
}

#include "model4.c"
int ex4_mat[512][512];
char ex4_output[1<<20];
void printmat() {
    for(int i=0; i<512; ++i) {
        for(int j=0; j<512; ++j) {
            int x = ex4_mat[i][j];
            if (x==0) *output++='0';
            else printval(x);
            *output++ = ' ';
        }
        output[-1] = '\n';
    }
}
void ex4_extract() {
    adecoder dec;
    ad_init(&dec, ex4_encoded);

    for(int i=0; i<16; ++i) for(int j=0; j<16; ++j) {
        int x = ad_read_prob(&dec, EX4_S_HIGH-EX4_S_LOW);
//        printf("read %d %d: %d\n", i, j, x);
        ad_apply_range(&dec, x, x+1, EX4_S_HIGH-EX4_S_LOW);
        ex4_mat[i][j] = EX4_S_LOW + x;
    }
#if 1
    for(int i=0, block=0; i<5; ++i) {
        int s = 16<<i;
        for(int j=1; j<4; ++j, ++block) {
            int sy = s*(j&1), sx = s*(j>>1);
            ex4_init_block(block);
            for(int y=0; y<s; ++y) for(int x=0; x<s; ++x) {
                uint32_t p = ad_read_prob(&dec, TOTALPROB);
                int s = ex4_prob2sym(p);
//                printf("sym: %d\n", sym);
                uint32_t low = ex4_sym2prob(s);
                uint32_t hi = ex4_sym2prob(s+1);
//                printf("range: %u %u %u\n", low, hi, TOTALPROB);
                ad_apply_range(&dec, low, hi, TOTALPROB);
                ex4_add_sym(s);

                ex4_mat[sy+y][sx+x] = s;
            }
        }
    }
#endif

    output = ex4_output;
    printmat();

    int outf = openfile("c/kiel.arr",O_WRONLY|O_TRUNC|O_CREAT, 0644);
    writedata(outf, ex4_output, (int)(output-ex4_output));
}
char ex5_output[1<<20];
char ex5_input[1<<20];
double matvar(int i, int j) {
    int s = 16<<i;
    int sy = s*(j&1), sx = s*(j>>1);
    double r=0;
    for(int y=0; y<s; ++y) for(int x=0; x<s; ++x) {
        int v = ex4_mat[sy+y][sx+x];
        r += v*v;
    }
    return r / (s*s);
}
void ex5_extract() {
    int in = openfile("kiddog.arr",O_RDONLY,0);
    readdata(in, ex5_input, sizeof(ex5_input));
    input = ex5_input;
    int* inp=&ex4_mat[0][0];
    for(int i=0; i<512*512; ++i) *inp++ = readint();

#if 0
    const int T = 80;
    for(int i=0; i<512; ++i) {
        for(int j=0; j<512; ++j) {
            if (i<16 && j<16) continue;
            int x = ex4_mat[i][j];
            if (abs(x)<T) {
                x = 0;
            } else if (x<-T) x+=T;
            else x-=T;
            ex4_mat[i][j] = x;
        }
    }
#else
//    const int T = 100;
    double var0 = matvar(4,3);
    for(int i=0; i<5; ++i) {
        int s = 16<<i;
        double L = s*s;
        double J = 5;
        double B = sqrt(log(L/J));
        for(int j=1; j<4; ++j) {
            double var = matvar(i,j);
            double T = B * var0 / sqrt(var);
            int sy = s*(j&1), sx = s*(j>>1);
            for(int y=0; y<s; ++y) for(int x=0; x<s; ++x) {
                int z = ex4_mat[sy+y][sx+x];
                if (abs(z)<T) {
                    z = 0;
                } else if (z>0) z-=T;
                else z+=T;
                ex4_mat[sy+y][sx+x] = z;
            }
        }
    }
#endif

    output = ex5_output;
    printmat();
    int out = openfile("c/kd.arr",O_WRONLY|O_TRUNC|O_CREAT, 0644);
    writedata(out, ex5_output, (int)(output-ex5_output));
}

#ifndef DEBUG
void _start()
#else
int main()
#endif
{
    ex1_extract();
    ex2_extract();
    ex3_extract();
    ex4_extract();
    ex5_extract();
    //exit
#ifndef DEBUG
    asm ("xor %ebx, %ebx;mov $1, %eax;int $128;");
#else
    return 0;
#endif
}
