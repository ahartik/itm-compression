#define STDIN 0
#include "read.h"
#define N (100*1000)

const uint32_t totalprob = 1073741824;
// Probability that x0==0 given x1..x4
const uint32_t cprobs[16] = {797188835,193951189,131034232,639237548,545077926,78034203,49537144,386161857,953843534,397327946,298056828,846205568,787748553,190671424,127947857,625238763,};

uint32_t nextProb() {
	char data[8];
	readdata(STDIN, data, 8);
	int r=0;
	for(int i=0; i<4; ++i) {
		r += (data[2*i]-'0')<<i;
	}
	return cprobs[r];
}
