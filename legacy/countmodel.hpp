#ifndef COUNT_MODEL_HPP
#define COUNT_MODEL_HPP

#include"model.hpp"
#include<cmath>
#include<iostream>
#include<cassert>
/*
 * Count model with 
 *
 */
namespace ac{
    class CountModel: public Model{
        size_t tree[2*512];
        void add(Symbol x,int a=16);
        size_t get(Symbol x)const;
        size_t sum(Symbol x)const;
        size_t total;
        public:
            CountModel();
            virtual Symbol symbolFromProb(Prob prob)const;
            virtual ProbPair symbolRange(Symbol symbol)const;
            virtual void processSymbol(Symbol  symbol);
            virtual Prob totalProb()const;
            virtual ~CountModel();
    };
}

#endif
