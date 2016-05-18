CC = g++
FLAGS = -O2 -I. -I../minilibs -I../tinyxml -I/opt/include -g -DNDEBUG
LIB = -lstdc++ -lpthread -lssl -lcrypto -lhttpserver -lmicrohttpd
OPTLIB = -L/opt/lib -lboost_system -lboost_thread -lboost_system -lboost_date_time -lboost_program_options -lboost_thread -lboost_regex -lboost_random -lboost_filesystem
AGENTLIB = ../minilibs/*.o
OBJ = Translator.o
BIN = ../bin
XML = ../tinyxml/*.o
JSON = ../libjson/Objects_static/*.o

all:	$(OBJ) test.cpp Translator.h
	$(CC) $(FLAGS) $(OPTLIB) -o test test.cpp $(AGENTLIB) $(OBJ) $(XML) $(LIB) $(JSON)

%.o:	%.cpp
	$(CC) -c $(FLAGS) $<

openssl/%.o : openssl/%.c
	$(CC) -c $<
	
install:	all
	cp test $(BIN)/test

clean:	
	rm *.o wsserver
