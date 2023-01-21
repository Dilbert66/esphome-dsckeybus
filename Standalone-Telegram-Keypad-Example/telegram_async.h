/*
 *    1. If you don't have one, register an account on https://telegram.org and download the app for your platform
 *    2. Create a new bot through @BotFather with cmd /newbot
 *    3. Copy the bot token to the sketch in telegramBotToken.
 *    5. Send a message to the newly created bot to start a conversation.
 *    6. Send the cmd /getid to @myidbot: https://telegram.me/myidbot
 *    7. Copy the user ID to the sketch in telegramUserID.
 *    8. I recommend you also prevent your bot from being added to groups by sending the command /setjoingroups to @Botfather.  Make sure you first add the bot to any groups you want first before disabling the feature.
        
 */

#ifndef __TELEGRAMNOTIFY_H

#define __TELEGRAMNOTIFY_H
#include "Arduino.h"
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#define ASYNC_CORE 0 // which core to run async tasks on
#define TASK_STACK_SIZE 9800 // how much stack task is assigned in words 
#define LONG_POLL_TIMEOUT 120 //how many seconds to keep the connection open for a long poll get updates session
#define TELEGRAM_PUSH
#define DEBUG_PUSHLIB 2


struct rx_message_t {
  String text;
  String chat_id;
  String message_id;
  String id;
  String sender;
  bool is_callback;
  String date;
};

struct msgType {
    String msg_text;
    String method;

};

class PushLib {
public:

PushLib(const char * token, const char * userID, const char * prefix);
~PushLib();

void loop();
void begin();
void stop();
void sendMessageJson(String & msg, String method = "/sendMessage");
void sendMessageDoc(JsonDocument &  doc,  String method = "/sendMessage");
bool isSending();
void addCmdHandler(std:: function < void(rx_message_t * msg) > callback);

private:

//Go Daddy Secure Certificate Authority - G2
  //5/3/2011 to 5/3/2031
const char * telegram_root_ca = R"=EOF=(
-----BEGIN CERTIFICATE-----
MIIE0DCCA7igAwIBAgIBBzANBgkqhkiG9w0BAQsFADCBgzELMAkGA1UEBhMCVVMx
EDAOBgNVBAgTB0FyaXpvbmExEzARBgNVBAcTClNjb3R0c2RhbGUxGjAYBgNVBAoT
EUdvRGFkZHkuY29tLCBJbmMuMTEwLwYDVQQDEyhHbyBEYWRkeSBSb290IENlcnRp
ZmljYXRlIEF1dGhvcml0eSAtIEcyMB4XDTExMDUwMzA3MDAwMFoXDTMxMDUwMzA3
MDAwMFowgbQxCzAJBgNVBAYTAlVTMRAwDgYDVQQIEwdBcml6b25hMRMwEQYDVQQH
EwpTY290dHNkYWxlMRowGAYDVQQKExFHb0RhZGR5LmNvbSwgSW5jLjEtMCsGA1UE
CxMkaHR0cDovL2NlcnRzLmdvZGFkZHkuY29tL3JlcG9zaXRvcnkvMTMwMQYDVQQD
EypHbyBEYWRkeSBTZWN1cmUgQ2VydGlmaWNhdGUgQXV0aG9yaXR5IC0gRzIwggEi
MA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC54MsQ1K92vdSTYuswZLiBCGzD
BNliF44v/z5lz4/OYuY8UhzaFkVLVat4a2ODYpDOD2lsmcgaFItMzEUz6ojcnqOv
K/6AYZ15V8TPLvQ/MDxdR/yaFrzDN5ZBUY4RS1T4KL7QjL7wMDge87Am+GZHY23e
cSZHjzhHU9FGHbTj3ADqRay9vHHZqm8A29vNMDp5T19MR/gd71vCxJ1gO7GyQ5HY
pDNO6rPWJ0+tJYqlxvTV0KaudAVkV4i1RFXULSo6Pvi4vekyCgKUZMQWOlDxSq7n
eTOvDCAHf+jfBDnCaQJsY1L6d8EbyHSHyLmTGFBUNUtpTrw700kuH9zB0lL7AgMB
AAGjggEaMIIBFjAPBgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBBjAdBgNV
HQ4EFgQUQMK9J47MNIMwojPX+2yz8LQsgM4wHwYDVR0jBBgwFoAUOpqFBxBnKLbv
9r0FQW4gwZTaD94wNAYIKwYBBQUHAQEEKDAmMCQGCCsGAQUFBzABhhhodHRwOi8v
b2NzcC5nb2RhZGR5LmNvbS8wNQYDVR0fBC4wLDAqoCigJoYkaHR0cDovL2NybC5n
b2RhZGR5LmNvbS9nZHJvb3QtZzIuY3JsMEYGA1UdIAQ/MD0wOwYEVR0gADAzMDEG
CCsGAQUFBwIBFiVodHRwczovL2NlcnRzLmdvZGFkZHkuY29tL3JlcG9zaXRvcnkv
MA0GCSqGSIb3DQEBCwUAA4IBAQAIfmyTEMg4uJapkEv/oV9PBO9sPpyIBslQj6Zz
91cxG7685C/b+LrTW+C05+Z5Yg4MotdqY3MxtfWoSKQ7CC2iXZDXtHwlTxFWMMS2
RJ17LJ3lXubvDGGqv+QqG+6EnriDfcFDzkSnE3ANkR/0yBOtg2DZ2HKocyQetawi
DsoXiWJYRBuriSUBAA/NxBti21G00w9RKpv0vHP8ds42pM3Z2Czqrpv1KrKQ0U11
GIo/ikGQI31bS/6kA1ibRrLDYGCD+H1QQc7CoZDDu+8CL9IVVO5EFdkKrqeKM+2x
LXY2JtwE65/3YR8V3Idv7kaWKK2hJn0KCacuBKONvPi8BDAB
-----END CERTIFICATE-----
)=EOF=";

TaskHandle_t xHandle;

const char * telegramBotToken; // Set the Telegram bot access token
const char * telegramUserID; // Set the default Telegram chat recipient user/group ID
const char * messagePrefix; // Set a prefix for all messages

WiFiClientSecure * ipClient;
bool resetTask;
bool sending = false;
uint8_t inMsgIdx, outMsgIdx;
const uint8_t msgQueueSize = 10;
msgType msgQueue[10];
msgType getNextMsg();
msgType peekNextMsg();
void saveMsgToQueue(String & msg_text,String  method);
uint8_t msgAvailable();
void handleCommands(rx_message_t  m);
const int telegramCheckInterval = 2000;
rx_message_t getmessage;
int lastMsgReceived = 0;
static void getUpdatesSendTask(void * args);
void postMessage(WiFiClientSecure * ipClient, String * msg,String *method);
std:: function < void(rx_message_t * msg) > cmdHandlerCallback;
};

#endif