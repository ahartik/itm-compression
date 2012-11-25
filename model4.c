#pragma once

#include "model.h"
#include "pmath.h"
#include <stdint.h>

#define EX4_MIN_PROB 100
#define EX4_S_HIGH 6112
#define EX4_S_LOW 976
#define EX4_MIN (-1142)
#define EX4_MAX 1177
#define EX4_RANGE (EX4_MAX - EX4_MIN)
#define EX4_MAX_VALUE (EX4_RANGE*MIN_PROB)

#define EX4_GG
#include "model4_params.h"

#if 0
#define EX4_GAUSS_VAR 40.
#define EX4_GAUSS_RATIO 0.25f

#ifdef EX4_GG
const float betas[15] = {1.37056,1.71141,1.06758,0.955131,1.12707,1.14947,0.979452,0.823537,1.47109,1.20927,1.21333,1.75878,1.8126,1.50046,1.96758,};
const float alphas[15] = {194.984,458.965,100.827,65.5414,128.155,60.9546,46.2135,42.8572,50.9973,44.3452,46.9541,49.454,49.6979,48.1428,49.3484,};
#endif
#endif

static inline uint32_t ex4_sym2prob(double sym, ld alpha, ld beta) {
    sym -= .5;
    const long double fac = TOTALPROB - EX4_MAX_VALUE;
    ld g = cdf(sym/EX4_GAUSS_VAR);
#ifndef EX4_GG
    return MIN_PROB*(sym-EX4_MIN) + fac * g;
#else
    double gg = gcdf(sym/alpha, beta);
//    printf("gg %Lf %Lf %f\n", sym/alpha, beta, gg);
//    assert(gg>=0);
//    printf("diff %f\n", sym-mid);
    double r = EX4_GAUSS_RATIO;
    return MIN_PROB*(sym-EX4_MIN) + fac * (r*g + (1-r)*gg);
#endif
}
static inline int ex4_prob2sym(uint32_t prob, ld alpha, ld beta) {
#if 0
    int r=EX4_MIN;
    while(ex4_sym2prob(r, alpha, beta)<=prob) ++r;
    return r-1;
#else
    int low=EX4_MIN, hi=EX4_MAX;
    while(hi-low>1) {
        int mid = (low+hi)>>1;
        if (ex4_sym2prob(mid, alpha, beta) <= prob) {
            low = mid;
        } else {
            hi = mid;
        }
    }
    return low;
#endif
}
