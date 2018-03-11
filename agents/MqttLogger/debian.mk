CC = g++
FLAGS = -O2 -I. -I../minilibs -I../tinyxml -I/opt/include -g -DNDEBUG
LIB = -lstdc++ -lpthread -lssl -lcrypto -lhttpserver -lmicrohttpd -lmosquittopp -lmosquitto
OPTLIB = -L/opt/lib -lboost_system -lboost_thread -lboost_system -lboost_date_time -lboost_program_options -lboost_thread -lboost_regex -lboost_random -lboost_filesystem
AGENTLIB = ../minilibs/*.o
OBJ = agent.o Receiver.o MqttClientThread.o MqttClient.o
BIN = ../bin
XML = ../tinyxml/*.o
JSON = ../libjson/Objects_static/*.o

mqtt_logger:	$(OBJ) main.cpp agent.h Receiver.h MqttClientThread.h MqttClient.h
	$(CC) $(FLAGS) $(OPTLIB) -o mqtt_logger main.cpp $(AGENTLIB) $(OBJ) $(XML) $(LIB) $(JSON)

%.o:	%.cpp
	$(CC) -c $(FLAGS) $<

openssl/%.o : openssl/%.c
	$(CC) -c $<
	
install:	mqtt_logger
	cp mqtt_logger $(BIN)/mqtt_logger

clean:	
	rm *.o mqtt_logger
