#include"arithmetic.hpp"
#include"dummymodel.hpp"
#include"countmodel.hpp"
#include"triemodel.hpp"
#include<iostream>

using namespace ac;
int main(){
    //ModelPtr dummy(new DummyModel());
    ///ModelPtr model(new CountModel());
    ModelPtr model(new TrieModel());
    //ModelPtr model(new AnnModel());
    Encoder enc(model,&std::cout,false);
    char c;
    while(std::cin.get(c))
    {
        enc.writeSymbol((unsigned char)c);
    }

    enc.writeSymbol(Eof);
    return 0;
}
