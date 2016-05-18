CC = g++
FLAGS = -I. -I../minilibs -I../tinyxml -I/usr/local/include/boost -g -DNDEBUG
LIB = -lstdc++ -lpthread -lboost_system -lboost_thread -lssl -lcrypto -lboost_system -lboost_date_time -lboost_program_options -lboost_thread -lboost_regex
AGENTLIB = ../minilibs/*.o
OBJ = agent.o ProgramEngine.o TimedRuleEngine.o eventcounter.o ruleset.o timedruleset.o RuleEngine.o eventcounterset.o rule.o timedrule.o ActionEngine.o action.o actionset.o BooleanConditionSet.o BooleanCondition.o

BIN = ../bin
XML = ../tinyxml/*.o

all:	$(OBJ) main.cpp agent.h 
	$(CC) $(FLAGS) -o mngagentx main.cpp $(AGENTLIB) $(OBJ) $(XML) $(LIB)

%.o:	%.cpp
	$(CC) -c $(FLAGS) $<

openssl/%.o : openssl/%.c
	$(CC) -c $<
	
install:	all
	cp mngagentx $(BIN)/mngagent

clean:	
	rm *.o mngagentx
