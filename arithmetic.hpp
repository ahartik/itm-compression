#include<iostream>
#include<stdint.h>
#include<cassert>
#include<tr1/memory>

#include"bitstream.hpp"
#include"model.hpp"
namespace ac{
    
    static bool msbMatch(uint32_t a, uint32_t b){
        return (a>>31)==(b>>31);
    }
    static bool sndMatch(uint32_t a, uint32_t b){
        return (a>>30)==(b>>30);
    }
    void printBits(uint32_t l){
        for(int i=31;i>=0;i--){
            std::cerr<<((l>>i)&1);
        }
        std::cerr<<"\n";
    }
    
    static bool underflow(uint32_t l, uint32_t u){
        return ((l>>30)==1) && ((u>>30)==2);
    }

    struct Range{
        uint32_t lower;
        uint32_t upper;
        Range():
            lower(0),
            upper(~0)
        {
        }
        void applyProb(Prob l,Prob u){
            uint64_t range = uint64_t(upper)-lower+1;
            uint64_t scaled_low = (l * range) / ProbMax;
            uint64_t scaled_up =  (u * range) / ProbMax;
            upper = lower + scaled_up - 1 ;
            lower = lower + scaled_low;
        }
        void shift(){
            lower<<=1;
            upper<<=1;
            upper|=1;
        }
        void shift2(){
            upper<<=1;
            lower<<=1;
            upper|=1;
            //keep MSB:s
            lower&=~(1<<31);
            upper|=1<<31;
        }
        bool msbMatch()const{
            return ac::msbMatch(upper,lower);
        }
        bool underflow()const{
            return ac::underflow(lower,upper);
        }
        bool lowMsb()const{
            return lower>>31;
        }
        Prob readProb(uint32_t code) {
            uint64_t range = uint64_t(upper - lower) + 1;
            //assert(ProbMax <= range);
            uint64_t unscaled = uint64_t(code - lower) + 1;
            unscaled = unscaled*ProbMax;
            unscaled--;
            unscaled /= range;
            return Prob(unscaled);
        }
    };
    class Encoder{
        BitOutStream m_bout;
        size_t m_underflow;
        ModelPtr m_model;
        Range range;
        void encodeRange(Prob l,Prob u){
            range.applyProb(l,u);
            /*
            uint64_t range = uint64_t(m_upper)-m_lower+1;
            std::cerr<<"range="<<range<<"\n";
            std::cerr<<"ranges:\n";
            printBits(m_lower);
            printBits(m_upper);
            uint64_t scaled_low = (l * range) / ProbMax;
            uint64_t scaled_up =  (u * range) / ProbMax;
            m_upper = m_lower + scaled_up - 1 ;
            m_lower = m_lower + scaled_low;
            std::cerr<<"new ranges:\n";
            printBits(m_lower);
            printBits(m_upper);
            */
            while(range.msbMatch())
            {
                //m_bout.br();
                bool msb = range.lowMsb();
                m_bout.write(msb);
                m_bout.write(!msb,m_underflow);
                m_underflow = 0;
                range.shift();
            }
            //check underflows
            while(range.underflow())
            {
                m_underflow++;
                range.shift2();
            }
            //std::cerr<<"trimmed ranges:\n";
            //printBits(range.lower);
            ///printBits(range.upper);
        }
        public:
        Encoder(ModelPtr m,std::ostream* out,bool own):
            m_bout(out,own),
            m_underflow(0),
            m_model(m)
        {}

        void writeSymbol(Symbol sym){
            ProbPair range = m_model->symbolRange(sym);
            //std::cerr<<"write("<<range.first<<", "<<range.second<<")\n";
            encodeRange(range.first, range.second);
            m_model->processSymbol(sym);
        }
        ~Encoder(){
            m_bout.write((range.lower&(1<<30))!=0);
            m_underflow++;
            while(m_underflow-->0){
                m_bout.write((range.lower&(1<<30))==0);
            }
            m_bout.flush();
        }
    };

    class Decoder{
        Range range;
        uint32_t m_code;
        BitInStream m_bin;
        ModelPtr m_model;
        public:
        Decoder(ModelPtr model,std::istream* in, bool own):
            m_bin(in,own),
            m_code(0),
            m_model(model)
        {
            for(int i=0;i<32;i++)
            {
                m_code<<=1;
                m_code|=m_bin.read();
            }
        }
        void readBits(){
            while(range.msbMatch()){
                range.shift();
                bool b = m_bin.read();
                m_code<<=1;
                m_code|=b;
            }
            while(range.underflow()){
                range.shift2();

                bool b = m_bin.read();
                m_code<<=1;
                //swap msb
                m_code^=1<<31;
                m_code|=b;
                //assert(m_code < m_upper);
                //assert(m_code >= m_lower);
            }
        }


        Symbol readSymbol(){
            Prob p = range.readProb(m_code);
            //std::cerr<<"P = "<<p<<"\n";
            Symbol ret = m_model->symbolFromProb(p);
            ProbPair rng = m_model->symbolRange(ret);
            range.applyProb(rng.first, rng.second);
            readBits();
            m_model->processSymbol(ret);
            return ret;
        }
    };
};
