CC = arm-linux-gnueabi-g++
HOME = /home/horus/x-tools/arm-unknown-linux-gnueabi/arm-unknown-linux-gnueabi
CFLAGS = -g -I. -I../tinyxml -I$(HOME)/boost_1_53_0/ -I$(HOME)/include -I$(HOME)/usr/lib/openssl-arm-unknown/include/ -DNDEBUG
LIB = -lstdc++ -lpthread -lboost_system
AGENTLIB = agentlib.o attribute.o Mattribute.o namelist.o filemanager.o httpserver.o logger.o net.o phone.o remoteagent.o period.o dayprogram.o program.o alarm.o hsrv.o iplocation.o message.o phone.o pwrprogram.o observer.o MMessage.o MRemoteAgent.o Trigger.o agentnet.o udpserver.o udpclient.o cipher.o
XML = ../tinyxml

all:	$(AGENTLIB)

observer.o: Observer.cpp Subject.cpp SubjectSet.cpp Subject.h Observer.h SubjectSet.h ObserverTCP.cpp ObserverTCP.h
	$(CC) $(CFLAGS) -c Observer.cpp Subject.cpp SubjectSet.cpp ObserverTCP.cpp 

%.o:	%.cpp %.h hsrv.h agentlib.h
	$(CC) -c $(CFLAGS) $<

clean:
	rm *.o
	rm *.gch
