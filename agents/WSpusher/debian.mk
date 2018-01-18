CC = g++
CFLAG = -std=c++11 -O3 -w -I. -I/opt/include -g -DNDEBUG
LIB = -lboost_iostreams -lboost_system -lboost_thread -lboost_regex -lpthread -lssl -lcrypto -lhttpserver -lmicrohttpd

BIN = ../bin

wspusher:	wspusher.cpp 
	$(CC) $(CFLAG) $(LIB) -o wspusher wspusher.cpp

install:	wspusher
	cp wspusher $(BIN)/wspusher

clean:
	wspusher
