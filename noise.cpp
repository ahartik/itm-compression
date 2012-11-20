#include <iostream>
#include <fstream>
#include <cmath>
#include <cassert>
#include <boost/math/special_functions/gamma.hpp>
#include "pmath.h"
using namespace std;
const int S = 512;
const int B = 16;

int mat[S][S];

#if 0
double ggpdf(double x, double a, double b) {
	return b/(2*a*gamma(1/b)) * exp(-pow(x/a, b));
}
double gpdf(double x, double v) {
	return exp(-x*x/(2*v)) / sqrt(2*M_PI*v);
}
#endif

const int VARS = 1;

const int M = 15;
int startx[M];
int starty[M];
int sizes[M];

#if 0
double eval(double v) {
	double p = 0;
	for(int i=0; i<S; ++i) {
		for(int j=0; j<S; ++j) {
			double x = mat[i][j];
			const double eps = 1e-6;
			p += -log(eps + (1-eps)*(cdf((x+.5)/sqrt(v)) - cdf((x-.5)/sqrt(v))));
		}
	}
	cout<<"eval done "<<p<<'\n';
	return p;
}
#endif
template<class F>
double tseek(double low, double hi, F fun, int iter=50) {
	for(int i=0; i<iter; ++i) {
		double a = (2*low+hi)/3, b = (low+2*hi)/3;
		if (fun(a) < fun(b)) {
			hi = b;
		} else {
			low = a;
		}
	}
	return low;
}
double prob(double x, double gaussVar, double gaussRatio, double curA, double curB) {
	double x0 = x-.5, x1 = x+.5;
	double g = cdf(x1/gaussVar) - cdf(x0/gaussVar);
	double gg = gcdf(x1/curA, curB) - gcdf(x0/curA, curB);
	assert(!isnan(gg));
//	cout<<"prob "<<x<<" : "<<g<<' '<<gg<<' '<<gaussRatio<<'\n';
	double eps = 1e-6;
	return eps + (1-eps) * (gaussRatio*g + (1-gaussRatio)*gg);
}
double evalBlock(int curBlock, double gaussVar, double gaussRatio, double alpha, double beta) {
	int s = sizes[curBlock];
	double r=0;
	for(int i=0; i<s; ++i) {
		for(int j=0; j<s; ++j) {
			int x = startx[curBlock] + j;
			int y = starty[curBlock] + i;
			assert(x>=0);
			assert(y>=0);
			assert(x<S);
			assert(y<S);
			double c = -log2(prob(mat[y][x], gaussVar, gaussRatio, alpha, beta));
//			cout<<"cc "<<mat[y][x]<<' '<<c<<'\n';
			assert(!isnan(c));
			r += c;
		}
	}
//	cout<<"evalblock "<<curBlock<<' '<<alpha<<' '<<beta<<" : "<<r<<'\n';
	assert(!isnan(r));
	return r;
}

struct BetaS {
	int cur;
	double gaussVar,gaussRatio,alpha;
	double operator()(double beta) {
		return evalBlock(cur, gaussVar, gaussRatio, alpha, beta);
	}
};
struct AlphaS {
	int cur;
	double gaussVar,gaussRatio;
	BetaS s;
	double operator()(double x) {
//		cout<<"alpha "<<x<<'\n';
		s = (BetaS){cur, gaussVar, gaussRatio, x};
		return s(tseek(0, 5, s, 10));
	}
};
struct BlockS {
	double gaussVar;
	AlphaS s;
	double operator()(double gaussRatio) {
		double r=0;
#pragma omp parallel for reduction(+:r)
		for(int i=0; i<15; ++i) {
			s = (AlphaS){i, gaussVar, gaussRatio};
			double c = s(tseek(0, 50, s, 10));
			assert(!isnan(c));
			cout<<"block cost "<<i<<" : "<<c<<'\n';
			r += c;
		}
		return r;
	}
};
struct GaussRatioS {
	BlockS s;
	double operator()(double x) {
		s = (BlockS){x};
		return s(tseek(0, 1, s));
	}
};

#if 0
double blockB(double beta) {
	curB = beta;
	return evalBlock();
}
double blockA(double alpha) {
	curA = alpha;
	cout<<"alpha: "<<alpha<<'\n';
	return blockB(tseek(0, 50, blockB, 10));
}

double varEval(double ratio) {
	gaussRatio = ratio;
	double r=0;
	for(int i=0; i<15; ++i) {
		curBlock = i;
		double c = blockA(tseek(0, 50, blockA, 10));
		cout<<"block cost "<<c<<'\n';
		r += c;
	}
	cout<<"vareval "<<gaussVar<<' '<<gaussRatio<<" : "<<r<<'\n';
	return r;
}
double varWeightEval(double var) {
	gaussVar = var;
	return varEval(tseek(0, 1, varEval));
}
#endif

double getVar() {
	GaussRatioS s;
	return tseek(0, 50, s);
//	return tseek(0, 50, varWeightEval);
}

int main() {
#if 0
	for(int i=1; i<20; ++i) {
		double a = i, b = 10;
		cout<<igamma(a, b)<<' '<<boost::math::gamma_p(a,b)<<'\n';
	}
	return 0;
#endif

	ifstream in("kiel.arr");
	for(int i=0; i<S; ++i) for(int j=0; j<S; ++j) in>>mat[i][j];

	for(int i=0, s=S/2; i<5; ++i, s/=2) {
		startx[3*i] = s;
		startx[3*i+1] = 0;
		startx[3*i+2] = s;
		starty[3*i] = 0;
		starty[3*i+1] = s;
		starty[3*i+2] = s;
		for(int j=0; j<3; ++j) sizes[3*i+j] = s;
	}

	cout<<getVar()<<'\n';
}
