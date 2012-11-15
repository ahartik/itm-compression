#include "model.h"
#include <math.h>
#include <stdint.h>
static inline long double cdf(ld x) {
//    printf("cdf %Lf\n", x);
    return .5 * (1 + erfl(x/sqrt(2)));
}
#define MIN_PROB 5000
#define MAX_VALUE (500*MIN_PROB)
#define LEARN_RATE 10
#define LOW_LEARN_RATE 50
#define START_VALUE 5225
#define LOW_UNLEARN 200

static uint32_t sym2prob(uint32_t sym, double mid, ld var) {
    const long double fac = TOTALPROB - MAX_VALUE;
//    printf("diff %f\n", sym-mid);
    return MIN_PROB*sym + fac * cdf((sym-mid-.5f)/sqrtl(1.25f*var));
}
static uint32_t prob2sym(uint32_t prob, double m, ld var) {
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
