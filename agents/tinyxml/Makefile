CXX    := g++

all:	tinyxml.o tinyxmlparser.o tinyxmlerror.o tinystr.o

tinystr.o:	tinystr.h
		$(CXX) -c tinystr.cpp

tinyxml.o: tinyxml.h tinystr.h
		$(CXX) -c tinyxml.cpp

tinyxmlparser.o: tinyxml.h tinystr.h
		$(CXX) -c tinyxmlparser.cpp

tinyxmlerror.o: tinyxml.h tinystr.h
		$(CXX) -c tinyxmlerror.cpp

clean:
	rm *.o
