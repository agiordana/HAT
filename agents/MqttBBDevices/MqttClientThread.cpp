//
//  MqttClientThread.cpp
//  logger
//
//  Created by Attilio Giordana on 8/27/14.
//  Copyright (c) 2014 Università del Piemonte Orientale. All rights reserved.
//

#include "MqttClientThread.h"

using namespace std;

MqttClientThread::MqttClientThread(string n) {
    id = n;
    vector<string> subjects;
    string threadpath = "threads/"+id;
    MParams params(threadpath);
    params.load();

    setof<MAttribute> topic = params.extractCategory("topic");
    for(size_t i=0; i<topic.size(); i++) {
       if(topic[i].value=="mqtt_sub") subjects.push_back(topic[i].name);
    }

    mosqpp::lib_init();
    mqttclient = new MqttClient("mqtt_bbdevice", "localhost", 1883, subjects);
}

MqttClientThread::~MqttClientThread() {
   mosqpp::lib_cleanup();
}

void MqttClientThread::start_service_threads() {
    this->service_thread.push_back(new Thread(&MqttClientThread::do_work, this));
    this->service_thread.push_back(new Thread(&MqttClientThread::do_work1, this));
}

void MqttClientThread::do_work(MqttClientThread* obj) {
    MMessage msg;
    while (true) {
        msg = obj->receive_message();
        if(msg.mtype == "mqtt" && msg.msubtype == "mqtt_send") {
    	    string pub_topic = msg.getString("topic");
	    string message = msg.getString("value");
            mqttclient->publish(pub_topic, message);
        }
    }
}

void MqttClientThread::do_work1(MqttClientThread* obj) {
  int rc;
  while(true){
           rc = mqttclient->loop();
           if(rc){
                 mqttclient->reconnect();
           }
        }
}
