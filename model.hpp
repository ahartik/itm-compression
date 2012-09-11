#ifndef MODEL_HPP
#define MODEL_HPP

#include<utility>
#include<stdint.h>
#include<tr1/memory>
namespace ac{
    typedef uint32_t Prob;
    typedef uint32_t Symbol;
    const uint32_t ProbLen = 30;
    const uint64_t ProbMax = 1<<ProbLen;
    typedef std::pair<Prob,Prob> ProbPair;

    const Symbol Eof = 256;
    class Model{
        public:
            virtual Symbol symbolFromProb(Prob prob) = 0;
            virtual ProbPair symbolRange(Symbol symbol) = 0;
            virtual void processSymbol(Symbol symbol) = 0;
            virtual ~Model(){
            }
    };
    typedef std::tr1::shared_ptr<Model> ModelPtr;
}
#endif
