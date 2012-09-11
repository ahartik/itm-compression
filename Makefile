
default:modeltest encode decode

CXX=g++
CXXFLAGS=-O3 -Wall
#LDFLAGS=-lfann
LDFLAGS=
MODEL_FILES=model.hpp dummymodel.hpp countmodel.hpp countmodel.cpp  triemodel.hpp triemodel.cpp
#annmodel.hpp annmodel.cpp
MODEL_OBJECTS=countmodel.o triemodel.o
#annmodel.o

countmodel.o:countmodel.cpp countmodel.hpp model.hpp
	${CXX} ${CXXFLAGS} -c countmodel.cpp

triemodel.o:triemodel.cpp triemodel.hpp model.hpp
	${CXX} ${CXXFLAGS} -c triemodel.cpp
	
#annmodel.o:annmodel.cpp annmodel.hpp model.hpp
#${CXX} ${CXXFLAGS} -c annmodel.cpp

modeltest:modeltest.cpp ${MODEL_FILES} ${MODEL_OBJECTS}
	${CXX} ${CXXFLAGS} modeltest.cpp ${MODEL_OBJECTS} -o modeltest ${LDFLAGS}

encode:encodetest.cpp arithmetic.hpp bitstream.hpp ${MODEL_FILES} 
	${CXX} ${CXXFLAGS} -o encode encodetest.cpp ${MODEL_OBJECTS} ${LDFLAGS}
decode:decodetest.cpp arithmetic.hpp bitstream.hpp ${MODEL_FILES} 
	${CXX} ${CXXFLAGS} -o decode decodetest.cpp ${MODEL_OBJECTS} ${LDFLAGS}
	

clean:
	rm -f *.o
	rm modeltest encode decode
