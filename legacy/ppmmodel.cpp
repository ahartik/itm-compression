
#include"ppmmodel.hpp"
#include<deque>
#include<cmath>
#include<vector>
#include<iostream>
#include<tr1/unordered_map>
#include<cassert>
#include<deque>
#include<stdint.h>

namespace ac
{
    const size_t MaxContext = 3;
    typedef unsigned long long Hash;
    typedef unsigned long long LongProb;
    const int MaxHistory = 100000;
    static Hash string_hash(Hash h,Symbol c){
        //from boost::hash_combine
        //return h^(c + 0x9e3779b9 + (h << 6) + (h >> 2));
        return ((h * 31337)<<8)+c;
    }

    class HashBIT{
        typedef std::tr1::unordered_map<Hash, Prob> ProbMap;
        ProbMap map;
        Prob hg(Hash strHash, Symbol c)const
        {
            Hash h = string_hash(strHash, c);
            ProbMap::const_iterator it = map.find(h);
            if (it != map.end()){
                //std::cout<<"hg("<<h<<") = "<<it->second<<"\n";
                return it->second;
            }
            //std::cout<<"hg("<<h<<") = "<<0<<"\n";
            return 0;
        }
        void hi(Hash strHash, Symbol c, int k)
        {
            Hash h = string_hash(strHash, c);
            //std::cout<<"hi("<<h<<")\n";
            ProbMap::iterator it = map.find(h);
            if (it == map.end()){
                map[h] = k;
                return;
            }
            if(k < 0)
                assert(it->second >= Symbol(-k));

            it->second+=k;
            if(it->second == 0)
                map.erase(it);
        }
        public:
        void increase(Hash strHash, Symbol sym, int k)
        {
            int c = sym+1;
            assert(c < 258);
            for (; c <= 512; c += (c&-c))
            {
                hi(strHash, c, k);
            }
        }
        Prob sum(Hash strHash, Symbol sym)const{
            Prob su = 0;
            int c = sym+1;
            assert(c < 258);
            for (; c > 0; c -= (c & -c) ){
                //Hash h = string_hash(strHash, c);
                Prob p = hg(strHash,c);
                su += p;
                //std::cout<<"hg("<<c<<") = "<<p<<"\n";
            }
            return su; 
        }
        Prob maxProb(Hash strHash)const
        {
            return sum(strHash,256);
        }
    };

    class PPMModel : public ac::Model{
        std::vector<HashBIT> cbit;
        std::deque<unsigned char> history;
        std::deque<Hash> last;
            PPMModel(const PPMModel& o); // disabled
            PPMModel& operator=(PPMModel); // disabled
        LongProb longTotal;
        unsigned longTotalLog2;
        public:
            PPMModel();
            virtual Symbol symbolFromProb(Prob prob) const;
            virtual ProbPair symbolRange(Symbol symbol) const;
            virtual void processSymbol(Symbol symbol);
            virtual Prob totalProb() const;
            virtual ~PPMModel();

            Prob getSum(Symbol sym)const;
    };

    PPMModel::PPMModel()
    {
        cbit.resize(MaxContext);
        longTotal = Eof+2;
        longTotalLog2 = log2(longTotal)+1;
    }

    PPMModel::~PPMModel()
    {
    }

    Symbol PPMModel::symbolFromProb(Prob prob) const
    {
        Symbol left = 0;
        Symbol right = Eof+1;
        Symbol c;
        while(left+1 < right)
        {
            c = (left+right)/2;
            Prob sum = getSum(c);
            //std::cout<<"sum("<<c<<") = "<<sum<<"\n";
            if (sum <= prob)
                left = c;
            else
                right = c;
        }
        //std::cout<<"c = "<<c<<"\n";
        if(right <= Eof && prob >= getSum(left))
            left = right;

        return left;
    }

    ProbPair PPMModel::symbolRange(Symbol symbol) const
    {
        Prob start = getSum(symbol-1);

        Prob end = getSum(symbol);
        assert(start < end);
        return ProbPair(start, end);
    }

    void PPMModel::processSymbol(Symbol symbol)
    {
        last.push_front(0);
        while(last.size() > MaxContext)
            last.pop_back();
        for(size_t i=1;i<last.size();i++)
            last[i] = string_hash(last[i], symbol);
        for(size_t i=0;i<std::min(last.size(),MaxContext);i++)
        {
            cbit[i].increase(last[i], symbol, 1);
        }

        this->longTotal = Eof+2;
        for(size_t i = 0; i < std::min(last.size(),MaxContext);i++)
        {
            longTotal += (LongProb(cbit[i].maxProb(last[i])));
        }
        longTotalLog2 = log2(longTotal)+1;
        assert(totalProb() > symbolRange(Eof).first);
        assert(totalProb() ==  symbolRange(Eof).second);
    }

    Prob PPMModel::getSum(Symbol sym) const
    {
        if(sym < 0) return 0;
        if(sym == Eof)return totalProb();
        LongProb total = sym+1;

        for(size_t i = 0; i < std::min(last.size(),MaxContext);i++)
        {
            total += (LongProb(cbit[i].sum(last[i],sym)) );
        }

        if (ProbLen < longTotalLog2)
            return total >> (longTotalLog2 - ProbLen);
        return total;
    }

    Prob PPMModel::totalProb() const
    {
        assert(longTotal > 0 );
        if (ProbLen < longTotalLog2)
            return longTotal >> (longTotalLog2 - ProbLen);
        return longTotal;
    }
    ModelPtr createPPMModel()
    {
        return ModelPtr(new PPMModel());
    }
    void ppm_test()
    {
        HashBIT tb;
        tb.increase(0,3,1);
        tb.increase(0,128,1);
        tb.increase(0,198,1);
        assert(tb.sum(0,255)==3);
        std::cout<<"totalProb = "<<tb.maxProb(0)<<"\n";
        assert(tb.maxProb(0) == 3);
    }
}
