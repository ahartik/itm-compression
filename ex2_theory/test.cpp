#include"pmath.hpp"
#include<vector>
#include<iostream>
#include<cstdio>

using namespace std;
int main()
{



    vector<ld> stocks[4];
    FILE* f = fopen("four-stocks.csv", "r");
    while(!feof(f))
    {
        ld v[4];
        fscanf(f,"%Lf,%Lf,%Lf,%Lf",v,v+1,v+2,v+3);
        for(int i=0;i<4;i++)
            stocks[i].push_back(v[i]);
    }
    fclose(f);

    ld max = 1<<16;
    ld slice = 1/max;
    ld pf = 1.0l-slice;
    ld pa = ld(1.0)/(1ULL<<32);

    ld one = 0;

    for(int i=-max/2;i<max/2;i++)
    {
        ld change = i/max;
        ld p = cdf(change+0.5*slice,0.001)-cdf(change-0.5*slice,0.001);
        one+=p;
    }
    cout<<"one = "<<one<<"\n";

    for(int s=0;s<4;s++)
    {
        ld var = 0.0001;
        ld last = stocks[s][0]*slice;
        ld codelen = 16;
        ld mc = 0;
        for(int i=1;i<stocks[s].size();i++)
        {
            ld scaled = stocks[s][i]/max;
            ld change = abs(last-scaled);
            ld p = cdf(change+0.5*slice,var)-cdf(change-0.5*slice,var);
            p = pf*p+pa;
            mc = std::max(change,mc);
            //cout<<"p = "<<p<<"\n";
            last = scaled;
            var = (var*i+change*change)/(i+1);
            codelen += -log2(p);
        }
        cout<<codelen<<" "<<mc<<" "<<max*sqrt(var)<<"\n";
    }
    cout<<"With delta^2:\n";
    for(int s=0;s<4;s++)
    {
        ld var = 0.0001;
        ld last = stocks[s][1]*slice;
        ld ldy = last-stocks[s][0]*slice;
        ld codelen = 32;
        ld mc = 0;
        for(int i=1;i<stocks[s].size();i++)
        {
            ld scaled = stocks[s][i]/max;
            ld projected = last+ldy;
            ld change = abs(projected-scaled);
            ld p = cdf(change+0.5*slice,var)-cdf(change-0.5*slice,var);
            p = pf*p+pa;
            mc = std::max(change,mc);
            //cout<<"p = "<<p<<"\n";
            ldy = scaled - last;
            last = scaled;
            var = (var*i+change*change)/(i+1);
            codelen += -log2(p);
        }
        cout<<codelen<<" "<<mc<<" "<<max*sqrt(var)<<"\n";
    }
    cout<<"With variance over last 1000:\n";
    for(int s=0;s<4;s++)
    {
        ld var = 0.0001;
        ld last = stocks[s][0]*slice;
        ld codelen = 16;
        ld mc = 0;
        for(int i=1;i<stocks[s].size();i++)
        {
            ld scaled = stocks[s][i]/max;
            ld change = abs(last-scaled);
            ld p = cdf(change+0.5*slice,var)-cdf(change-0.5*slice,var);
            p = pf*p+pa;
            mc = std::max(change,mc);
            last = scaled;
            int vi = i;
            if (vi > 5)vi = 5;
            int vd = vi+1;
            var = (var*vi+change*change)/(vd);
            codelen += -log2(p);
        }
        cout<<codelen<<" "<<mc<<" "<<max*sqrt(var)<<"\n";
    }
    return 0;
}
