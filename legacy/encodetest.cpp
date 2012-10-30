#include"arithmetic.hpp"
#include"ppmmodel.hpp"
#include"countmodel.hpp"
#include<cstdlib>
#include<fstream>
#include<iostream>
#include<sstream>

using namespace std;
using namespace ac;
ModelPtr createModel()
{
    return createPPMModel();
}

int main(){
    const int N = 1<<16;
    unsigned char input[N];
    for(int i=0;i<N;i++)
        input[i] = rand()%256;

    //encoding part
    {
        ModelPtr model = createModel();
        std::ofstream fout(".encoded");
        Encoder enc(model,&fout,false);
        for(int i=0;i<N;i++)
        {
            ProbPair pp = model->symbolRange(input[i]);
            enc.writeSymbol(input[i]);
        }
        enc.writeSymbol(Eof);
    }
    cout<<"decode"<<"\n";
    //decoding part
    {
        ModelPtr model = createModel();
        std::ifstream fin(".encoded");
        Decoder dec(model,&fin,false);
        for(int i=0;i<N;i++)
        {
            ProbPair pp = model->symbolRange(input[i]);
            Symbol sym = dec.readSymbol();
            assert(sym!=Eof);
            unsigned char c = sym;
            if (c != input[i])
            {
                std::cout<<"ERROR AT "<<i<<"\n";
                std::cout<<"Was "<<(int)c<<" expected "<<(int)input[i]<<"\n";
                assert(c==input[i]);
            }
        }
        Symbol sym = dec.readSymbol();
        assert(sym == Eof);
    }

    return 0;
}
