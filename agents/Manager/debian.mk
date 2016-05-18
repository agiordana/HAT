CC = g++
FLAGS = -I. -I../minilibs -I../tinyxml -I/opt/include -g -DNDEBUG 
LIB = -lstdc++ -lpthread -lssl -lcrypto -lhttpserver -lmicrohttpd
OPTLIB = -L/opt/lib -lboost_system -lboost_thread -lboost_date_time -lboost_program_options -lboost_regex -lboost_date_time -lboost_random 
AGENTLIB = ../minilibs/*.o
OBJ = agent.o ProgramEngine.o TimedRuleEngine.o eventcounter.o ruleset.o timedruleset.o RuleEngine.o eventcounterset.o rule.o timedrule.o ActionEngine.o action.o actionset.o BooleanConditionSet.o BooleanCondition.o Configurator.o ConfRoutingMap.o

BIN = ../bin
XML = ../tinyxml/*.o

all:	$(OBJ) main.cpp agent.h 
	$(CC) $(FLAGS) $(OPTLIB) -o manager main.cpp $(AGENTLIB) $(OBJ) $(XML) $(LIB)

%.o:	%.cpp
	$(CC) -c $(FLAGS) $<

openssl/%.o : openssl/%.c
	$(CC) -c $<
	
install:	all
	cp manager $(BIN)/manager

clean:	
	rm *.o manager
