#pragma once
#include<math.h>

typedef long double ld;

ld igamma(ld s,ld x)
{
//    return boost::math::gamma_p(s,x);
    return boost::math::tgamma_lower(s,x);
    ld ga = 0;
    const int N = 2000;
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

static inline long double gcdf(ld x,ld b) {
//    printf("cdf %Lf\n", x);
    double sgn = x < 0 ? -1 : 1;
    return 0.5+sgn*igamma(1/b,pow(fabs(x), b))/(2*tgammal(1/b));
}

static inline long double cdf(ld x) {
//    printf("cdf %Lf\n", x);
    return .5 * (1 + erfl(x/sqrt(2)));
}
