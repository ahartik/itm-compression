
#include"ptree.hpp"
#include<iostream>
#include<cstring>
#include<cassert>
namespace ac{

// 1
// 23
// 4567
// 89abcdef
// ...
    size_t PTree::get(Symbol x){
        return tree[x+512];
    }
    void PTree::add(Symbol x){
        int y = 512+x;
        total++;
        while(y){
            tree[y]++;
            y/=2;
        }
    }
    size_t PTree::sum(Symbol x){
        int y = 512+x;
        int sum = 0;
        while(y){
            if(y%2)
                sum+=tree[y-1];
            y/=2;
        }
        return sum;
    }

    PTree::PTree():
        total(0)
    {
        memset(tree,0,sizeof(tree));
        for(Symbol i=0;i<=Eof;i++){
            add(i);
        }
    }
    Symbol PTree::symbolFromProb(Prob prob){
        size_t count = ((uint64_t)(prob+1)*total+ProbMax-1)/ProbMax-1;
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
    ProbPair PTree::symbolRange(Symbol symbol){
        assert(symbol <= Eof);
        Prob low = sum(symbol);
        Prob up = low+get(symbol); 
        //up = (symbol+1)*(symbol+2)/2
        //   = (symbol+1)*symbol/2+2*(symbol+1)/2
        ProbPair ret(uint64_t(low)*ProbMax/total,uint64_t(up)*ProbMax/total);
        assert(ret.first < ret.second);
        assert(ret.second <= ProbMax);
        return ret;
    }
}
