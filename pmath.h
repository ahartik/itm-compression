#pragma once
#include<math.h>

typedef long double ld;

#if 0
ld igamma(ld a,ld z)
{
//    return boost::math::gamma_p(s,x);
    return boost::math::tgamma_lower(s,x);
    ld ga = 0;
    const int N = 200;
    ld flog = 0;
    ld xlog = log(x);
    ld xplog = s * xlog;
    ld sign = 1;

    for(int n=0;n<N;n++)
    {
        printf("xx %Lf %Lf\n", xplog, flog);
        ga+=sign * exp(xplog-flog-log(n+s));
        xplog += xlog;
        flog += log(n+1);
        sign = -sign;
    }
    return ga;
}

#else
static ld igrecz(int,ld,ld);
static ld igrec1(int n, ld a,ld z)
{
    if (n == 50) return (n-a);
    return (n-a) / (1+igrecz(n,a,z));
}
static ld igrecz(int n, ld a,ld z)
{
    return (n) / (z+igrec1(n+1,a,z));
}
static ld igamma(ld a,ld z)
{
#ifdef __cplusplus
    return boost::math::tgamma_lower(a,z);
#endif
    return tgamma(a) - exp(-z)*pow(z,a) / (z+igrec1(1,a,z));
}

#endif

static inline ld gpdf(ld x, ld b) {
    return b * exp(-pow(x,b)) / (2*tgamma(1/b));
}
static inline ld gcdf2(ld x, ld b) {
    if (x<0) return 1-gcdf2(-x,b);
    const int n = 10;
    ld r = .5;
    ld eps = x/n;
    for(int i=0; i<n; ++i) {
        ld z = i*eps;
        r += eps * gpdf(z, b);
    }
    return r;
}

static inline long double gcdf(ld x,ld b) {
    if (fabs(x)<.2) return gcdf2(x,b);
//    printf("cdf %Lf\n", x);
//    if (x==0) return .5;
    double sgn = x < 0 ? -1 : 1;
    return 0.5+sgn*igamma(1/b,pow(fabs(x), b))/(2*tgammal(1/b));
}

static inline long double cdf(ld x) {
//    printf("cdf %Lf\n", x);
    return .5 * (1 + erfl(x/sqrt(2)));
}
