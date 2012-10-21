#include"dummymodel.hpp"
#include"countmodel.hpp"

#include<stdlib.h>
#include<iostream>
#include<cassert>
using namespace ac;
void modelTest(Model& dm){
    for(uint32_t i=0;i<=Eof;i++){
        //std::cout<<"--\ni = "<<i<<"\n";
        ProbPair p = dm.symbolRange(i);
        assert(dm.symbolFromProb(p.first)==i);
        assert(dm.symbolFromProb(p.second-1)==i);
    }
}
const int TEXTN = 10000;

int main(){
    DummyModel dm;
    CountModel cm;
    Model* models[]={
        new DummyModel,
        new CountModel
    };
    const int MODELCOUNT = sizeof(models)/sizeof(models[0]);
    //AnnModel ann;
    for(int i=0;i<TEXTN;i++){
        std::cout<<i<<"/"<<TEXTN<<"\n";
        Symbol sym = rand()%Eof;
        for(int j=0;j<MODELCOUNT;j++)
        {
            models[j]->processSymbol(sym);
            modelTest(*models[j]);
        }
    }
    for(int j=0;j<MODELCOUNT;j++)
    {
        models[j]->processSymbol(Eof);
        modelTest(*models[j]);
    }
}
