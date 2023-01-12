/*
 *  MQTT Arduino ketch (esp8266/esp32)
 *
 *  Processes the security system status and allows for control of all system aspects using MQTT.
 *
 * 
 * First copy all *.h and *cpp files from the /src/dscKeybusInterface directory to the same location
 * where you placed the sketch or into a subdirectory within your arduino libraries folder.
 * 
 *  Usage:
 *    1. Set the WiFi SSID and password in the sketch.
 *    2. Set the security system access code to permit disarming through your control software.
 *    3. Set the MQTT server address and other connection options in the sketch.
 *    4. Upload the sketch and monitor using a tool such as MQTT explorer.
 *    5. Setup your home control software to process the MQTT topics
 *
 * 

/*NOTE: Only use SSL with an ESP32.  The ESP8266 will get out of memory errors with the bear ssl library*/

//#define useMQTTSSL /*set this to use SSL with a supported mqtt broker.  */

#ifdef ESP32

#include <WiFi.h>

#include <mDNS.h>

#include <WiFiClientSecure.h>

#define LED_BUILTIN 13

#else

#include <ESP8266WiFi.h>

#include <ESP8266mDNS.h>

#endif

#include <string>

#include <PubSubClient.h>

#include <WiFiUdp.h>

#include <ArduinoOTA.h>
#include <ArduinoJson.h>

#define ARDUINO_MQTT
#include "dscAlarm.h"

#define DEBUG 1


// Settings
const char * wifiSSID = "<yourwifiaccesspoint>"; //name of wifi access point to connect to
const char * wifiPassword = "<yourwifipassword>";
const char * accessCode = "1234"; // An access code is required to arm (unless quick arm is enabled)
const char * otaAccessCode = "1234"; // Access code for OTA uploading
const char * mqttServer = "<yourmqttserveraddress>"; // MQTT server domain name or IP address
const char * mqttUsername = "<mqttuser>"; // Optional, leave blank if not required
const char * mqttPassword = "<mqttpass>"; // Optional, leave blank if not required

const int dscClockPin=22;
const int dscReadPin=21;
const int dscWritePin=18;

const int defaultPartition=1;
const int maxZones=32;
const int expanderAddr1=10;
const int expanderAddr2=11;

const char * userCodes = "1:User1,2:User2";

// MQTT topics
const char * mqttClientName = "DSCInterface";
const char * mqttZoneStatusTopic = "DSC/Get/Zone"; // Sends zone status per zone: DSC/Get/Zone1 ... DSC/Get/Zone64
const char * mqttRelayStatusTopic = "DSC/Get/Relay"; //relay status (relay1... relay2)

const char * mqttTroubleMsgTopic = "DSC/Get/Trouble"; // Sends trouble status
const char * mqttPanelTopic = "DSC/Get"; 
//const char * mqttLightsSuffix = "/StatusLights"; // battery/ac
const char * mqttPartitionTopic = "DSC/Get/Partition"; // Partition1 ... Partition2
const char * mqttStatusSuffix = "/Status"; // alarm/triggered ready/etc Partition1 ... Partition
const char * mqttFireSuffix = "/Fire"; // Sends fire status per partition: DSC/Get/Fire1/(on:off) ... DSC/Get/Fire8/(on:off)
const char * mqttZoneMsgTopic = "DSC/Get/ZoneExtStatus"; //zone extended statuses op/by/al
const char * mqttPartitionMsgSuffix = "/Message"; // Status messages for the partition
const char * mqttBeepSuffix = "/Beep"; // send beep counts for partition
const char * mqttLine1Suffix = "/DisplayLine1"; // send display line 1 for partition
const char * mqttLine2Suffix = "/DisplayLine2"; // send display line 1
const char * mqttEventTopic = "DSC/Get/Event"; // send event data for partition
const char * mqttBirthMessage = "online";
const char * mqttLwtMessage = "offline";
const char * mqttCmdSubscribeTopic = "DSC/Set"; // Receives messages to write to the panel


//End user config

#ifdef useMQTTSSL
const int mqttPort = 8883; // MQTT server ssl port
#else
const int mqttPort = 1883; // MQTT server port
#endif



#ifdef useMQTTSSL
WiFiClientSecure wifiClient;
wifiClient.setInsecure();
#else
WiFiClient wifiClient;
#endif

PubSubClient mqtt(mqttServer, mqttPort, wifiClient);
unsigned long mqttPreviousTime;
int lastLedState;

DSCkeybushome * DSCkeybus;

void setup() {

  Serial.setDebugOutput(true);
  Serial.begin(115200);
  Serial.println();
  pinMode(LED_BUILTIN, OUTPUT); // LED pin as output.
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSSID, wifiPassword);

  uint8_t checkCount = 20;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf("Connecting to Wifi..%d\n", checkCount);
    delay(1000);
    if (checkCount--) continue;
    checkCount = 50;
    WiFi.disconnect();
    WiFi.reconnect();

  }
  // Port defaults to 8266
  ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(mqttClientName);

  // No authentication by default
  ArduinoOTA.setPassword(otaAccessCode);

  ArduinoOTA.onStart([]() {
    dsc.stop();
    Serial.println(F("Start"));
  });
  ArduinoOTA.onEnd([]() {
    Serial.println(F("\nEnd"));
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println(F("Auth Failed"));
    else if (error == OTA_BEGIN_ERROR) Serial.println(F("Begin Failed"));
    else if (error == OTA_CONNECT_ERROR) Serial.println(F("Connect Failed"));
    else if (error == OTA_RECEIVE_ERROR) Serial.println(F("Receive Failed"));
    else if (error == OTA_END_ERROR) Serial.println(F("End Failed"));
  });
  ArduinoOTA.begin();
  Serial.println(F("Ready"));
  Serial.print(F("IP address: "));
  Serial.println(WiFi.localIP());
  
  mqtt.setCallback(mqttCallback);

  if (mqttConnect()) mqttPreviousTime = millis();
  else mqttPreviousTime = 0;

  Serial.println(F("DSC Interface is online."));
  
    DSCkeybus = new DSCkeybushome(dscClockPin,dscReadPin,dscWritePin);
    DSCkeybus->accessCode=accessCode;
    DSCkeybus->maxZones=maxZones;
    DSCkeybus->defaultPartition=defaultPartition;
    DSCkeybus->debug=2; // 0 = off, 1 = minimal, 2 = all packets shown on console  3 =  + serial port debugging
    DSCkeybus->expanderAddr1=expanderAddr1; //zone expander
    DSCkeybus->expanderAddr2=expanderAddr2;
    DSCkeybus->userCodes=userCodes;
    
    DSCkeybus->onSystemStatusChange([&](std::string statusCode) {
        mqttPublish(mqttPanelTopic,mqttStatusSuffix,statusCode.c_str());
    });
    
    DSCkeybus->onPartitionStatusChange([&](std::string statusCode, int partition) {
      mqttPublish(mqttPartitionTopic,mqttStatusSuffix,partition,statusCode.c_str());
    });
    
    DSCkeybus->onPartitionMsgChange([&](std::string msg,uint8_t partition) {
        mqttPublish(mqttPartitionTopic,mqttPartitionMsgSuffix,partition,msg.c_str());
    });      

    DSCkeybus->onPanelStatusChange([&](panelStatus ps,bool open,int partition) {
        
           char psvalue[10];
           strcpy(psvalue,"");           
           switch (ps) {
           case trStatus: strcat(psvalue,PSTR("/Trouble"));break;
           case batStatus: strcat(psvalue,PSTR("/Battery"));break;
           case acStatus: strcat(psvalue,PSTR("/AC"));break;
           case panicStatus: strcat(psvalue,PSTR("/Panic"));break;
           case rdyStatus: strcat(psvalue,PSTR("/Ready"));break;
           case armStatus: strcat(psvalue,PSTR("/Armed"));break;
            } 
           if (!partition) 
           mqttPublish(mqttPanelTopic,psvalue,open);
           else
           mqttPublish(mqttPartitionTopic,psvalue,partition,open);
    });
    DSCkeybus->onZoneMsgStatus([&](std::string msg) {
            mqttPublish(mqttZoneMsgTopic,msg.c_str());
    });
    DSCkeybus->onLine1Display([&](std::string msg,int partition) {
          mqttPublish(mqttPartitionTopic,mqttLine1Suffix,partition,msg.c_str());
    });
    DSCkeybus->onLine2Display([&](std::string msg,int partition) {
          mqttPublish(mqttPartitionTopic,mqttLine2Suffix,partition,msg.c_str());
    });
    DSCkeybus->onEventInfo([&](std::string msg) {
           mqttPublish(mqttEventTopic,msg.c_str());
    });  

    DSCkeybus->onBeeps([&](std::string beep,int partition) {
        mqttPublish(mqttPartitionTopic,mqttBeepSuffix,partition,beep.c_str());
    });    
    DSCkeybus->onFireStatusChange([&](bool open, int partition) {
           if (!partition) 
             mqttPublish(mqttPanelTopic,mqttFireSuffix,open);
           else
             mqttPublish(mqttPartitionTopic,mqttFireSuffix,partition,open);
    }); 
    DSCkeybus->onTroubleMsgStatus([&](std::string msg) {
            mqttPublish(mqttTroubleMsgTopic,msg.c_str());
    });
    DSCkeybus->onZoneStatusChange([&](uint8_t zone, bool open) {
        mqttPublish(mqttZoneStatusTopic,zone,open);

    });
        DSCkeybus->onRelayChannelChange([&](uint8_t channel, bool state) {
        mqttPublish(mqttRelayStatusTopic,channel,state);
    });
   
     DSCkeybus->begin(); 
}



void loop() {
  static int loopFlag = 0;
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin();
    if (loopFlag==0)
        Serial.println(F("\nWifi disconnected. Reconnecting..."));
    WiFi.disconnect();
    WiFi.reconnect();
    delay(1000);    
    loopFlag=1;
  } else 
      loopFlag=0;
  
  if (WiFi.status() == WL_CONNECTED) {
    ArduinoOTA.handle();
    mqttHandle();
  }
 /* 
  static unsigned long ledTime;
  if (millis() - ledTime > 1000) {
    if (lastLedState) {
      digitalWrite(LED_BUILTIN, LOW);
      lastLedState = 0;
    } else {
      digitalWrite(LED_BUILTIN, HIGH);
      lastLedState = 1;

    }
    ledTime = millis();
  }
  */
  DSCkeybus->loop();
  
} //loop


// Handles messages received in the mqttSubscribeTopic
void mqttCallback(char * topic, byte * payload, unsigned int length) {

  payload[length] = '\0';

  StaticJsonDocument <256> doc;
  deserializeJson(doc,payload);
       int p=defaultPartition;
       const char * s="";
       const char * c="";
      if (strcmp(topic, mqttCmdSubscribeTopic) == 0) { 
        if (doc.containsKey("partition"))
          p=doc["partition"];
        if (doc.containsKey("code"))
            c=doc["code"];     
        if (doc.containsKey("state") )  {
            s=doc["state"];  
            DSCkeybus->set_alarm_state(s,c,p); 
        } else if (doc.containsKey("keys")) {
            s=doc["keys"]; 
            DSCkeybus->alarm_keypress_partition(s,p);
        } else if (doc.containsKey("fault") && doc.containsKey("zone")) {
          int z=doc["zone"];
          bool f=doc["fault"] > 0?1:0;
          DSCkeybus->set_zone_fault(z,f);
        } else if (doc.containsKey("stop")) {
           disconnectKeybus(); 
        } else if (doc.containsKey("start") || doc.containsKey("restart")) {
            ESP.restart();
        }
        
      }    
}

void mqttHandle() {
 
  if (!mqtt.connected()) {
    unsigned long mqttCurrentTime = millis();
    if (mqttCurrentTime - mqttPreviousTime > 5000) {
      mqttPreviousTime = mqttCurrentTime;

      if (mqttConnect()) {
        Serial.println(F("MQTT disconnected, successfully reconnected."));
        mqttPreviousTime = 0;
      } else {
        Serial.println(F("MQTT disconnected, failed to reconnect."));
        Serial.print(F("Status code ="));
        Serial.print(mqtt.state());
      }
    }
  } else mqtt.loop();
}

bool mqttConnect() {
  Serial.print(F("MQTT...."));
  //if (mqtt.connect(mqttClientName, mqttUsername, mqttPassword, mqttSystemTopic, 0, true, mqttLwtMessage)) {
  if (mqtt.connect(mqttClientName, mqttUsername, mqttPassword)) {      
    Serial.print(F("connected: "));
    Serial.println(mqttServer);
    mqtt.subscribe(mqttCmdSubscribeTopic);
  } else {
    Serial.print(F("connection error: "));
    Serial.println(mqttServer);
  }
  return mqtt.connected();
}

void mqttPublish(const char * publishTopic,  const char * value) {
  mqtt.publish(publishTopic, value);

}

void mqttPublish(const char * topic, const char * suffix,  const char * value) {
  char publishTopic[strlen(topic) + 3 + strlen(suffix)];
  strcpy(publishTopic, topic);
  strcat(publishTopic, suffix);
  mqtt.publish(publishTopic, value);

}

void mqttPublish(const char * topic,const char * suffix , byte srcNumber,  const char * value) {
  char publishTopic[strlen(topic) + 10 + strlen(suffix)];
  char dstNumber[3];
  strcpy(publishTopic, topic);
  itoa(srcNumber, dstNumber, 10);
  strcat(publishTopic, dstNumber);  
  strcat(publishTopic, suffix);  
  mqtt.publish(publishTopic, value);
}

void mqttPublish(const char * topic,const char * suffix , byte srcNumber,  bool bvalue) {
  const char * value = bvalue ? "ON" : "OFF";
  char publishTopic[strlen(topic) + 10 + strlen(suffix)];
  char dstNumber[3];
  strcpy(publishTopic, topic);
  itoa(srcNumber, dstNumber, 10);
  strcat(publishTopic, dstNumber);  
  strcat(publishTopic, suffix);  
  mqtt.publish(publishTopic, value);
}

void mqttPublish(const char * topic, byte srcNumber,  bool bvalue) {
  const char * value = bvalue ? "ON" : "OFF";
  char publishTopic[strlen(topic) + 10];
  char dstNumber[3];
  strcpy(publishTopic, topic);
  itoa(srcNumber, dstNumber, 10);
  strcat(publishTopic, dstNumber);
  mqtt.publish(publishTopic, value);

}

void mqttPublish(const char * topic,const char * suffix , bool bvalue) {
  const char * value = bvalue ? "ON" : "OFF";
  char publishTopic[strlen(topic) + strlen(suffix)+2];
  strcpy(publishTopic, topic);
  strcat(publishTopic, suffix);  
  mqtt.publish(publishTopic, value);

}



