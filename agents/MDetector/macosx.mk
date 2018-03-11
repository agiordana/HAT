CC = g++
FLAGS = -O2 -I. -I../minilibs -I../tinyxml -I/opt/include -I/usr/local/include -I/usr/local/include/httpserver -g -DNDEBUG
LIB = -lstdc++ -lpthread -lssl -lcrypto -lhttpserver -lmicrohttpd 
OPTLIB = -L/opt/lib -lboost_system -lboost_thread -lboost_system -lboost_date_time -lboost_program_options -lboost_thread -lboost_regex -lboost_random -lboost_filesystem
AGENTLIB = ../minilibs/*.o
OBJ = agent.o MdFilter.o DirWatch.o
BIN = ../bin
XML = ../tinyxml/*.o

all:	$(OBJ) main.cpp agent.h MdFilter.h
	$(CC) $(FLAGS) $(OPTLIB) -o mdetector main.cpp $(AGENTLIB) $(OBJ) $(XML) $(LIB) 

%.o:	%.cpp
	$(CC) -c $(FLAGS) $<

openssl/%.o : openssl/%.c
	$(CC) -c $<
	
install:	all
	cp mdetector $(BIN)/soapagent

clean:	
	rm *.o mdetector
