#pragma once
#include<math.h>

typedef long double ld;

#if 0
ld igamma(ld a,ld z)
{
    ld ga = 0;
    const int N = 200;
    ld flog = 0;
    ld zlog = log(z);
    ld zplog = log(pow(z,a));
    ld sign = 1;

    for(int n=1;n<N;n++)
    {
        ga+=sign * exp(zplog-flog-log(n+a-1));
        zplog += zlog;
        flog += log(n);
        sign = -sign;
    }
    return ga;
}

#else
static ld igrecz(int,ld,ld);
static ld igrec1(int n, ld a,ld z)
{
    if (n == 20) return (n-a);
    return (n-a) / (1+igrecz(n,a,z));
}
static ld igrecz(int n, ld a,ld z)
{
    return (n) / (z+igrec1(n+1,a,z));
}
static ld igamma(ld a,ld z)
{
    return tgamma(a) - exp(-z)*pow(z,a) / (z+igrec1(1,a,z));
}

#endif

static inline long double gcdf(ld x,ld b) {
//    printf("cdf %Lf\n", x);
    double sgn = x < 0 ? -1 : 1;
    return 0.5+sgn*igamma(1/b,pow(abs(x), b))/(2*tgammal(1/b));
}

static inline long double cdf(ld x) {
//    printf("cdf %Lf\n", x);
    return .5 * (1 + erfl(x/sqrt(2)));
}
