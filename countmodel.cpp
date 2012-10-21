#include"countmodel.hpp"

#include<iostream>
#include<cstring>
namespace ac{

// 1
// 23
// 4567
// 89abcdef
// ...
size_t CountModel::get(Symbol x)const{
    return tree[x+512];
}
void CountModel::add(Symbol x,int a){
    int y = 512+x;
    total+=a;
    while(y){
        tree[y]+=a;
        y/=2;
    }
}
size_t CountModel::sum(Symbol x)const{
    int y = 512+x;
    int sum = 0;
    while(y){
        if(y%2)
            sum+=tree[y-1];
        y/=2;
    }
    return sum;
}

CountModel::CountModel():
    total(1)
{
    memset(tree,0,sizeof(tree));
    for(Symbol i=0;i<=Eof;i++){
        add(i,1);
    }
}
Symbol CountModel::symbolFromProb(Prob prob)const{
    size_t count = prob;
    int i=1;
    size_t c = 0;
    while(i<512){
        //std::cout<<"c = "<<c<<" i = "<<i<<"\n";
        i*=2;
        if(tree[i]+c<=count){
            c+=tree[i];
            i++;
        }
    }
    return Symbol(i-512);
}

ProbPair CountModel::symbolRange(Symbol symbol)const{
    assert(symbol <= Eof);
    Prob low = sum(symbol);
    Prob up = low+get(symbol); 
    //up = (symbol+1)*(symbol+2)/2
    //   = (symbol+1)*symbol/2+2*(symbol+1)/2
    ProbPair ret(low,up);
    assert(ret.first < ret.second);
    assert(ret.second <= ProbMax);
    return ret;
}

void CountModel::processSymbol(Symbol  symbol){
    add(symbol);
}

Prob CountModel::totalProb()const{
    return total;
}


CountModel::~CountModel(){
#if 0 
    for(int i=0;i<Eof;i++){
        if(isprint(i))
            std::cerr<<(unsigned char)(i)<<"->"<<get(i)<<"\n";
    }
#endif
}

}
