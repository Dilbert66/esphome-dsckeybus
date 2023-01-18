/*
    Virtual Keypad/Telegram client for the DSC Power832 alarm system family and variants  (ESP32 Only)
    
    Provides a virtual keypad web interface and push notification  using the ESP32 as a standalone web server using
    AES encrypted web socket communications. All keypad functionality provided.
    
    This sketch uses portions of the code from the VirtualKeypad-Web example for DSC alarm systems found in the
    taligent/dscKeybusInterface respository at:
   https://github.com/taligentx/dscKeybusInterface/blob/master/examples/esp32/VirtualKeypad-Web/VirtualKeypad-Web.ino.
   
    It was adapted to use the DSC alarm system library at: 
   https://github.com/Dilbert66/esphome-dsckeybus/tree/dev/dscKeybusInterface, with the addition of two way 
    AES encryption for the web socket and push notification capability using the Telegram (https://telegram.org)
    messaging application due to it's ability to also provide control of your system remotely using bots in addition 
    to having push capability.  All this at no cost!

   Usage:
     1. Copy the sketch file VirtualKeypad-DSC.ino and telegram_async.h/.cpp files into a new sketch directory of the same name.
   
   If using the Virtualkeypad (#define  VIRTUALKEYPAD):

     2. Install the following libraries directly from each Github repository:
           ESPAsyncWebServer: https://github.com/me-no-dev/ESPAsyncWebServer
           AsyncTCP: https://github.com/me-no-dev/AsyncTCP
 
     3. Install the filesystem uploader tools to enable uploading web server files:
          https://github.com/me-no-dev/arduino-esp32fs-plugin
 
     4. Install the following libraries, available in the Arduino IDE Library Manager and
        the Platform.io Library Registry:
          AESLib: https://github.com/suculent/thinx-aes-lib
 
     5. If desired, update the DNS hostname in the sketch.  By default, this is set to
        "dsckeypad" and the web interface will be accessible at: http://dsckeypad.local


 

     If not using the VirtualKeypad, start at here step 6. Comment out the #define VIRTUALKEYPAD line.
     
      
     6.  Install the ArduinoJson library available in the Arduino IDE Library Manager and
        the Platform.io Library Registry:  https://github.com/bblanchon/ArduinoJson    
        
     7. Copy all .h and cpp files from the repository at: https://github.com/Dilbert66/esphome-dsckeybus/tree/dev/dscKeybusInterface
       to your sketch directory or into a subdirectory within your arduino libraries folder.        
        
     8.  Set all configuration variables in the sketch to match your local setup such as WIFI settings, password, teleggramids, aes password,etc.    
     
     9. Compile and upload the sketch. Recommended to use board "ESP32 Dev Module with Minimal SPIFFS partition scheme (190K SPIFFS   partition) to get the maximum flash storage for program storage if using OTA.    

     if using the VirtualKeypad:
     10. Upload the "data" directory (from the same directory as the .ino file) containing the web server files  to the SPIFFS partition:
          Arduino IDE: Tools > ESP32 Sketch Data Upload

       
     11. Configure Telegram:   
     
    a. Start a  conversation with @BotFather or go to url: https://telegram.me/botfather
    b. Create a new bot using BotFather: /newbot
    c. Copy the bot token to the telegramBotToken config variable in this sketch
    d. Start a conversation with the newly created bot to open the chat.
    e. Start a conversation with @myidbot, or go to url: https://telegram.me/myidbot to get your chat id.
    f. Get your user chat ID: /getid
    g. Copy the user chat ID to this sketch in the telegramUserID config variable.
    

    12. Once the sketch is loaded and running, any following updates can be done via OTA updates for initial testing.  
        See here for an example:  https://randomnerdtutorials.com/esp8266-ota-updates-with-arduino-ide-over-the-air/
        
        NOTES: 
        a. I do not normally recommended leaving the ability to do OTA updates active on a production system. Once done testing, you should either disable it by commenting out "useOTA" or set a good long passcode.
        Be aware that for uploading sketch data (web server files) via OTA, you cannot have a password set. Once all testing is done, you can then set your password of choice or disable the feature. 
        
        b. You can access the virtual keypad web interface by the IP address displayed through
        the serial output or http://dsckeypad.local (for clients and networks that support mDNS).
        You can also talk to your telegram bot from the bot chat window created above. Send /help for a list of commands. On boot, the system will send all status to your bot channel.
      
       
     
*/

#define VIRTUALKEYPAD  //comment if you do not want/need the virtualkeypad functionality
#define useOTA //comment this to disable OTA updates. 

#include <Arduino.h>

#ifdef useOTA
#include <ArduinoOTA.h>
#endif

#if defined(VIRTUALKEYPAD)
#include <ESPmDNS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AESLib.h>
#endif

#include <SPIFFS.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <string>
#include <list>


#define DEBUG 1

#define ARDUINO_MQTT
#include "dscAlarm.h"
#include "telegram_async.h" //telegram notify full async plugin with inbound bot cmd capability

//start user config



const char * wifiSSID = ""; //name of wifi access point to connect to
const char * wifiPassword = "";
const char * accessCode = "1234"; // An access code is required to arm (unless quick arm is enabled)
const char * otaAccessCode = ""; // Access code for OTA uploading
const char * clientName = "vistaKeypad"; //WIFI client name
const char * password = "!YourSecretPass123"; // login and AES encryption/decryption password. Up to 16 characters accepted.
const char * telegramBotToken=""; // Set the Telegram bot access token
const char * telegramUserID="1234567890"; // Set the default Telegram chat recipient user/group ID
const char * telegramMsgPrefix="[Alarm Panel] "; // Set a prefix for all messages
std::list<String> telegramAllowedIDs = {}; //list of additional telegram ids with access to bot.  Can include channel id.
std::list<int> notifyZones = {}; //comma separated list of zones that you want push notifications on change

const int dscClockPin=22;
const int dscReadPin=21;
const int dscWritePin=18;

const int defaultPartition=1;
const int maxPartitions=3;
const int maxZones=32;

/*
  #zone expander addresses:
  # 9  - zones 9-16
  # 10 - zones 17-24
  # 11 - zones 25-32
  # 12 - zones 33-40 (for systems with 64 zone support)
  # 13 - zones 41-48 (for systems with 64 zone support)
  # 14 - zones 49-56 (for systems with 64 zone support)
  # 16 - zones 57-64 (for systems with 64 zone support)  
  */
const int expanderAddr1=8; //1st zone expander emulator address to use . Set to 0 to disable. 
const int expanderAddr2=0;

const char * userCodes = "1:User1,2:User2,40:master";

uint8_t notificationFlag=1+2+4; //which events; bit 1=zones, bit 2=status, bit 3= events, bit 4 = messages, bit 5=light states

//end user config



const char * const telegramMenu[] PROGMEM ={"/help - this command","/armstay - arm in stay mode","/bypass - turn on full bypass","/reboot - reboot esp","/!<keys> - send cmds direct to panel","/getstatus - get zone/system/light statuses","/getstats - get memory useage stats","/stopbus - stop dsc bus","/startbus - start dsc bus","/stopnotify - pause notifications","/startnotify - unpause notifications","/setpartition=<partition> - set default partition","/&<p><keys> - send cmds to partition p","/addzones=<zone>,<zone> - add zones to notify list","/removezones=<zone>,<zone> - remove zones from notify list","/addids=<id>,<id> - add telegram ids to allowed control list","/removeids=<id>,<id> - remove telegram ids from allowed list","/getip - get url of keypad","/getcfg - list notify and telegram ids","/setnotifyflag=<flag> - sum of digits: zones = 1 , status = 2 , messages = 4 , events = 8 , light statues = 16"};


std::string accessCodeStr=accessCode;
#if defined(VIRTUALKEYPAD)
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
#endif

unsigned long pingTime;
bool pauseNotifications;
uint8_t activePartition=defaultPartition;

struct 
PushLib pushlib(telegramBotToken, telegramUserID, telegramMsgPrefix);

DSCkeybushome * DSCkeybus;

std::string key = std::string(password).append(16 - key.length(), '0');
#if defined(VIRTUALKEYPAD)
AES aes;
char * aeskey = & key[0];

byte ivaes[N_BLOCK] = {
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0
};
#endif 

bool inListTelegramID(String id) {

    auto it = std::find(telegramAllowedIDs.begin(), telegramAllowedIDs.end(), id);
    return it == telegramAllowedIDs.end()?false:true;

}

bool inListZone(byte zone) {

    auto it = std::find(notifyZones.begin(), notifyZones.end(), zone);
    return it == notifyZones.end()?false:true;

}

void pushNotification(String text, String receiverid = "") {
   
  StaticJsonDocument < 300 > doc;
  doc["chat_id"] = receiverid != "" ? receiverid : (String) telegramUserID;
  doc["text"] = text;
  pushlib.sendMessageDoc(doc);

}



void publishLcd(char * line1, char * line2, uint32_t id=0) {
#if defined(VIRTUALKEYPAD)
  if (ws.count()) {
    char outas[128];
    StaticJsonDocument < 200 > doc;
    if (line1 != NULL)
      doc["lcd_upper"] = line1;
    if (line2 != NULL)
      doc["lcd_lower"] = line2;
    serializeJson(doc, outas);
    String out = encrypt(outas);
  if (id )
    ws.text(id, out.c_str());
  else
      ws.textAll(out.c_str());
  }
#endif

}

void publishMsg(const char * stateName,String msg, uint32_t id=0) {
#if defined(VIRTUALKEYPAD)    
  char outas[128];
  StaticJsonDocument < 200 > doc;
  doc[stateName] = msg.c_str();
  serializeJson(doc, outas);
  String out = encrypt(outas);
  if (id )
    ws.text(id, out.c_str());
  else
      ws.textAll(out.c_str());  
  #endif
}

void publishStatus(const char * stateName, uint8_t state, uint32_t id=0) {
#if defined(VIRTUALKEYPAD)
  if (ws.count()) {
    char outas[128];
    StaticJsonDocument < 200 > doc;
    doc[stateName] = state;
    serializeJson(doc, outas);
    String out = encrypt(outas);
    if (DEBUG > 1)
        Serial.printf("publishstat: %s\n",out.c_str());
  if (id )
    ws.text(id, out.c_str());
  else
      ws.textAll(out.c_str());

  }
#endif
}


void setup() {

  Serial.begin(115200);
  delay(1000);
  Serial.println();

 
 // pinMode(LED_BUILTIN, OUTPUT); // LED pin as output.

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSSID, wifiPassword);
  
#if defined(VIRTUALKEYPAD)
  aes.setPadMode(paddingMode::CMS);
  aes.set_key((byte * ) aeskey, 128);
#endif

  uint8_t checkCount = 20;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf("Connecting to Wifi..%d\n", checkCount);
    delay(1000);
    if (checkCount--) continue;
    checkCount = 50;
    WiFi.reconnect();

  }
  //WiFi.setAutoReconnect(true);
  //WiFi.persistent(true);
   SPIFFS.begin(); 
#if defined(VIRTUALKEYPAD)
  if (!MDNS.begin(clientName)) {
    Serial.println(F("Error setting up MDNS responder."));
    while (1) {
      delay(1000);
    }
  }
  File root = SPIFFS.open("/");

  File file = root.openNextFile();
  Serial.println(F("Opening SPIFFS"));
  
  while (file) {
    Serial.print(F("FILE: "));
    Serial.println(file.name());
    file = root.openNextFile();

  }
  ws.onEvent(onWsEvent);
  server.addHandler( & ws);
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  server.begin();
  MDNS.addService("http", "tcp", 80);
  Serial.print(F("Web server started: http://"));
  Serial.print(clientName);
  Serial.println(F(".local"));
#endif
  
  readConfig();
  
  #ifdef useOTA
  // Port defaults to 8266
  ArduinoOTA.setPort(3232); //port 3232 needed for spiffs OTA upload

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(clientName);

  // No authentication by default
  ArduinoOTA.setPassword(otaAccessCode);

  ArduinoOTA.onStart([]() {
    pushlib.stop();
    dsc.stop();
    Serial.println(F("Start"));
  });
  ArduinoOTA.onEnd([]() {
    pushlib.begin();
    Serial.println(F("\nEnd"));
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    delay(1);
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println(F("Auth Failed"));
    else if (error == OTA_BEGIN_ERROR) Serial.println(F("Begin Failed"));
    else if (error == OTA_CONNECT_ERROR) Serial.println(F("Connect Failed"));
    else if (error == OTA_RECEIVE_ERROR) Serial.println(F("Receive Failed"));
    else if (error == OTA_END_ERROR) Serial.println(F("End Failed"));
    DSCkeybus->begin();
    pushlib.begin();    
  });

  ArduinoOTA.begin();
  #endif

  Serial.println(F("Ready"));
  Serial.print(F("IP address: "));
  Serial.println(WiFi.localIP());

   DSCkeybus = new DSCkeybushome(dscClockPin,dscReadPin,dscWritePin);
    DSCkeybus->accessCode=accessCode;
    DSCkeybus->maxZones=maxZones;
    DSCkeybus->defaultPartition=defaultPartition;
    DSCkeybus->debug=2; // 0 = off, 1 = minimal, 2 = all packets shown on console  3 =  + serial port debugging
    DSCkeybus->expanderAddr1=expanderAddr1; //zone expander
    DSCkeybus->expanderAddr2=expanderAddr2;
    DSCkeybus->userCodes=userCodes;
    
    DSCkeybus->onSystemStatusChange([&](std::string statusCode) {
        if (!DSCkeybus->forceRefresh && !pauseNotifications && statusCode!="") {
         char msg[40];
         snprintf(msg, 40, "System status: %s",statusCode.c_str());            
         pushNotification(msg); 
        }         
    });
    
    DSCkeybus->onPartitionStatusChange([&](std::string statusCode, int partition) {
        if (!DSCkeybus->forceRefresh && !pauseNotifications && statusCode!="" && (notificationFlag & 2))  {      
         char msg[40];
         snprintf(msg, 40, "Partition:%d status: %s",partition,statusCode.c_str());            
         pushNotification(msg);   
        }
    });
    
    DSCkeybus->onPartitionMsgChange([&](std::string pmsg,uint8_t partition) {
        if (!DSCkeybus->forceRefresh && pmsg!="" && !pauseNotifications && (notificationFlag & 8))   {     
         char msg[100];
         snprintf(msg, 100, "Partition:%d msg: %s",partition,pmsg.c_str());            
         pushNotification(msg);   
        }
    });      

    DSCkeybus->onPanelStatusChange([&](panelStatus ps,bool open,int partition) {

        if (partition==activePartition || !partition)   { 
          
           char msg[30]="";        
           switch (ps) {
           case trStatus:publishStatus("trouble_status",open);snprintf(msg, 30, "Partition:%d Trouble status is %s",partition,open?"ON":"OFF");break ;       
           case batStatus: publishStatus("battery_status",open);snprintf(msg, 30, "Partition:%d Battery status is %s",partition,open?"ON":"OFF");break;
           case acStatus: publishStatus("power_status",open);snprintf(msg, 30, "Partition:%d AC status is %s",partition,open?"ON":"OFF");break;
           case panicStatus: publishStatus("panic_status",open);snprintf(msg, 30, "Partition:%d Panic status is %s",partition,open?"ON":"OFF");break;
           case rdyStatus: publishStatus("ready_status",open);snprintf(msg, 30, "Partition:%d Ready status is %s",partition,open?"ON":"OFF");break;
           case armStatus: publishStatus("armed_status",open);snprintf(msg, 30, "Partition:%d Armed status is %s",partition,open?"ON":"OFF");break;
           case chimeStatus: publishStatus("chime_status",open);snprintf(msg, 30, "Partition:%d Chime status is %s",partition,open?"ON":"OFF");break; 
           default: break;
            }
          
           if (!DSCkeybus->forceRefresh && !pauseNotifications && (notificationFlag & 8))
            pushNotification(msg);
        }
    });
    DSCkeybus->onZoneMsgStatus([&](std::string msg) {
           publishMsg("zone_info",msg.c_str());
    });
    DSCkeybus->onLine1Display([&](std::string msg,int partition) {
        if (partition==activePartition)
            publishLcd((char * )msg.c_str(),NULL);
    });
    DSCkeybus->onLine2Display([&](std::string msg,int partition) {
        if (partition==activePartition && msg!="")        
           publishLcd(NULL,(char * )msg.c_str());

    });
    DSCkeybus->onEventInfo([&](std::string msg) {
        
      if (!DSCkeybus->forceRefresh && msg !="" && !pauseNotifications && (notificationFlag & 4) )     
        pushNotification(msg.c_str());
            publishMsg("event_info",msg.c_str());
    });  

    DSCkeybus->onBeeps([&](std::string beep,int partition) {

    });    
    DSCkeybus->onFireStatusChange([&](bool open, int partition) {
           if (partition==activePartition) {
            publishStatus("fire_status",open);
           }              
            if (!DSCkeybus->forceRefresh && !pauseNotifications && (notificationFlag & 2)) {
                char msg[100];
                snprintf(msg, 100, "Partition:%d Fire status is now %s",partition,open?"ON":"OFF"); 
                pushNotification(msg);
 
           }        
    }); 
    
    DSCkeybus->onTroubleMsgStatus([&](std::string tmsg) {
        if (!DSCkeybus->forceRefresh && tmsg!="" && !pauseNotifications && (notificationFlag & 8)) {
            char msg[100];
            snprintf(msg, 100, "Trouble: %s",tmsg.c_str()); 
            pushNotification(msg);
        }
    });
    
    DSCkeybus->onZoneStatusChange([&](uint8_t zone, bool open) {
        if (inListZone(zone) && !DSCkeybus->forceRefresh && !pauseNotifications && (notificationFlag & 1)) {
            char msg[100];
            snprintf(msg, 100, "Zone %d is now %s ", zone,open?"OPEN":"CLOSED"); 
            pushNotification(msg);
        }

    });
    DSCkeybus->onRelayChannelChange([&](uint8_t channel, bool state) {
        
    });
   
     DSCkeybus->begin(); 

 
  pauseNotifications=false;
  #ifdef TELEGRAM_PUSH
  pushlib.addCmdHandler( & cmdHandler);
  #endif
  pushlib.begin();
  pushNotification("System restarted");

}

void loop() {

  static unsigned long previousWifiTime;
  if (WiFi.status() != WL_CONNECTED && millis() - previousWifiTime >= 20000) {
    Serial.println(F("Reconnecting to WIFI network"));
    WiFi.disconnect();
    WiFi.reconnect();
    previousWifiTime = millis();

  }
  
#if defined(VIRTUALKEYPAD)
  if (millis() - pingTime > 300000 && ws.count() > 0) {
    ws.pingAll();
    pingTime = millis();
  }
#endif

  DSCkeybus->loop();  
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
  pushlib.loop();

  #ifdef useOTA
  if (!pushlib.isSending()) //they conflict (when using ASYNC tasks) so we only check for ota when not pushing out a message
    ArduinoOTA.handle();
  #endif

}
#if defined(VIRTUALKEYPAD)
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t * data, size_t len) {

  if (type == WS_EVT_CONNECT) {
    client -> printf("{\"connected_id\": %u}", client -> id());
    Serial.printf("ws[%s][%u] connected\n", server -> url(), client -> id());

    if (dsc.keybusConnected && ws.count()) {
      publishLcd((char * )
        "DSC bus", (char * )
        "connected", client -> id());
      DSCkeybus->forceRefresh=true;
    } else
    if (!dsc.keybusConnected && ws.count()) {
      publishLcd((char * )
        "DSC bus", (char * )
        "disconnected");
    }
    //client->ping();
    pingTime = millis();
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("ws[%s][%u] disconnect\n", server -> url(), client -> id());

  } else if (type == WS_EVT_ERROR) {
    Serial.printf("ws[%s][%u] error(%u): %s\n", server -> url(), client -> id(), *((uint16_t * ) arg), (char * ) data);
  } else if (type == WS_EVT_PONG) {
    Serial.printf("ws[%s][%u] pong[%u]: %s\n", server -> url(), client -> id(), len, (len) ? (char * ) data : "");
  } else if (type == WS_EVT_DATA) {
    AwsFrameInfo * info = (AwsFrameInfo * ) arg;
    String msg = "";
    if (info -> final && info -> index == 0 && info -> len == len) {
      //the whole message is in a single frame and we got all of it's data
      Serial.printf("ws[%s][%u] %s-message[%llu]: ", server -> url(), client -> id(), (info -> opcode == WS_TEXT) ? "text" : "binary", info -> len);
      if (info -> opcode == WS_TEXT) {
        for (size_t i = 0; i < info -> len; i++) {
          msg += (char) data[i];
        }
      }
      Serial.printf("%s\n", msg.c_str());
      if (info -> opcode == WS_TEXT) {
        msg = decrypt(msg);
        StaticJsonDocument < 200 > doc;
        auto err = deserializeJson(doc, msg);
        if (!err) {
          if (doc.containsKey("btn_single_click")) {
            char * tmp = (char * ) doc["btn_single_click"].as <
              const char * > ();
            char *
              const sep_at = strchr(tmp, '_');
            if (sep_at != NULL) {
              * sep_at = '\0';

              char * v = sep_at + 1;
              if (dsc.keybusConnected) {

                if (strcmp(v, "s") == 0) {
                  DSCkeybus->alarm_keypress_partition("S",activePartition);
                } else if (strcmp(v, "w") == 0) {
                DSCkeybus->alarm_keypress_partition("W",activePartition);
              } else if (strcmp(v, "c") == 0) {
                DSCkeybus->alarm_keypress_partition("*4",activePartition);
              } else if (strcmp(v, "x") == 0 && !dsc.armed[activePartition-1]) {
                DSCkeybus->alarm_keypress_partition("#",activePartition);
              } else if (strcmp(v, "p1") == 0) {
                  setActivePartition(1);
              } else if (strcmp(v, "p2") == 0) {
                  setActivePartition(2);             
              } else if (strcmp(v, "p3") == 0) {  
                  setActivePartition(3);
              }  else DSCkeybus->alarm_keypress_partition(v,activePartition);
              }
              Serial.printf("got key %s\n", v);
            }
          }
        }
      }
    } else {
      //message is comprised of multiple frames or the frame is split into multiple packets
      if (info -> index == 0) {
        if (info -> num == 0) {
          Serial.printf("ws[%s][%u] %s-message start\n", server -> url(), client -> id(), (info -> message_opcode == WS_TEXT) ? "text" : "binary");
        }
        Serial.printf("ws[%s][%u] frame[%u] start[%llu]\n", server -> url(), client -> id(), info -> num, info -> len);
      }

      Serial.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server -> url(), client -> id(), info -> num, (info -> message_opcode == WS_TEXT) ? "text" : "binary", info -> index, info -> index + len);

      if (info -> opcode == WS_TEXT) {
        for (size_t i = 0; i < info -> len; i++) {
          msg += (char) data[i];
        }
      } else {
        char buff[3];
        for (size_t i = 0; i < info -> len; i++) {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          msg += buff;
        }
      }

      Serial.printf("%s\n", msg.c_str());

      if ((info -> index + len) == info -> len) {
        Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", server -> url(), client -> id(), info -> num, info -> len);
        if (info -> final) {
          Serial.printf("ws[%s][%u] %s-message end\n", server -> url(), client -> id(), (info -> message_opcode == WS_TEXT) ? "text" : "binary");
          if (info -> message_opcode == WS_TEXT) client -> text("I got your text message");
          else client -> binary("I got your binary message");
        }
      }
    }
  }
}

String encrypt(String msg) {
  char b64data[200];
  byte cipher[200];
  char ivdata[40];
  gen_iv(ivaes);
  base64_encode(ivdata, (char * ) ivaes, 16);

  aes.do_aes_encrypt((byte * ) msg.c_str(), msg.length(), cipher, (byte * ) aeskey, 128, ivaes);
  base64_encode(b64data, (char * ) cipher, aes.get_size());
  char outmsg[200];
  sprintf(outmsg, "{\"iv\":\"%s\",\"data\":\"%s\"}", ivdata, b64data);
  return (String) outmsg;
}

String decrypt(String wsmsg) {
  StaticJsonDocument < 300 > doc;
  auto err = deserializeJson(doc, wsmsg);
  if (!err) {
    std::string eiv, emsg;
    if (doc.containsKey("iv")) {
      eiv = doc["iv"].as < std::string > ();
    }
    if (doc.containsKey("data")) {
      emsg = doc["data"].as < std::string > ();
    }
    char data_decoded[200];
    char iv_decoded[40];
    char out[200];
    int encrypted_length = base64_decode(data_decoded, (char * ) emsg.c_str(), emsg.length());
    base64_decode(iv_decoded, (char * ) eiv.c_str(), eiv.length());
    aes.do_aes_decrypt((byte * ) data_decoded, encrypted_length, (byte * ) out, (byte * ) aeskey, 128, (byte * ) iv_decoded);
    int len = aes.get_size() - out[aes.get_size() - 1]; //remove padding
    out[len] = '\0';
    return out;
  } else return "";
}

uint8_t getrnd() {
  uint8_t rand = (uint8_t) random(0, 0xff);
  return rand;
}

// Generate a random initialization vector
void gen_iv(byte * iv) {
  for (int i = 0; i < N_BLOCK; i++) {
    iv[i] = (byte) getrnd();
  }
}
#endif

void setActivePartition(uint8_t partition) {

     if (partition < 1 || partition > maxPartitions) return;
     char msg[30];
     if (dsc.disabled[partition-1]) {
             sprintf(msg, "Partition %d is disabled\n",partition);
             publishMsg("event_info",msg);
             Serial.printf("%s\n",msg);
             return;
     }
         
     activePartition=partition;
     DSCkeybus->defaultPartition=partition;  
     DSCkeybus->forceRefresh=true;     
    sprintf(msg, "Partition: %d",partition);
    publishMsg("event_info",msg);
    Serial.printf("%s\n",msg);
    
}

void readConfig(){

   Serial.println(F("Reading config)"));

   File file = SPIFFS.open("/configFile","r");
   if(!file || file.isDirectory()){
       Serial.println(F("Failed to open file for reading"));
       return ;
   }
   StaticJsonDocument <300>doc;
   DeserializationError error = deserializeJson(doc, file);

    if (error)
    {
        Serial.println(F("Failed to parse cred file"));
        if (file) file.close();
        return ;
    }


   if (doc.containsKey("ids")) {
    JsonArray ids=doc["ids"];
    telegramAllowedIDs.clear(); 
    for (String id :ids) {
      telegramAllowedIDs.push_back(id);
      Serial.printf("IDS %s \n",id.c_str());
    }        

   }
   
   if (doc.containsKey("zones")) {
    JsonArray zones=doc["zones"];
    notifyZones.clear();
    for (int z : zones) {
      notifyZones.push_back(z);
      Serial.printf("Zone read is %d\n",z); 
    }    
    
   }
   if (doc.containsKey("notificationflag")) {
       notificationFlag=(uint8_t) doc["notificationflag"];
   }
   if (file) file.close();


}


void writeConfig(){
   Serial.println(F("Writing config"));

   File file = SPIFFS.open("/configFile", FILE_WRITE);

   if(!file){
      Serial.println(F("Failed to open file for writing"));
      return;
   }
   StaticJsonDocument <200>doc;
   StaticJsonDocument <100>zones;
   StaticJsonDocument <100>ids;

   for (int z : notifyZones) {
        Serial.printf("Notify zone: %d\n",z);
        zones.add(z);
   }

   for (String id : telegramAllowedIDs) {
       Serial.printf("Allowed ids: %s\n",id.c_str());
       ids.add(id);
   }

   doc["zones"]=zones;
   doc["ids"]=ids;
   doc["notificationflag"]=notificationFlag;
     String out;
     serializeJson(doc, out);
     Serial.printf("Serialized=%s\n",out.c_str());
     
    if(file.print(out)){
      Serial.println(F("File written"));
   }else {
      Serial.println(F("Write failed"));
   }
   if (file) file.close();

}



#ifdef TELEGRAM_PUSH
//used with telegram to handle incoming cmds
String getZoneStatus() {
  String s = F("<b>Zone statuses:</b> \n");
  s=s+(String) DSCkeybus->previousZoneStatusMsg.c_str()+"\n";
 
  return s;
}

String getPartitionStatus() {
  String s="";
   for (int p=1;p<=maxPartitions;p++) {
      s = s + F("<b>Partition ") + (String) p + F(" status:</b>\n");
      if (DSCkeybus->partitionStatus[p-1].disabled)
          s=s+F("Partition is disabled\n");
      else if (DSCkeybus->partitionStatus[p-1].armed) {
          if (DSCkeybus->partitionStatus[p-1].armedNight)
            s=s+F("Partition is armed Night\n");          
          else if (DSCkeybus->partitionStatus[p-1].armedAway)
            s=s+F("Partition is armed Away\n");
          else if (DSCkeybus->partitionStatus[p-1].armedStay)
            s=s+F("Partition is armed Stay\n");
          else
            s=s+F("Partition is armed\n");
      } else if (DSCkeybus->partitionStatus[p-1].exitdelay)
             s=s+F("Partition is in exit delay\n");
       else if (DSCkeybus->partitionStatus[p-1].ready)
           s=s+F("Partition is ready\n");
      else 
          s=s+F("Partition is not ready\n");
       if (DSCkeybus->partitionStatus[p-1].chime && !DSCkeybus->partitionStatus[p-1].disabled)
            s=s+F("Chime is ") + String(DSCkeybus->partitionStatus[p-1].chime?"ON\n":"OFF\n");
      s=s+"\n";
   }
  
  return s;
  
}

String getPanelStatus() {
  String s="";
    s = s + F("<b>Panel Lights</b>\n");
  if (dsc.powerTrouble)
      s=s+F("AC is off |");
  
  if (!dsc.batteryTrouble) //if 1 then battery is ok
      s=s+F("Battery is low\n");
 
  if (dsc.trouble)
      s=s+F("Trouble is on\n");

  s = s + "\n"; 

  return s;
}

void sendCurrentConfig(JsonDocument & doc) {
      String config="Zones List\n";
      for (int z : notifyZones) {
        config=config +  String(z) + "\n";
      }
      config = config + "\nTelegram IDs\n";
      for (String id : telegramAllowedIDs) {
        config=config +  id + "\n";
      }
      config = config + "\nNotification Flag=" + (String) notificationFlag + "\n";
      doc["text"] = config;
      pushlib.sendMessageDoc(doc);
}

//telegram callback to handle bot commands
void cmdHandler(rx_message_t * msg) {

  if (!inListTelegramID(msg -> chat_id) && strcmp(msg -> chat_id.c_str(),telegramUserID) != 0) {
    Serial.printf("Chat ID %s not allowed to send cmds", msg -> chat_id.c_str());
    return;
  }
  static bool firstRun=true;
  StaticJsonDocument < 1000 > doc;
  #ifdef DEFAULT_PUSH_OPTIONS
  deserializeJson(doc, DEFAULT_PUSH_OPTIONS);
  #endif
  doc["chat_id"] = msg -> chat_id;
  String sub = msg -> text.substring(0, 2);

  if (msg -> text == "/armstay") {
    if (!dsc.armed[activePartition-1]) {      
        doc["text"] = F("sending armed stay");
        pushlib.sendMessageDoc(doc);
        DSCkeybus->alarm_keypress_partition("s",activePartition);
    }

  } else if (msg -> text == "/armaway") {
    if (!dsc.armed[activePartition-1]) {        
        doc["text"] = F("sending armed away");
        pushlib.sendMessageDoc(doc);
        DSCkeybus->alarm_keypress_partition("w",activePartition);
    }

  } else if (msg -> text == "/bypass") {
    if (!dsc.armed[activePartition-1]) {        
        doc["text"] = F("Sending bypass...");
        pushlib.sendMessageDoc(doc);
        DSCkeybus->alarm_keypress_partition("*199#",activePartition);
    }
  } else if (msg -> text == "/reboot" && !firstRun) {
    doc["text"] = F("Rebooting...");
    pushlib.sendMessageDoc(doc);
    delay(5000);
    ESP.restart();
  } else if (msg -> text == "/getstatus") {
    String s = "\n" + getPanelStatus();
    s += String(F("------------------------------\n"));
    s += getPartitionStatus();
    s += String(F("------------------------------\n"));
    s += getZoneStatus();
    s += String(F("------------------------------\n"));    
    if (pauseNotifications)
      s += String(F("Notifications are DISABLED\n"));
    else
      s += String(F("Notifications are ACTIVE\n"));
     s += String(F("Notification flag is ")) + (String) notificationFlag + "\n";
    s += "Active partition is " + (String) activePartition + " \n";
    doc["parse_mode"] = "HTML";
    doc["text"] = s;
    doc.remove("reply_markup"); //msg too long for markup        
    pushlib.sendMessageDoc(doc);

  } else if (sub == "/!") {
      
    String cmd = msg -> text.substring(2, msg -> text.length());
    std::string s(cmd.c_str());
    Serial.printf("cmd = %s\n", cmd.c_str());
    DSCkeybus->alarm_keypress_partition(s,activePartition);    

  }  else if (sub == "/&") {
     int p;
    String pstr = msg -> text.substring(2,3);     
    sscanf(pstr.c_str(), "%d", &p);     
    String cmd = msg -> text.substring(3, msg -> text.length());
    std::string s(cmd.c_str());    
    Serial.printf("cmd = %s,partition=%d,pstr=%s\n", cmd.c_str(),p,pstr.c_str());
    DSCkeybus->alarm_keypress_partition(s,p);

  }  else if (msg -> text == "/stopbus") {
    dsc.stop();
    doc["text"] = F("Vista bus stopped...");
    pushlib.sendMessageDoc(doc);

  } else if (msg -> text == "/startbus") {
    dsc.stop();
    DSCkeybus->begin();
    doc["text"] = F("DSC bus started..");
    pushlib.sendMessageDoc(doc);

  } else if (msg -> text == "/stopnotify") {
    pauseNotifications = true;
    doc["text"] = F("Notifications paused..");
    pushlib.sendMessageDoc(doc);


  } else if (msg -> text == "/startnotify") {
    pauseNotifications = false;
    doc["text"] = F("Notifications un-paused..");
    pushlib.sendMessageDoc(doc);

  } else if (msg -> text == "/getstats") {
    char buf[100];
    snprintf(buf, 100, "\n<b>Memory Useage</b>\nFreeheap=%d\nMinFreeHeap=%d\nHeapSize=%d\nMaxAllocHeap=%d\n", ESP.getFreeHeap(), ESP.getMinFreeHeap(), ESP.getHeapSize(), ESP.getMaxAllocHeap());
    doc["parse_mode"] = "HTML";
    doc["text"] = String(buf);
    pushlib.sendMessageDoc(doc);
    
  } else if (msg -> text.startsWith("/setpartition")) {
    String pstr = msg -> text.substring(msg -> text.indexOf('=') + 1, msg -> text.length());
    int p;
    sscanf(pstr.c_str(), "%d", & p);
    if (p > 0 && p < 4) {
      setActivePartition(p);
      char out[40];
      sprintf(out, "Partition is now set to %d\n", p);
      doc["text"] = String(out);
      pushlib.sendMessageDoc(doc);
    }
   } else if (msg -> text.startsWith("/setnotifyflag")) {
    String pstr = msg -> text.substring(msg -> text.indexOf('=') + 1, msg -> text.length());
    int p;
    sscanf(pstr.c_str(), "%d", & p);
    if (p >= 0 && p <256) {
      notificationFlag=p;
      writeConfig();      
      char out[40];
      sprintf(out, "Nofitification flag set to %d\n", p);
      doc["text"] = String(out);
      pushlib.sendMessageDoc(doc);
    }
      
  } else if (msg -> text.startsWith("/addzones")) {
    String pstr = msg -> text.substring(msg -> text.indexOf('=') + 1, msg -> text.length());

    char * token = strtok((char *)pstr.c_str(), ",");
   // loop through the string to extract all other tokens
   while( token != NULL ) {
      int z;
      sscanf(token, "%d", & z);       
     if (z > 0 && z < maxZones) {
      if (!inListZone(z)) {
      notifyZones.push_back(z);
      writeConfig();
      char out[40];
      sprintf(out, "Added zone %d to notify list\n", z);
      doc["text"] = String(out);
      pushlib.sendMessageDoc(doc);
      }
    }
      token = strtok(NULL, ",");
   }      
     sendCurrentConfig(doc);

  } else if (msg -> text.startsWith("/removezones")) {
    String pstr = msg -> text.substring(msg -> text.indexOf('=') + 1, msg -> text.length());
       char * token = strtok((char *)pstr.c_str(), ",");
   // loop through the string to extract all other tokens
   while( token != NULL ) {
       int z;
      sscanf(token, "%d", & z);       
     if (z > 0 && z < maxZones) {
      if (inListZone(z)) {
      notifyZones.remove(z);
      writeConfig();
      char out[40];
      sprintf(out, "Removed zone %d from notify list\n", z);
      doc["text"] = String(out);
      pushlib.sendMessageDoc(doc);
      }
    }
      token = strtok(NULL, ",");
   }  
   sendCurrentConfig(doc);
       
  } else if (msg -> text.startsWith("/removeids")) {
    String pstr = msg -> text.substring(msg -> text.indexOf('=') + 1, msg -> text.length());
    char * token = strtok((char *)pstr.c_str(), ",");
   // loop through the string to extract all other tokens
   while( token != NULL ) {
    if (strcmp(token,"")!=0) {
      if (inListTelegramID(String(token))) {
      telegramAllowedIDs.remove(String(token));
      writeConfig();
      char out[40];
      sprintf(out, "Removed id %s from ID list\n", token);
      doc["text"] = String(out);
      pushlib.sendMessageDoc(doc);
      }
    }
     token = strtok(NULL, ",");
   }
   sendCurrentConfig(doc);
    
  } else if (msg -> text.startsWith("/addids")) {
    String pstr = msg -> text.substring(msg -> text.indexOf('=') + 1, msg -> text.length());
   char * token = strtok((char *)pstr.c_str(), ",");
   // loop through the string to extract all other tokens
   while( token != NULL ) {
    if (strcmp(token,"")!=0) {
      if (!inListTelegramID(String(token))) {
      telegramAllowedIDs.push_back(String(token));
      writeConfig();
      char out[40];
      sprintf(out, "Added id %s to ID list\n", token);
      doc["text"] = String(out);
      pushlib.sendMessageDoc(doc);
      }
    }
     token = strtok(NULL, ",");
    }      
   sendCurrentConfig(doc);
    
  } else if (msg -> text.startsWith("/getcfg")) {
      sendCurrentConfig(doc);
      
  } else if (msg -> text.startsWith("/getip")) {
      char out[50];
      sprintf(out, "Local IP address http://%s\n", WiFi.localIP().toString().c_str());
      doc["text"] = String(out);
      pushlib.sendMessageDoc(doc);

  } else if (msg -> text == "/help") {
     
    String menu="";
    int x=1;
    for (auto s : telegramMenu) {
        menu=menu + String(x) + ". " + String(FPSTR(s)) + "\n";
        x++;
    }
    doc["text"]=menu;
    doc.remove("reply_markup"); //msg too long for markup
    pushlib.sendMessageDoc(doc);

  }
    firstRun=false;
    doc.clear();
}

#endif