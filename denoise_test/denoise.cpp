#include<iostream>
#include<cmath>
#include<vector>

using namespace std;

int mat[512][512];

int cllevel[5] = {400,300,200,200,200};
void denoise(int x,int y,int w,int h){
    if(x==0 && y == 0){
        if(w==16)return;
        denoise(x,y,w/2,h/2);
        denoise(x+w/2,y,w/2,h/2);
        denoise(x,y+h/2,w/2,h/2);
        denoise(x+w/2,y+h/2,w/2,h/2);
        return;
    }
    int cl = 400;
    for(int i=x;i<x+w;i++)
        for(int j=y;j<y+h;j++)
        {
            //if(mat[i][j]<-cl)mat[i][j]=-cl;
            //if(mat[i][j]>cl)mat[i][j]=cl;
            double d = abs(mat[i][j]) / cl;
            if(d<1) mat[i][j] *= d*d;
        }
}

int main()
{
    for(int i=0;i<512;i++)
        for(int j=0;j<512;j++)
            cin>>mat[i][j];


    denoise(0,0,512,512);
    
    for(int i=0;i<512;i++){
        for(int j=0;j<512;j++)
            cout<<mat[i][j]<<" ";
        cout << "\n";
    }
}
