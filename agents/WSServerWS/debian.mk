CC = g++
FLAGS = -O2 -I. -I../minilibs -I../tinyxml -I/opt/include -g -DNDEBUG
LIB = -lstdc++ -lpthread -lssl -lcrypto -lhttpserver -lmicrohttpd
OPTLIB = -L/opt/lib -lboost_system -lboost_thread -lboost_system -lboost_date_time -lboost_program_options -lboost_thread -lboost_regex -lboost_random -lboost_filesystem
AGENTLIB = ../minilibs/*.o
OBJ = agent.o AgentInterface.o NameTable.o RpcServer.o RpcRoutingMap.o TransTab.o Receiver.o ArchiveThread.o PushThread.o JParser.o DeviceManager.o AlarmManager.o AlarmManagerSandA.o AlarmManagerTranslator.o AlarmManagerQueues.o AlarmManagerZones.o AlarmManagersrv.o AuthManager.o AuthManagerInit.o AuthManagerGet.o User.o AuthManagersrv.o AuthManagerUsersrv.o AutomatismManager.o AutomatismManagersrv.o LogManager.o UserLog.o Translator.o PwrManager.o PwrManagersrv.o SrvManager.o ConfManager.o GuiLib.o easywsclient.o
BIN = ../bin
XML = ../tinyxml/*.o
JSON = ../libjson/Objects_static/*.o

wsserverws:	$(OBJ) main.cpp agent.h JParser.h AgentInterface.h NameTable.h  RpcServer.h TransTab.h Receiver.h ArchiveThread.h PushThread.h DeviceManager.h AlarmManager.h AuthManager.h AutomatismManager.h UserLog.h Translator.h LogManager.h PwrManager.h SrvManager.h ConfManager.h
	$(CC) $(FLAGS) $(OPTLIB) -o wsserverws main.cpp $(AGENTLIB) $(OBJ) $(XML) $(LIB) $(JSON)

%.o:	%.cpp
	$(CC) -c $(FLAGS) $<

openssl/%.o : openssl/%.c
	$(CC) -c $<
	
install:	wsserverws
	cp wsserverws $(BIN)/wsserverws

clean:	
	rm *.o wsserverws
