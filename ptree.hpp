#ifndef PTREE_HPP
#define PTREE_HPP

#include"model.hpp"
namespace ac{

// 1
// 23
// 4567
// 89abcdef
// ...
    class PTree{
        uint64_t total;
        size_t tree[1024];
        public:
        size_t get(Symbol x);
        void add(Symbol x);
        size_t sum(Symbol x);

        PTree();
        Symbol symbolFromProb(Prob prob);
        ProbPair symbolRange(Symbol symbol);
    };
}
#endif
