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
		double av=0, bv=0;
//#pragma omp parallel sections
		{
			{ av = fun(a); }
//#pragma omp section
			{ bv = fun(b); }
		}

//		if (fun(a) < fun(b)) {
		if (av < bv) {
			hi = b;
		} else {
			low = a;
		}
	}
	return low;
}
double prob(double x, double gaussVar, double gaussRatio, double curA, double curB) {
	double x0 = x-.5, x1 = x+.5;
//	double x0 = x-1, x1 = x;
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
//#pragma omp for reduction(+,r)
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
//	cout<<"evalblock "<<curBlock<<' '<<alpha<<' '<<beta<<' '<<s<<" : "<<r<<'\n';
	assert(!isnan(r));
	return r;
}
double getAlpha(int block, double beta) {
	int s = sizes[block];
	double sum=0;
	for(int i=0; i<s; ++i) for(int j=0; j<s; ++j) {
		int x = startx[block]+j, y = starty[block]+i;
		double v = mat[y][x];
		sum += pow(fabs(v), beta);
	}
	double a = pow(beta*sum/(s*s), 1/beta);
	return a;
}

struct BetaS {
	int cur;
	double gaussVar,gaussRatio;
	double operator()(double beta) {
		double alpha = getAlpha(cur, beta);
//		cout<<"alpha for "<<beta<<" : "<<alpha<<'\n';
		return evalBlock(cur, gaussVar, gaussRatio, alpha, beta);
	}
};
#if 0
struct AlphaS {
	int cur;
	double gaussVar,gaussRatio;
	double operator()(double x) {
//		cout<<"alpha "<<x<<'\n';
		BetaS s = {cur, gaussVar, gaussRatio, x};
		return s(tseek(0, 5, s, 10));
	}
};
#endif
struct BlockS {
	double gaussVar;
	double operator()(double gaussRatio) {
		double r=0;
#pragma omp parallel for reduction(+:r)
		for(int i=0; i<15; ++i) {
			BetaS s = {i, gaussVar, gaussRatio};
			double c = s(tseek(0, 5, s, 10));
			assert(!isnan(c));
			cout<<"block cost "<<i<<" : "<<c<<'\n';
			r += c;
		}
		cout<<"total cost "<<gaussVar<<' '<<gaussRatio<<" : "<<r<<'\n';
		cout.flush();
		return r;
	}
};
struct GaussRatioS {
	double operator()(double x) {
		BlockS s = {x};
		return s(tseek(0, 1, s, 10));
	}
};

double getVar() {
	GaussRatioS s;
	return tseek(0, 50, s);
//	return tseek(0, 50, varWeightEval);
}

double betas[15];

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

	for(int i=0; i<5; ++i) {
		int s = 16<<i;
		for(int j=0; j<3; ++j) {
			sizes[3*i+j] = s;
			startx[3*i+j] = s*((j+1)>>1);
			starty[3*i+j] = s*((j+1)&1);
		}
	}

	for(int k=0; k<15; ++k) {
		int s = sizes[k];
		double sum=0;
		for(int i=0; i<s; ++i) for(int j=0; j<s; ++j) {
			sum += mat[starty[k]+i][startx[k]+j];
		}
		cout<<"sum "<<k<<' '<<sum<<" ; "<<sum/(s*s)<<'\n';
	}

#if 0
	double var = getVar();
	cout<<"variance: "<<var<<'\n';
	double ratio = GaussRatioS()(var);
	cout<<"result: "<<var<<' '<<ratio<<'\n';
#else
	double var = 37, ratio = .25;
	double total = 0;
#pragma omp parallel for reduction(+:total)
	for(int i=0; i<15; ++i) {
		BetaS s = {i, var, ratio};
		double beta = tseek(0, 5, s, 20);
		double cost = s(beta);
		cout<<"beta "<<i<<" : "<<beta<<" ; "<<cost<<'\n';
		total += cost;
		betas[i] = beta;
	}
	cout<<"total "<<total<<'\n';

	ofstream out("model4_params.h");
	out<<"#pragma once\n";
	out<<"#define EX4_GAUSS_VAR "<<var<<'\n';
	out<<"#ifdef EX4_GG\n";
	out<<"#define EX4_GAUSS_RATIO "<<ratio<<'\n';
	out<<"static const float betas[15] = {"; for(int i=0; i<15; ++i) out<<betas[i]<<','; out<<"};\n";
	out<<"static const float alphas[15] = {"; for(int i=0; i<15; ++i) out<<getAlpha(i,betas[i])<<','; out<<"};\n";
	out<<"#endif\n";
#endif
}
