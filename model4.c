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
#define EX4_GAUSS_VAR 40

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
