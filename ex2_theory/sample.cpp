#include<iostream>
using namespace std;

int main()
{
    int tx;
    double ty;
    tx = ty = 0;
    const int SF = 100;
    double ly = -1;
    while(cin.good())
    {
        double x,y;
        cin>>x>>y;
        if (ly> 0 )
        {
            tx+=1;
            ty+=(y-ly);
        }
        ly = y;
        if(tx == SF)
        {
            cout<<(x-SF/2)<<" "<<(ty/SF)<<"\n";
            tx = 0;
            ty = 0;
        }
    }
}
