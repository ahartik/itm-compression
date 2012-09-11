#include"arithmetic.hpp"
#include"dummymodel.hpp"
#include<iostream>

using namespace ac;
int main(){
    ModelPtr dummy(new DummyModel());
    Encoder enc(dummy,&std::cerr,false);
    enc.writeSymbol(10);
    enc.writeSymbol(11);
    enc.writeSymbol(12);
    enc.writeSymbol(50);
    return 0;
}
