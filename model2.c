#include "model.h"
#include <math.h>
#include <stdint.h>
static inline double cdf(double x) {
	return .5 * (1 + erf(x/sqrt(2)));
}
#define MAX_VALUE 50000

static inline uint32_t sym2prob(uint32_t sym, double mid, double var) {
	const double fac = TOTALPROB - MAX_VALUE;
	return sym + fac * cdf((sym-mid)/var);
}
static inline uint32_t prob2sym(uint32_t prob, double m, double var) {
	uint32_t low=0, hi=MAX_VALUE+1;
	while(hi-low>1) {
		int mid = (low+hi)>>1;
		if (sym2prob(mid, m, var) <= prob) {
			low = mid;
		} else {
			hi = mid;
		}
	}
	return low;
}
