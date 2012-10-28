default:modeltest acencode acdecode encodetest

CXX=g++
CXXFLAGS=-Os -Wall
#LDFLAGS=-lfann
LDFLAGS=
MODEL_FILES=model.hpp countmodel.hpp countmodel.cpp ppmmodel.hpp ppmmodel.cpp
#annmodel.hpp annmodel.cpp
MODEL_OBJECTS=countmodel.o ppmmodel.o
#annmodel.o

countmodel.o:countmodel.cpp countmodel.hpp model.hpp
	${CXX} ${CXXFLAGS} -c $<

ppmmodel.o:ppmmodel.cpp ppmmodel.hpp model.hpp
	${CXX} ${CXXFLAGS} -c $<

modeltest:modeltest.cpp ${MODEL_FILES} ${MODEL_OBJECTS}
	${CXX} ${CXXFLAGS} modeltest.cpp ${MODEL_OBJECTS} -o modeltest ${LDFLAGS}

encodetest:encodetest.cpp arithmetic.hpp bitstream.hpp ${MODEL_FILES} ${MODEL_OBJECTS}
	${CXX} ${CXXFLAGS} -o encodetest  $< ${MODEL_OBJECTS} ${LDFLAGS}

acencode:acencode.cpp arithmetic.hpp bitstream.hpp ${MODEL_OBJECTS}  ${MODEL_FILES}
	${CXX} ${CXXFLAGS} -o acencode  $< ${MODEL_OBJECTS} ${LDFLAGS}

acdecode:acdecode.cpp arithmetic.hpp bitstream.hpp ${MODEL_OBJECTS}  ${MODEL_FILES}
	${CXX} ${CXXFLAGS} -o acdecode $< ${MODEL_OBJECTS} ${LDFLAGS}

	
.PHONY: clean

clean:
	rm -f *.o
	rm modeltest acencode acdecode encodetest
