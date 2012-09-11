
#ifndef DUMMY_MODEL_HPP
#define DUMMY_MODEL_HPP

#include"model.hpp"
#include<cmath>
#include<iostream>
#include<cassert>
/*
 * Dummy model with 
 *
 */
namespace ac{
    const uint32_t Total = 1+258*259/2;
    class DummyModel: public Model{
        public:
            virtual Symbol symbolFromProb(Prob prob){
                //Prob = 
                //original
                Prob o = ((uint64_t)(prob+1)*Total+ProbMax-1)/ProbMax-1;
                int x = sqrt(o*2+0.25)-0.5;
                return Symbol(x);
            }
            virtual ProbPair symbolRange(Symbol symbol){
                assert(symbol <= Eof);
                Prob low = symbol*(symbol+1)/2;
                Prob up = low+symbol+1; 
                //up = (symbol+1)*(symbol+2)/2
                //   = (symbol+1)*symbol/2+2*(symbol+1)/2
                ProbPair ret(uint64_t(low)*ProbMax/Total,uint64_t(up)*ProbMax/Total);
                assert(ret.first < ret.second);
                assert(ret.second < ProbMax);
                return ret;
            }
            virtual void processSymbol(Symbol  symbol){
                //NOP
            }
            virtual ~DummyModel(){
            }
    };
}

#endif
