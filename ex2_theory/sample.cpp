#include<iostream>
using namespace std;

int main()
{
    int tx;
    double ty;
    tx = ty = 0;
    const int SF = 100;
    while(cin.good())
    {
        double x,y;
        cin>>x>>y;
        tx+=1;
        ty+=y;
        if(tx == SF)
        {
            cout<<(x-SF/2)<<" "<<(ty/SF)<<"\n";
            tx = 0;
            ty = 0;
        }
    }
}
