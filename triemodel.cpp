#include"triemodel.hpp"

#include<iostream>
#include<cstring>
#include<cassert>
namespace ac{

struct Trie
{
    Trie* c[256];
    uint64_t count;
    int childCount;
    Trie():
    count(0),
    childCount(0)
    {
        for(int i=0;i<256;i++)
            c[i]=NULL;
        //std::fill(c,c+256,NULL);
    }
    void addSub(Symbol s){
        if(s==Eof)return;
        assert(s<256);
        if(c[s]==NULL){
            c[s] = new Trie;
            childCount++;
        }
    }
    void addSubs(const std::deque<Symbol>& str, size_t i){
        if(i==str.size())return;
        Symbol s = str[i];
        addSub(s);
        count++;
        c[s]->addSubs(str,i+1);
    }
    Trie* find(const std::deque<Symbol>& hist, size_t i){
        if(i==hist.size())return this;
        if(c[hist[i]]==NULL)return NULL;
        return c[hist[i]]->find(hist,i+1);
    }
    Prob prob(Symbol sym)const{
        if(sym==Eof)return 1;
        if(c[sym]==0)return 1;
        return 1+c[sym]->count;
    }
    Prob total()const{
        return count+258;
    }
    void prune(){
        
    }
    ~Trie(){
        for(int i=0;i<256;i++)
            delete c[i];
    }
};

const size_t HistorySize = 32;

TrieModel::TrieModel():
    trie(new Trie)
{
    for(Symbol i=0;i<Eof;i++)
        trie->addSub(i);
}
Trie* TrieModel::findTrie(){
    for(size_t i=0;i<=history.size();i++){
        Trie* t = trie->find(history,i);
        if(t){
            if(t->count>2)return t;
        }
    }
    return trie;

}
Symbol TrieModel::symbolFromProb(Prob prob){
    Trie* t = findTrie();
    assert(t!=NULL);
    Prob total = t->total();
    size_t count = ((uint64_t)(prob+1)*total+ProbMax-1)/ProbMax-1;
    for(Symbol s=0;s<=Eof;s++){
        Prob p = t->prob(s);
        if(count<p){
            return s;
        }else{
            count-=p;
        }
    }
    assert(!"ERNO");
}

ProbPair TrieModel::symbolRange(Symbol symbol){
    assert(symbol <= Eof);

    Trie* t = findTrie();
    assert(t!=NULL);
    Prob low = 0;
    for(Symbol s=0;s<symbol;s++)
        low+=t->prob(s);
    Prob up =low+t->prob(symbol);
    Prob total = t->total();
    //up = (symbol+1)*(symbol+2)/2
    //   = (symbol+1)*symbol/2+2*(symbol+1)/2
    ProbPair ret(uint64_t(low)*ProbMax/total,uint64_t(up)*ProbMax/total);
    assert(ret.first < ret.second);
    assert(ret.second <= ProbMax);
    return ret;
}

void TrieModel::processSymbol(Symbol  symbol){
    history.push_back(symbol);
    if(history.size()>HistorySize)
        history.pop_front();
    for(int i=0;i<history.size();i++)
        trie->addSubs(history,i);
}

TrieModel::~TrieModel(){
    delete trie;
#if 0 
    for(int i=0;i<Eof;i++){
        if(isprint(i))
            std::cerr<<(unsigned char)(i)<<"->"<<get(i)<<"\n";
    }
#endif
}

}
