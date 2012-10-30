#ifndef BITSTREAM_HPP
#define BITSTREAM_HPP
#include<cassert>
#include<istream>
#include<ostream>

// Set this to 1 to get ascii zeros and ones instead.
#define DEBUG 0

namespace ac{
    class BitOutStream{
        std::ostream* out;
        bool delete_out;
        unsigned int offset;
        unsigned char cur_byte;
        public:
        BitOutStream(std::ostream* nout, bool to_delete):
            out(nout),
            delete_out(to_delete),
            offset(0),
            cur_byte(0)
        {
        }

        void write(bool bit){
#if DEBUG
            (*out)<<int(bit);
            offset++;
            if(offset==8){
                (*out)<<" ";
                offset = 0;
            }

#else
            cur_byte|=bit<<offset;
            offset++;
            if (offset == 8){
                out->put(cur_byte);
                cur_byte = 0;
                offset = 0;
            }
#endif


        }
        void br(){
            (*out)<<"\n";
        }
        void write(bool bit,size_t count){
            for(size_t i=0;i<count;i++)write(bit);
        }
        void flush(bool pad=0){
            while(offset)
                write(pad);
        }

        ~BitOutStream(){
            if(delete_out)
                delete out;
        }
    };

    class BitInStream{
        std::istream* in;
        bool delete_in;
        unsigned char cur_byte;
        unsigned int offset;
        bool is_eof;
        public:
        BitInStream(std::istream* nin, bool to_delete):
            in(nin),delete_in(to_delete),offset(0),is_eof(0)
        { 
#if !DEBUG
            in->get(reinterpret_cast<char&>(cur_byte));
#endif
        }
        bool read(){
#if DEBUG
            char ret;
            (*in)>>ret;
            is_eof = !bool(*in);
            if(is_eof)return 0;
            assert(ret=='1' || ret=='0');
            return ret=='1';
#else
            bool ret = (cur_byte>>offset)&1;
            offset++;
            if(offset==8){
                if(!in->get(reinterpret_cast<char&>(cur_byte))){
                    is_eof = 1;
                    cur_byte = 0;
                }
                offset=0;
            }
#endif
            return ret;
        }
        bool eof()const{
            return is_eof;
        }
        ~BitInStream(){
            if(delete_in)
                delete in;
        }
    };

}
#endif
