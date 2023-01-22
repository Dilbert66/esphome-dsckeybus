#include "telegram_async.h"


PushLib::PushLib(const char * token, const char * userID, const char * prefix) {
    telegramBotToken=token;
    telegramUserID=userID;
    messagePrefix=prefix;

}

PushLib::~PushLib() {

}

void PushLib::addCmdHandler(std:: function < void(rx_message_t * msg) > callback) {
  cmdHandlerCallback = callback;
}

void PushLib::stop() {
  resetTask = true;
}

bool PushLib::isSending() {
  return sending;
}

msgType PushLib::peekNextMsg() {
  msgType t;
  t.msg_text="";
  if (inMsgIdx == outMsgIdx) return t;
  return msgQueue[outMsgIdx];
}

msgType PushLib::getNextMsg() {
    msgType t;
    t.msg_text="";
  if (inMsgIdx == outMsgIdx) return t;
  uint8_t currentMsgIdx = outMsgIdx;
  outMsgIdx = (outMsgIdx + 1) % msgQueueSize;
  return msgQueue[currentMsgIdx];
}

void PushLib::saveMsgToQueue(String &  msg_text,String  method) {
  if (msgAvailable() + 1 == msgQueueSize) return; //overflow so can't save
  msgType msg;
  msg.msg_text = msg_text;
  msg.method=method;
  msgQueue[inMsgIdx] = msg;
  inMsgIdx = (inMsgIdx + 1) % msgQueueSize;
}

uint8_t PushLib::msgAvailable() {
  int avail = inMsgIdx - outMsgIdx;
  if (avail < 0)
    avail += msgQueueSize;
   return avail;
}

void PushLib::begin() {

  xTaskCreatePinnedToCore(
    this -> getUpdatesSendTask, //Function to implement the task
    "getUpdatesSendTask", //Name of the task
    TASK_STACK_SIZE, //Stack size in words
    (void * ) this, //Task input parameter
    10, //Priority of the task
    &xHandle, //Task handle.
    ASYNC_CORE //Core where the task should run
  );

}

void PushLib::loop() {

}

//stringfied json with all parameters
void PushLib::sendMessageJson(String & msg, String  method) {
  if (DEBUG_PUSHLIB > 0) printf("queueing msg %s\n", msg.c_str());
  saveMsgToQueue(msg,method);
}

//json document
void PushLib::sendMessageDoc(JsonDocument &  doc, String  method) {
  String msg;
  serializeJson(doc, msg);  
  if (DEBUG_PUSHLIB > 0) printf("queueing send message text %s\n", msg.c_str());  
  saveMsgToQueue(msg,method);
}

void PushLib::postMessage(WiFiClientSecure *ipClient, String *msg, String  *method) {


  if ( !ipClient -> connected()) 
    ipClient -> connect("api.telegram.org", 443);

    if (ipClient -> connected()) {
    ipClient->setHandshakeTimeout(30);         
    sending = true;
    if (DEBUG_PUSHLIB > 1) printf("Sending message: %s\n", msg->c_str());
    ipClient -> print(F("POST /bot"));
    ipClient -> print(telegramBotToken);
    ipClient -> print(*method);
    ipClient -> println(F(" HTTP/1.1"));
    ipClient -> println(F("Host: api.telegram.org"));
    ipClient -> println(F("Content-Type: application/json"));
    ipClient -> print(F("Content-Length: "));
    ipClient -> println(msg->length());
    ipClient -> println();
    ipClient -> print( *msg);

    // Wait for a response
    unsigned long previousMillis = millis();
    while (ipClient->connected() && !ipClient -> available() ) {
      if (millis() - previousMillis > 5000) {
        ipClient -> stop();
        sending = false;
        Serial.println(F("Timeout sending. Exited"));
        break;
      }
      delay(1);
    }
    // Reads the response until the first space - the next characters will be the HTTP status code
    previousMillis =millis();     
    while (ipClient->connected() && ipClient -> available() ) {
      if (ipClient -> read() == ' ') break;
      if (millis() - previousMillis > 5000) {ipClient->stop(); break; }         
      delay(1);
    }

    // Checks the first character of the HTTP status code - the message was sent successfully if the status code
    // begins with "2"
    char statusCode = ipClient -> read();

    // Successful, reads the remaining response to clear the client buffer
    if (statusCode == '2') {
      previousMillis =millis();        
      while (ipClient->connected() && ipClient -> available() ) {
        ipClient -> read();
        delay(1);
        if (millis() - previousMillis > 5000) {ipClient->stop(); break; }           
      }

      if (DEBUG_PUSHLIB > 0) Serial.println(F("Sending success"));
    }

    // Unsuccessful, prints the response to serial to help debug
    else {
      Serial.print(F("Push messaging error, response:"));
      Serial.print(statusCode);
      int idx = 0;
      previousMillis =millis();
      while (ipClient->connected() && ipClient -> available() && idx < 20) {
        Serial.printf("%c", ipClient -> read());
        idx++;
        delay(1);
        if (millis() - previousMillis > 5000) {ipClient->stop(); break; }        
      }
      Serial.printf("\n");
      
    }

    getNextMsg(); //ok done sending , pop the message off the queue, successful or not

  } else {
    Serial.println(F("Send task failed to connect to Telegram api..."));
    delay(10000);
  }
  sending = false;

}

void PushLib::getUpdatesSendTask(void * args) {

  PushLib * _this = (PushLib * ) args;
  WiFiClientSecure ipClient;
  
  if (_this -> telegram_root_ca && _this -> telegram_root_ca[0])
    ipClient.setCACert(_this -> telegram_root_ca);
  else
    ipClient.setInsecure();

  static unsigned long checkTime = millis();

  for (;;) {

    if (_this -> resetTask) {
      Serial.println(F("Got reset task"));
      break;
    }

    String payload = "";
    if (_this -> msgAvailable()) {
        msgType msg = _this -> peekNextMsg();        
      _this -> postMessage( & ipClient, & msg.msg_text,&msg.method);
      continue; //we make sure we clean out send queue first
    }
    if (millis() - checkTime > _this -> telegramCheckInterval ) {
      rx_message_t m;

      if (!ipClient.connected()) ipClient.connect("api.telegram.org", 443);

      if (ipClient.connected()) {
        ipClient.setHandshakeTimeout(30); 
        UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        if (DEBUG_PUSHLIB > 0) printf("\nTaskupdates free stack memory: %5d\n", (uint16_t) uxHighWaterMark);
        //if (DEBUG_PUSHLIB > 1) printf("FreeHeap: %6d, MaxBlock: %6d\n\n", heap_caps_get_free_size(0), heap_caps_get_largest_free_block(0));
        if (DEBUG_PUSHLIB > 1) Serial.println(F("connected to api"));

        String getRequest = "GET /bot" + (String)_this -> telegramBotToken + "/getUpdates?limit=1&allowed_updates=[%22message%22,%22callback_query%22,%22edit_channel_post%22]&timeout=" + String(LONG_POLL_TIMEOUT) + "&offset=" + String(_this -> lastMsgReceived) + " HTTP/1.1";
        // if (DEBUG_PUSHLIB > 1) Serial.printf("request: %s\n",getRequest.c_str());
        ipClient.println(getRequest);
        ipClient.println(F("Host: api.telegram.org"));
        ipClient.println(F("Accept: application/json"));
        ipClient.println(F("Cache-Control: no-cache"));
        ipClient.println();

        unsigned long previousMillis = millis();

        while (ipClient.connected() && !ipClient.available() && !_this -> resetTask) {
          if (_this -> msgAvailable()) { //check if pending outbound notifications
            //we have messages so exit poll
            ipClient.stop();
            break;
          }

          if (millis() - previousMillis > (LONG_POLL_TIMEOUT * 1000) + 5000) { //sanity exit if for some reason we never get a response back
            ipClient.stop();
            break;
          }
          delay(1);
        }

        previousMillis =millis();
        while (ipClient.connected() && !_this -> resetTask) {
          payload = ipClient.readStringUntil('\n');
          if (payload == "\r") {
            break;
          }
          if (millis() - previousMillis > 5000) {ipClient.stop(); break; }
          delay(1);
        }
        if (ipClient.connected() && !_this -> resetTask) {
          payload = ipClient.readStringUntil('\r');
           previousMillis=millis();
            while (ipClient.available() ) {
                ipClient.read();
                if (millis() - previousMillis > 5000) {ipClient.stop(); break; }                
            }           
        }
        if (payload != "") {
          if (DEBUG_PUSHLIB > 1) printf("payload=%s\n", payload.c_str());
          StaticJsonDocument < 2000 > root;
          auto err = deserializeJson(root, payload);
          if (!err) {
            if (root["result"][0]["callback_query"]["id"]) {
              int update_id = root["result"][0]["update_id"];
              update_id = update_id + 1;
              
              if (_this -> lastMsgReceived != update_id) {
                String sender = root["result"][0]["callback_query"]["message"]["from"]["username"];
                String text = root["result"][0]["callback_query"]["data"];
                String chat_id = root["result"][0]["callback_query"]["message"]["chat"]["id"];
                String date = root["result"][0]["callback_query"]["message"]["date"];
                String message_id= root["result"][0]["callback_query"]["message"]["message_id"];
                String id=root["result"][0]["callback_query"]["id"];
                
                m.sender = sender;
                m.message_id= message_id;
                m.text = text;
                m.is_callback=true;
                m.chat_id = chat_id;
                m.id=id;
                m.date = date;
                _this -> lastMsgReceived = update_id;
              }
            } else if (root["result"][0]["channel_post"]["id"]) { 
              int update_id = root["result"][0]["update_id"];
              update_id = update_id + 1;
     
              if (_this -> lastMsgReceived != update_id) {
                String sender = root["result"][0]["channel_post"]["message"]["from"]["username"];
                String text = root["result"][0]["channel_post"]["message"]["text"];
                String chat_id = root["result"][0]["channel_post"]["message"]["chat"]["id"];
                String date = root["result"][0]["channel_post"]["message"]["date"];
                String message_id= root["result"][0]["channel_post"]["message"]["message_id"];
                
                m.is_callback=false;
                m.sender = sender;
                m.text = text;
                m.chat_id = chat_id;
                m.message_id= message_id;                
                m.date = date;
                _this -> lastMsgReceived = update_id;
              }
            } else if (root["result"][0]["message"]["text"]) {
              int update_id = root["result"][0]["update_id"];
              update_id = update_id + 1;

              if (_this -> lastMsgReceived != update_id) {
                String sender = root["result"][0]["message"]["from"]["username"];
                String text = root["result"][0]["message"]["text"];
                String chat_id = root["result"][0]["message"]["chat"]["id"];
                String date = root["result"][0]["message"]["date"];
                String message_id= root["result"][0]["message"]["message_id"];
                
                m.is_callback=false;
                m.sender = sender;
                m.text = text;
                m.chat_id = chat_id;
                m.message_id= message_id;                
                m.date = date;
                _this -> lastMsgReceived = update_id;
              }
            }
          } else {
            m.chat_id = "0";
            Serial.println(F("\nMessage too long, skipped."));
            int update_id_first_digit = 0;
            int update_id_last_digit = 0;
            for (int a = 0; a < 3; a++) {
              update_id_first_digit = payload.indexOf(':', update_id_first_digit + 1);
            }
            for (int a = 0; a < 2; a++) {
              update_id_last_digit = payload.indexOf(',', update_id_last_digit + 1);
            }
            _this -> lastMsgReceived = payload.substring(update_id_first_digit + 1, update_id_last_digit).toInt() + 1;

          }
        }

      } else {
        Serial.println(F("Get updates failed to connect to telegram api..."));
        delay(10000);

      }

 
      if ( m.chat_id != "" && m.chat_id !="0"  && m.chat_id != "null") {
        if (DEBUG_PUSHLIB > 0) printf("message received %s from %s\n", m.text.c_str(), m.chat_id.c_str());
        _this->handleCommands(m);    
      }   
    }
    delay(1);
  }

  _this -> resetTask = false;
  vTaskDelete(NULL);

}

void PushLib::handleCommands(rx_message_t  m) {
  if (cmdHandlerCallback != NULL)
    cmdHandlerCallback( &m);
}

