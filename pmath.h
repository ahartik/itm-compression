#pragma once
#include<math.h>

typedef long double ld;

ld igamma(ld a,ld z)
{
    ld ga = 0;
    const int N = 20;
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

static inline long double gcdf(ld x,ld b) {
//    printf("cdf %Lf\n", x);
    double sgn = x < 0 ? -1 : 1;
    return 0.5+sgn*igamma(1/b,pow(abs(x), b))/(2*tgammal(1/b));
}

static inline long double cdf(ld x) {
//    printf("cdf %Lf\n", x);
    return .5 * (1 + erfl(x/sqrt(2)));
}
