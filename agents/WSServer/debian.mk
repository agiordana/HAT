CC = g++
FLAGS = -O2 -I. -I../minilibs -I../tinyxml -I/opt/include -g -DNDEBUG
LIB = -lstdc++ -lpthread -lssl -lcrypto -lhttpserver -lmicrohttpd
OPTLIB = -L/opt/lib -lboost_system -lboost_thread -lboost_system -lboost_date_time -lboost_program_options -lboost_thread -lboost_regex -lboost_random -lboost_filesystem
AGENTLIB = ../minilibs/*.o
OBJ = agent.o AgentInterface.o NameTable.o RpcServer.o RpcRoutingMap.o TransTab.o Receiver.o ArchiveThread.o PushThread.o PushConnection.o RemoteServerThread.o DeviceManager.o AlarmManager.o AuthManager.o AutomatismManager.o LogManager.o UserLog.o Translator.o PwrManager.o SrvManager.o
BIN = ../bin
XML = ../tinyxml/*.o
JSON = ../libjson/Objects_static/*.o

all:	$(OBJ) main.cpp agent.h AgentInterface.h NameTable.h  RpcServer.h TransTab.h Receiver.h ArchiveThread.h PushThread.h RemoteServerThread.h PushConnection.h DeviceManager.h AlarmManager.h AuthManager.h AutomatismManager.h UserLog.h Translator.h LogManager.h PwrManager.h SrvManager.h
	$(CC) $(FLAGS) $(OPTLIB) -o wsserver main.cpp $(AGENTLIB) $(OBJ) $(XML) $(LIB) $(JSON)

%.o:	%.cpp
	$(CC) -c $(FLAGS) $<

openssl/%.o : openssl/%.c
	$(CC) -c $<
	
install:	all
	cp wsserver $(BIN)/wsserver

clean:	
	rm *.o wsserver
