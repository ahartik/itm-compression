#include"arithmetic.hpp"
#include"dummymodel.hpp"
#include"countmodel.hpp"
#include<iostream>

using namespace ac;
int main(){
    //ModelPtr dummy(new DummyModel());
    ModelPtr model(new CountModel());
    //ModelPtr model(new AnnModel());
    Decoder dec(model,&std::cin,false);
    //for(int i=0;i<3;i++){
    for(;;)
    {
        Symbol sym = dec.readSymbol();
        if(sym==Eof)break;
        std::cout<<(unsigned char)sym;
    }
    return 0;
}
