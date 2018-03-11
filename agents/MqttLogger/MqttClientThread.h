//
//  MqttClientThread.h
//  logger
//
//  Created by Attilio Giordana on 24/02/18
//

#ifndef MQTT_CLIENT_THREAD
#define MQTT_CLIENT_THREAD
#include "agentlib.h"
#include "ObserverThread.h"
#include <openssl/rsa.h>
#include "MqttClient.h"

class MqttClientThread: public ObserverThread {
public:
    MqttClientThread(std::string name);
    
    virtual ~MqttClientThread();
    
    void start_service_threads();
    
private:
    
    MqttClient* mqttclient;
    
    void do_work(MqttClientThread* obj);
};

#endif
