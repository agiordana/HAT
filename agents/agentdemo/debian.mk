CC = g++
FLAGS = -O2 -I. -I../minilibs -I../tinyxml -I../libjson -I/opt/include -g -DNDEBUG
LIB = -lstdc++ -lpthread -lssl -lcrypto -lhttpserver -lmicrohttpd
OPTLIB = -L/opt/lib -L../libjson -ljson -lboost_system -lboost_thread -lboost_system -lboost_date_time -lboost_program_options -lboost_thread -lboost_regex -lboost_random -lboost_filesystem
AGENTLIB = ../minilibs/*.o
OBJ = agent.o DummySource.o DummyPit.o
BIN = ../bin
XML = ../tinyxml/*.o
JSON = ../libjson/Objects_static/*.o


all:	$(OBJ) main.cpp agent.h DummySource.h
	$(CC) $(FLAGS) $(OPTLIB) -o soapagentx main.cpp $(AGENTLIB) $(OBJ) $(XML) $(LIB)  $(JSON)

%.o:	%.cpp
	$(CC) -c $(FLAGS) $<

openssl/%.o : openssl/%.c
	$(CC) -c $<
	
install:	all
	cp soapagentx $(BIN)/soapagent

clean:	
	rm *.o soapagentx
