#ifndef Trie_MODEL_HPP
#define Trie_MODEL_HPP

#include"model.hpp"
#include<deque>
/*
 * Trie model with 
 *
 */
namespace ac{
    struct Trie;
    class TrieModel : public Model{
        Trie* trie;
        std::deque<Symbol> history;
        TrieModel(const TrieModel&);
        Trie* findTrie();
        public:
            TrieModel();
            virtual Symbol symbolFromProb(Prob prob);
            virtual ProbPair symbolRange(Symbol symbol);
            virtual void processSymbol(Symbol  symbol);
            virtual ~TrieModel();
    };
}

#endif
