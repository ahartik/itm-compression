#include"arithmetic.hpp"
#include"countmodel.hpp"
#include"ppmmodel.hpp"
#include<iostream>

using namespace ac;
int main(){
    //ModelPtr model(new CountModel());
    ModelPtr model = createPPMModel();
    Encoder enc(model,&std::cout,false);
    char c;
    while(std::cin.get(c))
    {
        enc.writeSymbol((unsigned char)c);
    }

    enc.writeSymbol(Eof);
    return 0;
}
