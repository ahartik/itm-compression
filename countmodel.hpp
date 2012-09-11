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
        size_t get(Symbol x);
        size_t sum(Symbol x);
        size_t total;
        public:
            CountModel();
            virtual Symbol symbolFromProb(Prob prob);
            virtual ProbPair symbolRange(Symbol symbol);
            virtual void processSymbol(Symbol  symbol);
            virtual ~CountModel();
    };
}

#endif
