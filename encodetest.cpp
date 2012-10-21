#include"arithmetic.hpp"
#include"countmodel.hpp"
#include<cstdlib>
#include<fstream>
#include<iostream>
#include<sstream>

using namespace ac;
int main(){
    const int N = 1<<16;
    unsigned char input[N];
    for(int i=0;i<N;i++)
        input[i] = rand()%256;

    //encoding part
    {
        ModelPtr model(new CountModel());
        std::ofstream fout(".encoded");
        Encoder enc(model,&fout,false);
        for(int i=0;i<N;i++)
        {
            enc.writeSymbol(input[i]);

        }
        enc.writeSymbol(Eof);
    }
    //decoding part
    {
        ModelPtr model(new CountModel());
        std::ifstream fin(".encoded");
        Decoder dec(model,&fin,false);
        for(int i=0;i<N;i++)
        {
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
