CC = g++
CFLAGS = -g -I. -I../tinyxml -I/usr/local/include -DNDEBUG
AGENTLIB = agentlib.o attribute.o Mattribute.o namelist.o filemanager.o httpserver.o logger.o net.o phone.o remoteagent.o period.o dayprogram.o program.o alarm.o  hsrv.o iplocation.o message.o phone.o pwrprogram.o  MMessage.o MRemoteAgent.o Trigger.o agentnet.o udpserver.o udpclient.o cipher.o Observer.o Subject.o SubjectSet.o ObserverTCP.o ObserverThread.o

XML = ../tinyxml

all:	$(AGENTLIB)

modbusio.o: modbusio.cpp modbusio.h
	    $(CC) $(CFLAGS) -c modbusio.h modbusio.cpp 

observer.o: Observer.cpp Subject.cpp SubjectSet.cpp Subject.h Observer.h SubjectSet.h ObserverTCP.cpp ObserverTCP.h
	$(CC) $(CFLAGS) -c Observer.cpp Subject.cpp SubjectSet.cpp ObserverTCP.cpp 

%.o:	%.cpp %.h hsrv.h agentlib.h
	$(CC) -c $(CFLAGS) $<

clean:
	rm *.o
	rm *.gch
