#include"dummymodel.hpp"
#include"countmodel.hpp"
#include"ppmmodel.hpp"

#include<stdlib.h>
#include<iostream>
#include<cassert>
using namespace ac;
void modelTest(Model& dm,Model& dm2){
    for(uint32_t i=0;i<=Eof;i++){
        //std::cout<<"--\ni = "<<i<<"\n";
        ProbPair p = dm.symbolRange(i);
        assert(p == dm2.symbolRange(i));
        assert(dm.symbolFromProb(p.first)==i);
        assert(dm.symbolFromProb(p.second-1)==i);
        assert(dm2.symbolFromProb(p.first)==i);
        assert(dm2.symbolFromProb(p.second-1)==i);
    }
}
const int TEXTN = 1000;

int main(){
    DummyModel dm;
    CountModel cm;
    ModelPtr models[]={
        ModelPtr(new DummyModel),
        ModelPtr(new DummyModel),
        ModelPtr(new CountModel),
        ModelPtr(new CountModel),
        createPPMModel(),
        createPPMModel()
    };
    ppm_test();
    const int MODELCOUNT = sizeof(models)/sizeof(models[0])/2;
    //AnnModel ann;
    for(int i=0;i<TEXTN;i++){
        std::cout<<i<<"/"<<TEXTN<<"\n";
        Symbol sym = rand()%Eof;
        for(int j=0;j<MODELCOUNT;j++)
        {
            models[j*2]->processSymbol(sym);
            models[j*2+1]->processSymbol(sym);
            modelTest(*models[j*2],*models[j*2+1]);
        }
    }
    for(int j=0;j<MODELCOUNT;j++)
    {
        models[j*2]->processSymbol(Eof);
        models[j*2+1]->processSymbol(Eof);
        modelTest(*models[j*2],*models[j*2+1]);
    }
}
