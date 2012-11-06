#include"pmath.hpp"
#include<vector>
#include<iostream>
#include<cstdio>

using namespace std;
int main()
{



    vector<ld> stock;
    FILE* f = fopen("stock1.dat", "r");
    while(!feof(f))
    {
        ld v[2];
        fscanf(f,"%Lf %Lf",v,v+1);
        stock.push_back(v[1]);
    }
    fclose(f);

    ld max = 1<<16;
    ld slice = 0.01;//1/max;
    ld pf = 1-1.0/50000;//1.0l-slice;
    ld pa = ld(1.0)/(1ULL<<30);

    ld one = 0;

    for(int i=-max/2;i<max/2;i++)
    {
        ld change = i/max;
        ld p = cdf(change+0.5*slice,0.001)-cdf(change-0.5*slice,0.001);
        one+=p;
    }
    cout<<"one = "<<one<<"\n";
    cout<<"cdf(-1,0,2) = "<<cdf(-1,2)<<"\n";
    cout<<"cdf(-1,0,sqrt(2)) = "<<cdf(-1,4)<<"\n";
    ld a0,a1,a2,a3,a4,a5;
    a0              = 0.171041     ;
    a1              = -0.000296173 ;
    a2              = 1.51966e-07  ;
    a3              = -3.22938e-11 ;
    a4              = 3.03209e-15  ;
    a5              = -1.04313e-19 ;
    cout << cdf(10,5) <<" == "<< cdf(1000,5*100*100)<<"\n";

    {
        ld var = 0.01;
        ld last = stock[0];
        ld codelen = 16;
        ld mc = 0;
        ld tp = 0;
        int learn = 10;
        for(long long i=1;i<stock.size();i++)
        {
            ld poly = a0+a1*i+a2*i*i+a3*i*i*i+a4*i*i*i*i+a5*i*i*i*i*i;
            ld scaled = stock[i];
            ld change =poly + scaled-last;//abs((last)-scaled);
            ld up = cdf(change+slice,var);
            ld down = cdf(change,var);
            ld p = up - down;
            p = pf*p+pa;
            //cout<<down<<" - "<<up<<"\tp = "<<p<<" "<<var<<"\n",
            mc = std::max(change,mc);
            last = scaled;
            var = (var*learn+change*change)/(learn+1);
            tp+=p;
            codelen += -log2(p);
        }
        cout<<"avg prob = "<<tp/stock.size()<<"\n";
        cout<<codelen<<" "<<mc<<" "<<sqrt(var)<<" "<<var<<"\n";
    }
    return 0;
}
