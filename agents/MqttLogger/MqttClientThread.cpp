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
    mosqpp::lib_init();
    mqttclient = new MqttClient("mqtt_logger", "localhost", 1883, subjects);
}

MqttClientThread::~MqttClientThread() {
   mosqpp::lib_cleanup();
}

void MqttClientThread::start_service_threads() {
    this->service_thread.push_back(new Thread(&MqttClientThread::do_work, this));
}

void MqttClientThread::do_work(MqttClientThread* obj) {
    MMessage msg;
    while (true) {
        msg = obj->receive_message();
        if(msg.mtype == "mqtt" && msg.msubtype == "mqtt_send") {
    	    string topic = "event";
	    string message = msg.xmlEncode(0);
            mqttclient->publish(topic, message);
        }
    }
}

