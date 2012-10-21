
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
    class DummyModel : public Model
    {
        public:
            virtual Symbol symbolFromProb(Prob prob) const
            {
                Prob o = prob;
                int x = sqrt(o*2+0.25)-0.5;
                return Symbol(x);
            }
            virtual ProbPair symbolRange(Symbol symbol) const
            {
                assert(symbol <= Eof);
                Prob low = symbol*(symbol+1)/2;
                Prob up = low+symbol+1; 
                ProbPair ret(low,up);
                assert(ret.first < ret.second);
                assert(ret.second < ProbMax);
                return ret;
            }
            virtual Prob totalProb() const
            {
                return Total;
            }
            virtual void processSymbol(Symbol  symbol)
            {
            }
            virtual ~DummyModel()
            {
            }
    };
}

#endif
