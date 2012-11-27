#pragma once

#include "model.h"
#include "pmath.h"
#include <stdint.h>

#define EX4_MIN_PROB 70
#define EX4_S_HIGH 6112
#define EX4_S_LOW 976
#define EX4_MIN (-1142)
#define EX4_MAX 1177
#define EX4_RANGE (EX4_MAX - EX4_MIN)
#define EX4_MAX_VALUE (EX4_RANGE*EX4_MIN_PROB)

#define EX4_BOX_RADIUS 25
#define EX4_BOX_BONUS 0.001
#define EX4_BOX_MIX (2*EX4_BOX_RADIUS*EX4_BOX_BONUS)

//#define EX4_GG
//#define EX4_HISTOS
#include "model4_params.h"

static const float gdevs[15] = {195.452,362.49,134.572,105.016,159.407,72.0598,68.4516,88.515,44.6755,47.4234,50.098,37.6865,37.1555,41.2804,35.197,};
int curBlock;
#ifdef EX4_HISTOS
static double ex4_histo_arr[EX4_RANGE+1];
static double* ex4_histo = ex4_histo_arr - EX4_MIN;
static double ex4_histo_sum = 0;
static int ex4_pascal_level = 0;
#endif
static inline void ex4_init_block(int block)
{
    curBlock = block;
#ifdef EX4_HISTOS
    ex4_histo_sum = 0;
    ex4_pascal_level = 2 ; // (14-block)/5;
    for(int i=EX4_MIN; i <= EX4_MAX; i++)
    {
        ex4_histo[i] = 100000*
            (cdf((i+0.5)/gdevs[block]) - cdf((i-0.5)/gdevs[block]));
        ex4_histo_sum += ex4_histo[i];
    }
#endif
}
#if 0
char pascal[3][5] =
{
    {0,0,1,0,0},
    {0,1,2,1,0},
    {1,4,6,4,1}
};
#else
//char pascal[] = {1,6,15,20,15,6,1};
//char pascal[] = {1,7,21,35,35,21,7,1};
static const char pascal[] = {1,8,28,56,70,56,28,8,1};
#endif
static inline void ex4_add_sym(int sym)
{
#ifdef EX4_HISTOS
    for(int s = 0;s<9;s++){
        int add = 10*pascal[s];
        ex4_histo[sym+s-4]+=add;
        ex4_histo_sum+=add;
    }
#endif
}

static inline uint32_t ex4_sym2prob(double sym) {
//    sym -= .5;
    const long double fac = TOTALPROB - EX4_MAX_VALUE;
#ifdef EX4_HISTOS
    double cd = 0;
    for(int i = EX4_MIN; i < sym; i++)
        cd+=ex4_histo[i];
//    return (ld)TOTALPROB * cd / ex4_histo_sum;
    return EX4_MIN_PROB*(sym-EX4_MIN) + fac * cd / ex4_histo_sum;
#elif !defined(EX4_GG)
    ld g =  (1-EX4_BOX_MIX)*cdf(sym/gdevs[curBlock]);
    if (sym >= -EX4_BOX_RADIUS) {
        if (sym > EX4_BOX_RADIUS) g+=EX4_BOX_MIX;
        else g += EX4_BOX_BONUS*(sym+EX4_BOX_RADIUS);
    }
    return EX4_MIN_PROB*(sym-EX4_MIN) + fac * g;
#else
    double alpha = alphas[curBlock], beta = betas[curBlock];
    ld g = cdf(sym/EX4_GAUSS_VAR);
    double gg = gcdf(sym/alpha, beta);
//    printf("gg %Lf %Lf %f\n", sym/alpha, beta, gg);
//    assert(gg>=0);
//    printf("diff %f\n", sym-mid);
    double r = EX4_GAUSS_RATIO;
    return EX4_MIN_PROB*(sym-EX4_MIN) + fac * (r*g + (1-r)*gg);
#endif
}
static inline int ex4_prob2sym(uint32_t prob) {
#if 0
    int r=EX4_MIN;
    while(ex4_sym2prob(r)<=prob) ++r;
    return r-1;
#else
    int low=EX4_MIN, hi=EX4_MAX;
    while(hi-low>1) {
        int mid = (low+hi)>>1;
        if (ex4_sym2prob(mid) <= prob) {
            low = mid;
        } else {
            hi = mid;
        }
    }
    return low;
#endif
}
