#pragma once

#include "model.h"
#include "pmath.h"
#include <stdint.h>

#define EX4_MIN_PROB 5000
#define EX4_MAX_VALUE (500*MIN_PROB)
#define EX4_LEARN_RATE 10
#define EX4_LOW_LEARN_RATE 50
#define EX4_START_VALUE 5225
#define EX4_LOW_UNLEARN 200

static uint32_t ex4_sym2prob(uint32_t sym, double mid, ld var, ld beta,) {
    const long double fac = TOTALPROB - MAX_VALUE;
//    printf("diff %f\n", sym-mid);
    return MIN_PROB*sym + fac * cdf((sym-mid)/sqrtl(var));
}
static uint32_t ex4_prob2sym(uint32_t prob, double m, ld var) {
#if 1
    uint32_t r=0;
    while(sym2prob(r, m, var)<=prob) ++r;
    return r-1;
#else
    uint32_t low=0, hi=MAX_VALUE+1;
    while(hi-low>1) {
        uint32_t mid = (low+hi)>>1;
        if (sym2prob(mid, m, var) <= prob) {
            low = mid;
        } else {
            hi = mid;
        }
    }
    return low;
#endif
}
