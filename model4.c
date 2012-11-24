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
#define EX4_GAUSS_VAR 40.f
#define EX4_GAUSS_RATIO 0.25f

const float betas[15] = {1.14989,1.65168,1.0686,0.902979,1.10112,1.0686,0.920591,0.82169,1.38698,1.17157,1.17157,1.72755,1.75735,1.44592,1.9328,};
const float alphas[15] = {158.643,446.424,100.958,59.8716,124.305,55.7026,42.0782,42.6776,48.5472,42.8565,45.2155,48.9007,48.7739,46.7502,48.8775,};

static inline uint32_t ex4_sym2prob(int sym, ld alpha, ld beta) {
    const long double fac = TOTALPROB - EX4_MAX_VALUE;
//    printf("diff %f\n", sym-mid);
    return MIN_PROB*(sym-EX4_MIN) + fac * cdf(sym/EX4_GAUSS_VAR);
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
