
#pragma once

#include <cmath>

typedef long double ld;

ld cdf(ld x,ld d)
{
    return 0.5*(1+std::erf(x/sqrt(2*d)));
}

ld phi(ld x)
{
    // constants
    ld a1 =  0.254829592;
    ld a2 = -0.284496736;
    ld a3 =  1.421413741;
    ld a4 = -1.453152027;
    ld a5 =  1.061405429;
    ld p  =  0.3275911;

    // Save the sign of x
    int sign = 1;
    if (x < 0)
        sign = -1;
    x = fabs(x)/sqrt(2.0);

    // A&S formula 7.1.26
    ld t = 1.0/(1.0 + p*x);
    ld y = 1.0 - (((((a5*t + a4)*t) + a3)*t + a2)*t + a1)*t*exp(-x*x);

    return 0.5*(1.0 + sign*y);
}

