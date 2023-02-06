// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2022 Thomas Basler and others
 */
#include "MqttHandleVedirectHass.h"
#include "Configuration.h"
#include "MqttSettings.h"
#include "NetworkSettings.h"
#include "MessageOutput.h"

MqttHandleVedirectHassClass MqttHandleVedirectHass;

void MqttHandleVedirectHassClass::init()
{
}

void MqttHandleVedirectHassClass::loop()
{
    if (_updateForced) {
        publishConfig();
        _updateForced = false;
    }

    if (MqttSettings.getConnected() && !_wasConnected) {
        // Connection established
        _wasConnected = true;
        publishConfig();
    } else if (!MqttSettings.getConnected() && _wasConnected) {
        // Connection lost
        _wasConnected = false;
    }
}

void MqttHandleVedirectHassClass::forceUpdate()
{
    _updateForced = true;
}

void MqttHandleVedirectHassClass::publishConfig()
{
    if ((!Configuration.get().Mqtt_Hass_Enabled) ||
       (!Configuration.get().Vedirect_Enabled)) {
        return;
    }

    if (!MqttSettings.getConnected()) {
        return;
    }
    if (VeDirect.veMap.find("SER") == VeDirect.veMap.end()) {
        return;
    }

    publishBinarySensor("Load output state", "victron/LOAD", "ON", "OFF");

    yield();
}


void MqttHandleVedirectHassClass::publishBinarySensor(const char* caption, const char* subTopic, const char* payload_on, const char* payload_off)
{
    String serial = VeDirect.veMap["SER"];

    String sensorId = caption;
    sensorId.replace(" ", "_");
    sensorId.toLowerCase();

    String configTopic = "binary_sensor/dtu_victron_" + serial
        + "/" + sensorId
        + "/config";

    // String statTopic = MqttSettings.getPrefix() + serial + "/" + subTopic; // TODO extend with serial
    String statTopic = MqttSettings.getPrefix() + subTopic;

    DynamicJsonDocument root(1024);
    root[F("name")] = String("Victron ") + caption;
    root[F("uniq_id")] = serial + "_" + sensorId;
    root[F("stat_t")] = statTopic;
    root[F("pl_on")] = payload_on;
    root[F("pl_off")] = payload_off;

    JsonObject deviceObj = root.createNestedObject("dev");
    createDeviceInfo(deviceObj);

    char buffer[512];
    serializeJson(root, buffer);
    publish(configTopic, buffer);
}

void MqttHandleVedirectHassClass::createDeviceInfo(JsonObject& object)
{
    object[F("name")] = "Victron";
    object[F("ids")] = VeDirect.veMap["SER"];
    object[F("cu")] = String(F("http://")) + NetworkSettings.localIP().toString();
    object[F("mf")] = F("OpenDTU");
    object[F("mdl")] = VeDirect.getPidAsString(VeDirect.veMap["PID"].c_str());
    object[F("sw")] = AUTO_GIT_HASH;
}

void MqttHandleVedirectHassClass::publish(const String& subtopic, const String& payload)
{
    String topic = Configuration.get().Mqtt_Hass_Topic;
    topic += subtopic;
    MqttSettings.publishGeneric(topic.c_str(), payload.c_str(), Configuration.get().Mqtt_Hass_Retain);
}