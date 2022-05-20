//for project documenation visit https://github.com/Dilbert66/esphome-dsckeybus

#ifndef dscalarm_h
#define dscalarm_h

#include "esphome.h"

#include "dscKeybusInterface.h"

using namespace esphome;

#ifdef ESP32

#define dscClockPinDefault 22 // esp32: GPIO22
#define dscReadPinDefault 21 // esp32: GPIO21
#define dscWritePinDefault 18 // esp32: GPIO18

#else

#define dscClockPinDefault 5 // esp8266: GPIO5 
#define dscReadPinDefault 4 // esp8266: GPIO4 
#define dscWritePinDefault 15 // esp8266: GPIO15 

#endif

#define maxZonesDefault 32 //set to 64 if your system supports it

dscKeybusInterface dsc(dscClockPinDefault, dscReadPinDefault, dscWritePinDefault);
bool forceDisconnect;

void disconnectKeybus() {
  dsc.stop();
  dsc.keybusConnected = false;
  dsc.statusChanged = false;
  forceDisconnect = true;

}
enum panelStatus {
  acStatus,
  batStatus,
  trStatus,
  fireStatus,
  panicStatus,
  rdyStatus,
  armStatus
};

const char mm0[] PROGMEM = "Press # to exit";
const char mm1[] PROGMEM = "Zone Bypass";
const char mm2[] PROGMEM = "System Troubles";
const char mm3[] PROGMEM = "Alarm Memory";
const char mm4[] PROGMEM = "Door Chime";
const char mm5[] PROGMEM = "Access Codes";
const char mm6[] PROGMEM = "User Functions";
const char mm7[] PROGMEM = "Output Contact";
const char mm8[] PROGMEM = " ";
const char mm9[] PROGMEM = "No-Entry Arm";
const char mm10[] PROGMEM = "Quick Arming";

const char om0[] PROGMEM = "Press # to exit";
const char om1[] PROGMEM = "O/P 1";
const char om2[] PROGMEM = "O/P 2";

const char tm0[] PROGMEM = "Press # to exit";
const char tm1[] PROGMEM = "Service Required *";
const char tm2[] PROGMEM = "AC Failure";
const char tm3[] PROGMEM = "Tel Line Trouble";
const char tm4[] PROGMEM = "Comm Failure";
const char tm5[] PROGMEM = "Zone Fault *";
const char tm6[] PROGMEM = "Zone Tamper *";
const char tm7[] PROGMEM = "Low Battery *";
const char tm8[] PROGMEM = "System Time";

const char sm0[] PROGMEM = "Press # to exit";
const char sm1[] PROGMEM = "Low Battery";
const char sm2[] PROGMEM = "Bell Circuit";
const char sm3[] PROGMEM = "System Trouble";
const char sm4[] PROGMEM = "System Tamper";
const char sm5[] PROGMEM = "Mod Supervision";
const char sm6[] PROGMEM = "RF Jam detected";
const char sm7[] PROGMEM = "PC5204 Low Battery";
const char sm8[] PROGMEM = "PC5204 AC Fail";

const char um0[] PROGMEM = "Press # to exit";
const char um1[] PROGMEM = "Time and Date";
const char um2[] PROGMEM = "Auto Arm/Disarm";
const char um3[] PROGMEM = "Auto Arm Time";
const char um4[] PROGMEM = "System Test";
const char um5[] PROGMEM = "System Serv/DLS";
const char um6[] PROGMEM = "Event Buffer";

const char am0[] PROGMEM = " ";
const char am1[] PROGMEM = " ";
const char am2[] PROGMEM = "System Trouble:(*2) to view";
const char am3[] PROGMEM = "Bypass Active:(*1) to view";
const char am4[] PROGMEM = "Alarm Memory: (*3) to view";
const char am5[] PROGMEM = "Open Zones:Scroll to view <>";

const char ml0[] PROGMEM = "System is Ready:Ready to Arm <>";
const char ml1[] PROGMEM = "Secure System:Before Arming <>";

const char *
  const mainMenu[] PROGMEM = {
    mm0,
    mm1,
    mm2,
    mm3,
    mm4,
    mm5,
    mm6,
    mm7,
    mm8,
    mm9,
    mm10
  };
const char *
  const outputMenu[] PROGMEM = {
    om0,
    om1,
    om2
  };
const char *
  const troubleMenu[] PROGMEM = {
    tm0,
    tm1,
    tm2,
    tm3,
    tm4,
    tm5,
    tm6,
    tm7,
    tm8
  };
const char *
  const serviceMenu[] PROGMEM = {
    sm0,
    sm1,
    sm2,
    sm3,
    sm4,
    sm5,
    sm6,
    sm7,
    sm8
  };
const char *
  const userMenu[] PROGMEM = {
    um0,
    um1,
    um2,
    um3,
    um4,
    um5,
    um6
  };
const char *
  const statusMenu[] PROGMEM = {
    am0,
    am1,
    am2,
    am3,
    am4,
    am5
  };
const char *
  const statusMenuLabels[] PROGMEM = {
    ml0,
    ml1
  };

const char STATUS_PENDING[] PROGMEM = "pending";
const char STATUS_ARM[] PROGMEM = "armed_away";
const char STATUS_STAY[] PROGMEM = "armed_home";
const char STATUS_NIGHT[] PROGMEM = "armed_night";
const char STATUS_OFF[] PROGMEM = "disarmed";
const char STATUS_ONLINE[] PROGMEM = "online";
const char STATUS_OFFLINE[] PROGMEM = "offline";
const char STATUS_TRIGGERED[] PROGMEM = "triggered";
const char STATUS_READY[] PROGMEM = "ready";
const char STATUS_NOT_READY[] PROGMEM = "unavailable"; //ha alarm panel likes to see "unavailable" instead of not_ready when the system can't be armed

#ifdef ESP32
class DSCkeybushome: public CustomAPIDevice, public RealTimeClock {
#else
class DSCkeybushome: public CustomAPIDevice, public PollingComponent {    
#endif
  public: DSCkeybushome(byte dscClockPin = 0, byte dscReadPin = 0, byte dscWritePin = 0): dscClockPin(dscClockPin),
  dscReadPin(dscReadPin),
  dscWritePin(dscWritePin) {}

  std:: function < void(uint8_t, bool) > zoneStatusChangeCallback;
  std:: function < void(std::string) > systemStatusChangeCallback;
  std:: function < void(panelStatus, bool, int) > panelStatusChangeCallback;
  std:: function < void(bool, int) > fireStatusChangeCallback;
  std:: function < void(std::string, int) > partitionStatusChangeCallback;
  std:: function < void(std::string) > zoneMsgStatusCallback;
  std:: function < void(std::string) > troubleMsgStatusCallback;
  std:: function < void(uint8_t, bool) > relayChannelChangeCallback;
  std:: function < void(std::string, int) > line1DisplayCallback;
  std:: function < void(std::string, int) > line2DisplayCallback;
  std:: function < void(std::string) > eventInfoCallback;
  std:: function < void(std::string, int) > lightsCallback;
  std:: function < void(std::string, int) > beepsCallback;

  void onZoneStatusChange(std:: function < void(uint8_t zone, bool isOpen) > callback) {
    zoneStatusChangeCallback = callback;
  }
  void onSystemStatusChange(std:: function < void(std::string status) > callback) {
    systemStatusChangeCallback = callback;
  }
  void onFireStatusChange(std:: function < void(bool isOpen, int partition) > callback) {
    fireStatusChangeCallback = callback;
  }
  void onPanelStatusChange(std:: function < void(panelStatus ts, bool isOpen, int partition) > callback) {
    panelStatusChangeCallback = callback;
  }
  void onPartitionStatusChange(std:: function < void(std::string status, int partition) > callback) {
    partitionStatusChangeCallback = callback;
  }
  void onZoneMsgStatus(std:: function < void(std::string msg) > callback) {
    zoneMsgStatusCallback = callback;
  }
  void onTroubleMsgStatus(std:: function < void(std::string msg) > callback) {
    troubleMsgStatusCallback = callback;
  }
  void onRelayChannelChange(std:: function < void(uint8_t channel, bool state) > callback) {
    relayChannelChangeCallback = callback;
  }

  void onLine1Display(std:: function < void(std::string msg, int partition) > callback) {
    line1DisplayCallback = callback;
  }
  void onLine2Display(std:: function < void(std::string msg, int partition) > callback) {
    line2DisplayCallback = callback;
  }
  void onEventInfo(std:: function < void(std::string msg) > callback) {
    eventInfoCallback = callback;
  }
  void onLights(std:: function < void(std::string msg, int partition) > callback) {
    lightsCallback = callback;
  }
  void onBeeps(std:: function < void(std::string beep, int partition) > callback) {
    beepsCallback = callback;
  }

  char expanderAddr1,
  expanderAddr2,
  expanderAddr3;
  byte debug;
  const char * accessCode;
  unsigned long cmdWaitTime,
  beepTime,
  eventTime;
  bool extendedBufferFlag=false;
  int defaultPartition = 1;
  int activePartition = 1;
  byte maxZones = maxZonesDefault;

  private: uint8_t zone;
  byte dscClockPin,
  dscReadPin,
  dscWritePin;
  bool firstrun;
  bool options;

  struct partitionType {
    bool locked=false;
    unsigned long keyPressTime;
    byte lastStatus;
    byte status;
    bool inprogram=false;
    byte digits;
    bool newData;
    bool hexMode;
    byte editIdx;
    bool decimalInput;
    bool hex;
    bool eventViewer;
    bool submitted;
    byte currentSelection;
    byte selectedZone;

  };

  struct zoneType {
    bool tamper;
    bool batteryLow;
    bool open;
    bool alarm;
    bool enabled;
    byte partition;
    bool bypassed;

  };

  zoneType * zoneStatus;
  partitionType partitionStatus[dscPartitions];
  std::string zoneStatusMsg,
  previousZoneStatusMsg,
  systemMsg,
  previousSystemMsg,
  group1msg,
  group2msg,
  eventStatusMsg;
  bool relayStatus[16],
  previousRelayStatus[16];
  bool sendCmd,forceRefresh;
  byte system0,
  system1,
  previousSystem0,
  previousSystem1;
  byte programZones[dscZones];
  char decimalInputBuffer[6];
  byte line2Digit,
  line2Status;
  byte beeps,
  previousBeeps;
  bool refresh;
  


  void setup() override {

    zoneStatus = new zoneType[maxZones];
    if (debug > 2)
      Serial.begin(115200);
    set_update_interval(10);

    register_service( & DSCkeybushome::set_alarm_state, "set_alarm_state", {
      "state",
      "code",
      "partition"
    });
    register_service( & DSCkeybushome::alarm_disarm, "alarm_disarm", {
      "code"
    });
#ifdef ESP32
    register_service( & DSCkeybushome::set_panel_time, "set_panel_time", {});
#else
    register_service( & DSCkeybushome::set_panel_time, "set_panel_time", {
     "year","month","day","hour","minute"
    });
#endif
    register_service( & DSCkeybushome::alarm_arm_home, "alarm_arm_home");
    register_service( & DSCkeybushome::alarm_arm_night, "alarm_arm_night", {
      "code"
    });
    register_service( & DSCkeybushome::alarm_arm_away, "alarm_arm_away");
    register_service( & DSCkeybushome::alarm_trigger_panic, "alarm_trigger_panic");
    register_service( & DSCkeybushome::alarm_trigger_fire, "alarm_trigger_fire");
    register_service( & DSCkeybushome::alarm_keypress, "alarm_keypress", {
      "keys"
    });
    register_service( & DSCkeybushome::alarm_keypress_partition, "alarm_keypress_partition", {
      "keys",
      "partition"
    });
    register_service( & DSCkeybushome::set_zone_fault, "set_zone_fault", {
      "zone",
      "fault"
    });
    register_service( & DSCkeybushome::set_default_partition, "set_default_partition", {
      "partition"
    });

    firstrun = true;
    systemStatusChangeCallback(String(FPSTR(STATUS_OFFLINE)).c_str());
    forceDisconnect = false;
    #ifdef MODULESUPERVISION
    dsc.enableModuleSupervision = 1;
    #endif
    dsc.addModule(expanderAddr1);
    dsc.addModule(expanderAddr2);
    dsc.maxZones = maxZones;
    dsc.resetStatus();
    dsc.processModuleData = true;

    if (dscClockPin && dscReadPin && dscWritePin)
      dsc.begin(Serial, dscClockPin, dscReadPin, dscWritePin);
    else
      dsc.begin(Serial);

    for (int x = 0; x < maxZones; x++) {
      zoneStatus[x].tamper = false;
      zoneStatus[x].batteryLow = false;
      zoneStatus[x].open = false;
      zoneStatus[x].alarm = false;
      zoneStatus[x].enabled = false;
      zoneStatus[x].partition = 0;
      zoneStatus[x].bypassed = false;
    } 

    system1 = 0;
    system0 = 0;
  }
  
 std::string getUserName(char * code) {
  std::string name = code;
  if (userCodes -> value() != "") {
    std::string s = userCodes -> value();
    std::string token1, token2, token3;
    size_t pos, pos1;
    char buf[4];
    s.append(",");
    while ((pos = s.find(',')) != std::string::npos) {
      token1 = s.substr(0, pos); // store the substring   
      pos1 = token1.find(':');

      token2 = token1.substr(0, pos1);
      token3 = token1.substr(pos1 + 1);
      if (token2 == code) {
        name = token3;
        break;
      }
      s.erase(0, pos + 1); /* erase() function store the current positon and move to next token. */
    }
    //ESP_LOGD("info", "token=%s,token2=%s,token3=%s,name=%s,code=%s", token1.c_str(), token2.c_str(), token3.c_str(), name.c_str(), code);
  }
  return name;
}
  void set_default_partition(int partition) {
    if (partition > 0 && partition < dscPartitions) {
      defaultPartition = partition;
      dsc.currentDefaultPartition = partition;
    }

  }

  void set_zone_fault(int zone, bool fault) {
    ESP_LOGD("Debug", "Setting Zone Fault: %d,%d", zone, fault);
    dsc.setZoneFault(zone, fault);

  }

  void alarm_disarm(std::string code) {
      
    set_alarm_state("D", code, defaultPartition);

  }

  void alarm_arm_home() {

    set_alarm_state("S", "", defaultPartition);

  }

  void alarm_arm_night(std::string code) {

    set_alarm_state("N", code, defaultPartition);

  }

  void alarm_arm_away() {

    set_alarm_state("A", "", defaultPartition);

  }

  void alarm_trigger_fire() {

    set_alarm_state("F", "", defaultPartition);

  }

  void alarm_trigger_panic() {

    set_alarm_state("P", "", defaultPartition);

  }

  void processMenu(byte key, byte partition = -1) {

    if (partition < 1) partition = defaultPartition;
    byte * currentSelection = & partitionStatus[partition - 1].currentSelection;

   // ESP_LOGD("info", "key %d pressed, state=%02X,current=%02X,digits=%d,hex=%d,partition=%d,locked=%d", key, dsc.status[partition - 1], * currentSelection, partitionStatus[partition - 1].digits, partitionStatus[partition - 1].hex, partition, partitionStatus[partition - 1].locked);

    if (partitionStatus[partition - 1].locked) {
      line1DisplayCallback("System", partition);
      line2DisplayCallback("not available", partition);
      return;
    }

    if (partitionStatus[partition - 1].digits > 0) { //program mode data input 

      std::string tpl;
      tpl = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
      if (dsc.status[partition - 1] == 0xAA) { //time entry
        tpl = "XXXX    XXXXXX  XXXXXXXXXXXXXXXX";
      }

      if (key == '#') {
        partitionStatus[partition - 1].newData = false;
        if (key == '#' && partitionStatus[partition - 1].hex) {
          dsc.setLCDSend(partition, (partitionStatus[partition - 1].digits > 8));
        } else {
          dsc.write(key, partition);
        }
        return;

      } else if (key == '>') {
        getNextIdx(tpl, partition);

        if (!partitionStatus[partition - 1].hex && partitionStatus[partition - 1].editIdx == 0) {
          dsc.setLCDSend(partition);
          partitionStatus[partition - 1].newData = false;
          return;
        }

      } else if (key == '<') {
        getPrevIdx(tpl, partition);

      } else if (key != '*') {
        if (partitionStatus[partition - 1].decimalInput) {
          int num;
          decimalInputBuffer[partitionStatus[partition - 1].editIdx] = key;
          sscanf(decimalInputBuffer, "%d", & num);
          dsc.pgmBuffer.data[0] = num;
          //convert data to int and store it to data[0];
        } else {
            //skip every 5th byte since it's a checksum
          byte y = (partitionStatus[partition - 1].editIdx / 2) + (partitionStatus[partition - 1].editIdx/8);
          char k = key - 48;
          if (partitionStatus[partition - 1].hexMode) {
            if (k < 6)
              k += 10;
            else
              return;
          }
          
          if (partitionStatus[partition - 1].editIdx % 2)
            dsc.pgmBuffer.data[y] = (dsc.pgmBuffer.data[y] & 0xF0) | k;
          else
            dsc.pgmBuffer.data[y] = (dsc.pgmBuffer.data[y] & 0x0F) | (k << 4);
        }
        getNextIdx(tpl, partition);
        if (!partitionStatus[partition - 1].hex && partitionStatus[partition - 1].editIdx == 0) {
          dsc.setLCDSend(partition);
          partitionStatus[partition - 1].newData = false;
          return;
        }

      } else if (key == '*') {
        if (partitionStatus[partition - 1].hex) {
          partitionStatus[partition - 1].hexMode = partitionStatus[partition - 1].hexMode ? false : true;
        } else {
          dsc.write(key, partition);
          return;
        }
      }
      dsc.write("$", partition); //place holder character . 
      setStatus(partition - 1, true);
      return;
    }

    if (key == '#') {
      * currentSelection = 0xFF;
      partitionStatus[partition - 1].selectedZone = 0;
      dsc.write(key, partition);
      partitionStatus[partition - 1].eventViewer = false;
      activePartition = 1;
      setStatus(partition - 1, true);
      return;
    }

    if (dsc.status[partition - 1] < 0x04) {
      if (dsc.keybusVersion1) { //older versions don't support top level navigation 
        if (key == '<' || key == '>') return;
      } else {

        if (key == '<') {
          getPreviousMainOption(partition);
        } else
        if (key == '>') {
          getNextMainOption(partition);
        };
      }
      dsc.write(key, partition);
      setStatus(partition - 1, true);

    } else if (dsc.status[partition - 1] == 0x9E) { // * mainmenu
      if (key == '<') {
        * currentSelection = * currentSelection >= 11 ? 10 : ( * currentSelection > 0 ? * currentSelection - 1 : 10);
        if (*currentSelection == 8) *currentSelection-=1; //skip empty item
        if ( * currentSelection < 11)
          line2DisplayCallback(String(FPSTR(mainMenu[ * currentSelection])).c_str(), partition);
      } else if (key == '>') {
        * currentSelection = * currentSelection >= 10 ? 0 : * currentSelection + 1;
        if (*currentSelection == 8) *currentSelection+=1;
        if ( * currentSelection < 11)
          line2DisplayCallback(String(FPSTR(mainMenu[ * currentSelection])).c_str(), partition);
      } else if (key == '*' && * currentSelection > 0) {
        dsc.write(key, partition);
        char s[5];
        sprintf(s, "%d", * currentSelection % 10);
        const char * out = strcpy(new char[3], s);
        * currentSelection = 0xFF;
        dsc.write(s, partition);
      } else {
        dsc.write(key, partition);
        * currentSelection = 0xFF;
      }
    } else if (dsc.status[partition - 1] == 0xA1) { //trouble
      if (key == '*' && * currentSelection > 0) {
        char s[5];
        sprintf(s, "%d", * currentSelection);
        const char * out = strcpy(new char[3], s);
        * currentSelection = 0xFF;
        dsc.write(out, partition);
      } else if (key == '>') {
        * currentSelection = getNextOption( * currentSelection);
        if ( * currentSelection < 9)
          line2DisplayCallback(String(FPSTR(troubleMenu[ * currentSelection])).c_str(), partition);
      } else if (key == '<') {
        * currentSelection = getPreviousOption( * currentSelection);
        if ( * currentSelection < 9)
          line2DisplayCallback(String(FPSTR(troubleMenu[ * currentSelection])).c_str(), partition);
      } else {
        * currentSelection = 0xFF;
        dsc.write(key, partition);
      }
    } else if (dsc.status[partition - 1] == 0xC8) { //service
      if (key == '*' && * currentSelection > 0) {
        char s[5];
        sprintf(s, "%d", * currentSelection);
        const char * out = strcpy(new char[3], s);
        * currentSelection = 0xFF;
        dsc.write(out, partition);
      } else if (key == '>') {
        * currentSelection = getNextOption( * currentSelection);
        if ( * currentSelection < 9)
          line2DisplayCallback(String(FPSTR(serviceMenu[ * currentSelection])).c_str(), partition);
      } else if (key == '<') {
        * currentSelection = getPreviousOption( * currentSelection);
        if ( * currentSelection < 9)
          line2DisplayCallback(String(FPSTR(serviceMenu[ * currentSelection])).c_str(), partition);
      } else {
        * currentSelection = 0xFF;
        dsc.write(key, partition);
      }

    } else if (dsc.status[partition - 1] == 0xA9 && !partitionStatus[partition - 1].eventViewer) { // * user functions
      if (key == '<') {
        * currentSelection = * currentSelection >= 7 ? 6 : ( * currentSelection > 0 ? * currentSelection - 1 : 6);
        if ( * currentSelection < 7)
          line2DisplayCallback(String(FPSTR(userMenu[ * currentSelection])).c_str(), partition);
      } else if (key == '>') {
        * currentSelection = * currentSelection >= 6 ? 0 : * currentSelection + 1;
        if ( * currentSelection < 7)
          line2DisplayCallback(String(FPSTR(userMenu[ * currentSelection])).c_str(), partition);
      } else if (key == '*' && * currentSelection > 0) {
        char s[5];
        if ( * currentSelection == 6) { //event viewer. 
          partitionStatus[partition - 1].eventViewer = true;
          activePartition = partition;
          dsc.write('b', partition);
        } else {
          sprintf(s, "%d", * currentSelection % 6);
          const char * out = strcpy(new char[3], s);
          * currentSelection = 0xFF;
          dsc.write(s, partition);
        }
      } else {
        dsc.write(key, partition);
        * currentSelection = 0xFF;
      }
    } else if (dsc.status[partition - 1] == 0xA2) { //alarm memory
      if (key == '>') {
        * currentSelection = getNextOption( * currentSelection);
        dsc.write(key, partition);
      } else if (key == '<') {
        * currentSelection = getPreviousOption( * currentSelection);
        dsc.write(key, partition);
      } else {
        * currentSelection = 0xFF;
        dsc.write(key, partition);
      }
    } else if (dsc.status[partition - 1] == 0xA6) { //user codes
      if (key == '*') {
        char s[5];
        sprintf(s, "%02d", * currentSelection);
        const char * out = strcpy(new char[3], s);
        dsc.write(out, partition);
      } else if (key == '>') {
        * currentSelection = getNextUserCode( * currentSelection);
        dsc.write(key, partition);
      } else if (key == '<') {
        * currentSelection = getPreviousUserCode( * currentSelection);
        dsc.write(key, partition);
      } else {
        * currentSelection = 0xFF;
        dsc.write(key, partition);
      }
      setStatus(partition - 1, true);
    } else if (dsc.status[partition - 1] == 0x11) { //alarms
      if (key == '>') {
        * currentSelection = getNextAlarmedZone( * currentSelection, partition);
        dsc.write(key, partition);
      } else if (key == '<') {
        * currentSelection = getPreviousAlarmedZone( * currentSelection, partition);
        dsc.write(key, partition);
      } else {
        * currentSelection = 0xFF;
        dsc.write(key, partition);
      }
      setStatus(partition - 1, true);
    } else if (dsc.status[partition - 1] == 0xA0) { //bypass
      if (key == '*') {
        char s[5];
        sprintf(s, "%02d", * currentSelection);
        const char * out = strcpy(new char[3], s);
        dsc.write(out, partition);
      } else if (key == '>') {
        * currentSelection = getNextEnabledZone( * currentSelection, partition);
        dsc.write(key, partition);
      } else if (key == '<') {
        * currentSelection = getPreviousEnabledZone( * currentSelection, partition);
        dsc.write(key, partition);
      } else {
        * currentSelection = 0xFF;
        dsc.write(key, partition);
      }
      setStatus(partition - 1, true);
    } else if (dsc.status[partition - 1] == 0xB2) { //output control
      if (key == '<') {
        * currentSelection = * currentSelection >= 3 ? 2 : ( * currentSelection > 0 ? * currentSelection - 1 : 2);
        if ( * currentSelection < 3)
          line2DisplayCallback(String(FPSTR(outputMenu[ * currentSelection])).c_str(), partition);
        dsc.write(key, partition);
      } else if (key == '>') {
        * currentSelection = * currentSelection >= 2 ? 0 : * currentSelection + 1;
        if ( * currentSelection < 3)
          line2DisplayCallback(String(FPSTR(outputMenu[ * currentSelection])).c_str(), partition);
        dsc.write(key, partition);
      } else if (key == '*' && * currentSelection > 0) {
        char s[5];
        sprintf(s, "%d", * currentSelection);
        const char * out = strcpy(new char[3], s);
        * currentSelection = 0xFF;
        dsc.write(s, partition);
      } else {
        * currentSelection = 0xFF;
        dsc.write(key, partition);
      }
      setStatus(partition - 1, true);
    } else {
      dsc.write(key, partition);
      setStatus(partition - 1, false);
    }

  }

  void getPrevIdx(std::string tpl, byte partition) {
    int count = 0;
    do {
      partitionStatus[partition - 1].editIdx = partitionStatus[partition - 1].editIdx > 0 ? partitionStatus[partition - 1].editIdx - 1 : partitionStatus[partition - 1].digits - 1;
      count++;
       byte b = (partitionStatus[partition - 1].editIdx / 2) + (partitionStatus[partition - 1].editIdx % 2);  
      //if ((b%4)) continue;       
    } while (tpl[partitionStatus[partition - 1].editIdx] != 'X' && count <= partitionStatus[partition - 1].digits);

  }

  void getNextIdx(std::string tpl, byte partition) {
    int count = 0;
    do {
      partitionStatus[partition - 1].editIdx = partitionStatus[partition - 1].editIdx + 1 < partitionStatus[partition - 1].digits ? partitionStatus[partition - 1].editIdx + 1 : partitionStatus[partition - 1].editIdx = 0;
      count++;
       byte b = (partitionStatus[partition - 1].editIdx / 2) + (partitionStatus[partition - 1].editIdx % 2);  
      //if ((b%4)) continue;        
    } while (tpl[partitionStatus[partition - 1].editIdx] != 'X' && count <= partitionStatus[partition - 1].digits);

  }
#ifdef ESP32
  void set_panel_time() {

    ESPTime rtc = now();
    dsc.setDateTime(rtc.year, rtc.month, rtc.day_of_month, rtc.hour, rtc.minute);
  
  }
  #else
  void set_panel_time(int year,int month,int day,int hour,int minute) {
  dsc.setDateTime(year, month, day, hour, minute);
  }  
#endif 
  void alarm_keypress(std::string keystring) {
    alarm_keypress_partition(keystring, defaultPartition);
  }

  void alarm_keypress_partition(std::string keystring, int partition) {
    if (!partition) partition = defaultPartition;
    if (dsc.disabled[partition - 1]) return;
    const char * keys = strcpy(new char[keystring.length() + 1], keystring.c_str());
    if (debug > 0) ESP_LOGD("Debug", "Writing keys: %s to partition %d", keystring.c_str(), partition);
    partitionStatus[partition - 1].keyPressTime = millis();
    if (keystring.length() == 1) {
      processMenu(keys[0], partition);
    } else {
        ESP_LOGD("info","partition is %d",partitionStatus[partition].locked);
        if (!partitionStatus[partition].locked) dsc.write(keys, partition);
    }
  }

  bool isInt(std::string s, int base) {
    if (s.empty() || std::isspace(s[0])) return false;
    char * p;
    strtol(s.c_str(), & p, base);
    return ( * p == 0);
  }

  void set_alarm_state(std::string state, std::string code = "", int partition = 0) {

    if (code.length() != 4 || !isInt(code, 10)) code = ""; // ensure we get a numeric 4 digit code
    const char * alarmCode = strcpy(new char[code.length() + 1], code.c_str());
    if (!partition) partition = defaultPartition;

    if (partitionStatus[partition - 1].locked) return;

    // Arm stay
    if (state.compare("S") == 0 && !dsc.armed[partition - 1] && !dsc.exitDelay[partition - 1]) {
      dsc.write('s', partition); // Virtual keypad arm stay
    }
    // Arm away
    else if (state.compare("A") == 0 && !dsc.armed[partition - 1] && !dsc.exitDelay[partition - 1]) {
      dsc.write('w', partition); // Virtual keypad arm away
    }
    // Arm night  ** this depends on the accessCode setup in the yaml
    else if (state.compare("N") == 0 && !dsc.armed[partition - 1] && !dsc.exitDelay[partition - 1]) {
      //ensure you have the accessCode setup correctly in the yaml for this to work
      dsc.write('n', partition); // Virtual keypad arm away
      if (code.length() == 4 && !isInt(accessCode, 10)) { // if the code is sent and the yaml code is not active use this.
        dsc.write(alarmCode, partition);
      }
    }
    // Fire command
    else if (state.compare("F") == 0) {
      dsc.write('f', partition); // Virtual keypad arm away
    }
    // Panic command
    else if (state.compare("P") == 0) {
      dsc.write('p', partition); // Virtual keypad arm away
    }
    // Disarm
    else if (state.compare("D") == 0 && (dsc.armed[partition-1] || dsc.exitDelay[partition-1])) {
      if (code.length() == 4) { // ensure we get 4 digit code
        dsc.write(alarmCode, partition);
      }
    }
    partitionStatus[partition-1].keyPressTime = millis();    
  }

  void printPacket(const char * label, char cmd, volatile byte cbuf[], int len) {

#ifdef ESP32
    ESPTime rtc = now();
    char s1[4];
    char s2[25];
    std::string s;
    s = "";
    sprintf(s2, "%02d-%02d-%02d %02d:%02d ", rtc.year, rtc.month, rtc.day_of_month, rtc.hour, rtc.minute);
    for (int c = 0; c < len; c++) {
      sprintf(s1, "%02X ", cbuf[c]);
      s = s.append(s1);
    }
    ESP_LOGI(label, "%s %02X: %s", s2, cmd, s.c_str());
#else
    char s1[4];
    std::string s;
    s = "";
    for (int c = 0; c < len; c++) {
      sprintf(s1, "%02X ", cbuf[c]);
      s = s.append(s1);
    }
    ESP_LOGI(label, "%02X: %s", cmd, s.c_str());
#endif    

  }

  byte getPanelBitNumber(byte panelByte, byte startNumber) {

    byte bitCount = 0;
    for (byte bit = 0; bit <= 7; bit++) {
      if (bitRead(dsc.panelData[panelByte], bit)) {
        return (startNumber + bitCount);
      }
      bitCount++;
    }
    return defaultPartition;
  }

  bool getEnabledZonesB1(byte inputByte, byte startZone, byte partition) {
    bool zonesEnabled = false;
    byte zone;
    for (byte panelByte = inputByte; panelByte <= inputByte + 3; panelByte++) {
      if (dsc.panelData[panelByte] != 0) {
        zonesEnabled = true;
        for (byte zoneBit = 0; zoneBit < 8; zoneBit++) {
          zone = (zoneBit + startZone) + ((panelByte - inputByte) * 8) - 1;
          if (zone >= maxZones) continue;
          if (bitRead(dsc.panelData[panelByte], zoneBit)) {
            zoneStatus[zone].partition = partition;               
            zoneStatus[zone].enabled = true;
          } else if (zoneStatus[zone].partition==partition) {
                zoneStatus[zone].enabled = false;
          }
         
        }
      }
    }
    return zonesEnabled;
  }

  bool getEnabledZonesE6(byte inputByte, byte startZone, byte partitionByte) {
    bool zonesEnabled = false;
    byte zone;
    byte partition = getPanelBitNumber(partitionByte, 1);
    for (byte panelByte = inputByte; panelByte <= inputByte + 3; panelByte++) {
      if (dsc.panelData[panelByte] != 0) {
        zonesEnabled = true;
        for (byte zoneBit = 0; zoneBit < 8; zoneBit++) {
          zone = (zoneBit + startZone) + ((panelByte - inputByte) * 8) - 1;
          if (zone >= maxZones) continue;
          if (bitRead(dsc.panelData[panelByte], zoneBit)) {
            zoneStatus[zone].partition = partition;               
            zoneStatus[zone].enabled = true;
          } else if (zoneStatus[zone].partition==partition) {
                zoneStatus[zone].enabled = false;
          }
        }
      }
    }
    return zonesEnabled;
  }

  void printTimestamp() {
    float timeStamp = millis() / 1000.0;
    if (timeStamp < 10) Serial.print("    ");
    else if (timeStamp < 100) Serial.print("   ");
    else if (timeStamp < 1000) Serial.print("  ");
    else if (timeStamp < 10000) Serial.print(" ");
    Serial.print(timeStamp, 2);
    Serial.print(F(":"));
  }

  std::string getOptionsString() {

    char s1[4];
    std::string options = " ";
    byte option;
    for (byte optionGroup = 0; optionGroup < dscZones; optionGroup++) {
      for (byte optionBit = 0; optionBit < 8; optionBit++) {
        option = optionBit + 1 + (optionGroup * 8);
        if (bitRead(programZones[optionGroup], optionBit)) {
          sprintf(s1, "%d", option);
          options.append(s1);
        }
      }
    }
    return options;
  }

  bool checkUserCode(byte code) {
    byte option, optionGroup;

    for (optionGroup = 0; optionGroup < 4; optionGroup++) {
      for (byte optionBit = 0; optionBit < 8; optionBit++) {
        option = optionBit + 1 + (optionGroup * 8);
        if (bitRead(programZones[optionGroup], optionBit) && option == code) {
          return true;
        }

      }
    }
    return false;
  }

  byte getNextOption(byte start) {
    byte option, optionGroup, s;
    s = start >= maxZones ? 0 : start;
    for (optionGroup = 0; optionGroup < dscZones; optionGroup++) {
      for (byte optionBit = 0; optionBit < 8; optionBit++) {
        option = optionBit + 1 + (optionGroup * 8);
        if (bitRead(programZones[optionGroup], optionBit) && option > s) return option;
      }
    }
    return 0;
  }

  byte getPreviousOption(byte start) {
    byte s;
    s = start >= maxZones || start == 0 ? maxZones : start;
    for (byte optionGroup = dscZones - 1; optionGroup >= 0 && optionGroup < dscZones; optionGroup--) {
      for (byte optionBit = 7; optionBit >= 0 && optionBit < 8; optionBit--) {
        byte option = optionBit + 1 + (optionGroup * 8);
        if (bitRead(programZones[optionGroup], optionBit)) {
          if (option < s) return option;
        }
      }
    }
    return 0;
  }

  byte getNextUserCode(byte start) {
    if (start < 95)
      return start + 1;
    else
      return 1;
  }

  byte getPreviousUserCode(byte start) {
    if (start > 1)
      return start - 1;
    else
      return 95;
  }

  void getPreviousMainOption(byte partition) {

    byte menu = 0;
    byte zone = 0;

    if (partitionStatus[partition - 1].currentSelection > 5) {
      partitionStatus[partition - 1].currentSelection = 0;
      partitionStatus[partition - 1].selectedZone = 0;
      return;
    } else if (partitionStatus[partition - 1].currentSelection == 5) { //open zones
      partitionStatus[partition - 1].selectedZone = getPreviousOpenZone(partitionStatus[partition - 1].selectedZone, partition);
      if (partitionStatus[partition - 1].selectedZone) return;
    } else if (partitionStatus[partition - 1].currentSelection < 2)
      partitionStatus[partition - 1].currentSelection = 6;

    for (int x = partitionStatus[partition - 1].currentSelection; x >= 0; x--) {

      if ((x == 6) && !dsc.armed[partition - 1] && !dsc.armed[partition - 1]) { //openzones
        menu = 5;
        zone = 0;
        break;
      } else if (x == 3 && dsc.lights[partition - 1] & 0x10) { //trouble
        menu = 2;
        zone = 0;
        break;
      } else if (x == 4 && dsc.lights[partition - 1] & 0x08) { //bypass
        menu = 3;
        zone = 0;
        break;
      } else if (x == 5 && getNextAlarmedZone(0, partition)) { //alarms
        menu = 4;
        zone = 0;
        break;
      }
    }
    partitionStatus[partition - 1].currentSelection = menu;
    partitionStatus[partition - 1].selectedZone = zone;

  }

  void clearZoneAlarms(byte partition) {
    for (int zone = 0; zone < maxZones; zone++) {
      if (zoneStatus[zone].partition == partition)
        zoneStatus[zone].alarm = false;
    }
  }

  void clearZoneBypass(byte partition) {
    for (int zone = 0; zone < maxZones; zone++) {
      if (zoneStatus[zone].partition == partition)
        zoneStatus[zone].bypassed = false;
    }
  }

  byte getNextOpenZone(byte start, byte partition) {
    if (start >= maxZones) start = 0;
    for (int zone = start; zone < maxZones; zone++) {
      if (zoneStatus[zone].enabled && zoneStatus[zone].partition == partition && zoneStatus[zone].open) return zone + 1;
    }
    return 0;
  }

  byte getPreviousOpenZone(byte start, byte partition) {
    if (start == 1) return 0;
    if (start == 0 || start > maxZones) start = maxZones;
    for (int zone = start - 2; zone >= 0; zone--) {
      if (zoneStatus[zone].enabled && zoneStatus[zone].partition == partition && zoneStatus[zone].open) return zone + 1;
    }
    return 0;
  }

  void getNextMainOption(byte partition) {

    byte menu = 0;
    byte zone = 0;

    if (partitionStatus[partition - 1].currentSelection > 5) {
      partitionStatus[partition - 1].currentSelection = 0;
      return;
    } else if (partitionStatus[partition - 1].currentSelection == 5) { //open zones
      partitionStatus[partition - 1].selectedZone = getNextOpenZone(partitionStatus[partition - 1].selectedZone, partition);
      if (partitionStatus[partition - 1].selectedZone) return;
    }

    for (int x = partitionStatus[partition - 1].currentSelection; x < 6; x++) {
      if ((x == 0 || x == 1) && dsc.lights[partition - 1] & 0x10) { //trouble
        menu = 2;
        zone = 0;
        break;
      } else if (x == 2 && dsc.lights[partition - 1] & 0x08) { //bypass
        menu = 3;
        zone = 0;
        break;
      } else if (x == 3 && getNextAlarmedZone(0, partition)) { //alarm
        menu = 4;
        zone = 0;
        break;
      } else if (x == 4 && !dsc.armed[partition - 1] && !dsc.armed[partition - 1]) { //open
        menu = 5;
        zone = 0;
        break;
      }
    }
    partitionStatus[partition - 1].currentSelection = menu;
    partitionStatus[partition - 1].selectedZone = zone;

  }

  byte getNextEnabledZone(byte start, byte partition) {
    if (start >= maxZones) start = 0;
    for (int zone = start; zone < maxZones; zone++) {
      if (zoneStatus[zone].partition == partition && zoneStatus[zone].enabled) return zone + 1;
    }
    return 0;
  }

  byte getPreviousEnabledZone(byte start, byte partition) {

    if (start < 2 || start > maxZones) start = maxZones;
    for (int zone = start - 2; zone >= 0 && zone < maxZones; zone--) {
      if (zoneStatus[zone].partition == partition && zoneStatus[zone].enabled) return zone + 1;
    }
    return 0;
  }

  byte getNextAlarmedZone(byte start, byte partition) {
    if (start >= maxZones) start = 0;
    for (int zone = start; zone < maxZones; zone++) {
      if (zoneStatus[zone].partition == partition && zoneStatus[zone].alarm) return zone + 1;
    }
    return 0;
  }

  byte getPreviousAlarmedZone(byte start, byte partition) {
    if (start < 2 || start > maxZones) start = maxZones;
    for (int zone = start - 2; zone >= 0 && zone < maxZones; zone--) {
      if (zoneStatus[zone].partition == partition && zoneStatus[zone].alarm) return zone + 1;
    }
    return 0;
  }

  void getBypassZones(byte partition) {
    for (byte zoneGroup = 0; zoneGroup < dscZones; zoneGroup++) {
      for (byte zoneBit = 0; zoneBit < 8; zoneBit++) {
        zone = zoneBit + (zoneGroup * 8);
        if (!(zoneStatus[zone].partition == partition + 1 && zoneStatus[zone].enabled) || zone >= maxZones) continue;
        if (bitRead(programZones[zoneGroup], zoneBit)) {
          zoneStatus[zone].bypassed = true;
        } else {
          zoneStatus[zone].bypassed = false;
        }
      }
    }
    setStatus(partition, true);
  }

  void rtrim(std::string & s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
      std::not1(std::ptr_fun < int, int > (std::isspace))).base(), s.end());
  }

  void update() override {

    if ((millis() - beepTime > 2000 && beeps > 0)) {
      beeps = 0;
      for (byte partition = 1; partition <= dscPartitions; partition++) {
        if (dsc.disabled[partition - 1]) continue;
        beepsCallback("0", partition);
      }
      beepTime = millis();

    }
    if (millis() - eventTime > 30000) {
      eventInfoCallback("");
      eventTime = millis();
    }
    
 
    static unsigned long refreshTime;
    if (!firstrun && millis() - refreshTime > 60000 ) {
              refreshTime=millis();
              forceRefresh=true;
             
    }


    if ((!forceDisconnect && dsc.loop()) || forceRefresh) { //Processes data only when a valid Keybus command has been read
     
      
      

      static bool delayedStart = true;
      static unsigned long startWait = millis();
      if (millis() - startWait > 10000 && delayedStart) {
        delayedStart = false;
        if (!dsc.disabled[defaultPartition-1] && !partitionStatus[defaultPartition-1].locked) {
          partitionStatus[defaultPartition-1].keyPressTime = millis();
          dsc.write("*21#7##", defaultPartition); //fetch panel troubles /zone module low battery
        }
        for (byte partition = 1; partition <= dscPartitions; partition++) {
          if (dsc.disabled[partition - 1]) continue;
          beepsCallback("0", partition);
        }
        dsc.clearZoneRanges(); // start with clear expanded zones
 
      }

      if (debug > 1)
        printPacket("Paneldata: ", dsc.panelData[0], dsc.panelData, 16);
      #ifdef SERIALDEBUGCOMMANDS
      if (debug > 2) {
        printTimestamp();
        Serial.print(" ");
        dsc.printPanelBinary(); // Optionally prints without spaces: printPanelBinary(false);
        Serial.print(" [");
        dsc.printPanelCommand(); // Prints the panel command as hex
        Serial.print("] ");
        dsc.printPanelMessage(); // Prints the decoded message
        Serial.println();
      }
      #endif

      processStatus();
      for (byte partition = 0; partition < dscPartitions; partition++) {
        if (dsc.disabled[partition] || dsc.status[partition] != 0xA0) continue;
        getBypassZones(partition);
      }

    }

    if (!forceDisconnect && ( dsc.statusChanged || forceRefresh)) { // Processes data only when a valid Keybus command has been read and statuses were changed
      dsc.statusChanged = false; // Reset the status tracking flag

      if (debug == 1)
        printPacket("Paneldata: ", dsc.panelData[0], dsc.panelData, 16);

      for (byte partition = 0; partition < dscPartitions; partition++) {
        if (firstrun)
          beepsCallback("0", partition + 1);
        if (dsc.disabled[partition]) continue;
        setStatus(partition, true);

      }

      if (dsc.bufferOverflow) ESP_LOGD("Error", "Keybus buffer overflow");
      dsc.bufferOverflow = false;

      // Checks if the interface is connected to the Keybus
      if (dsc.keybusChanged || forceRefresh) {
        dsc.keybusChanged = false; // Resets the Keybus data status flag
        if (dsc.keybusConnected) {
          systemStatusChangeCallback( String(FPSTR(STATUS_ONLINE)).c_str());
        } else systemStatusChangeCallback( String(FPSTR(STATUS_OFFLINE)).c_str());
      }

      if (dsc.powerChanged || forceRefresh) {
        dsc.powerChanged = false;
        if (dsc.powerTrouble) {
          panelStatusChangeCallback(acStatus, false, 0); //no ac
        } else panelStatusChangeCallback(acStatus, true, 0);

      }

      if (dsc.batteryChanged || forceRefresh) {
        dsc.batteryChanged = false;
        if (dsc.batteryTrouble) {
          panelStatusChangeCallback(batStatus, true, 0);
        } else panelStatusChangeCallback(batStatus, false, 0);
      }

      if (dsc.keypadFireAlarm) {
        dsc.keypadFireAlarm = false;
        //panelStatusChangeCallback(fireStatus, true,0); //needs status reset
      }

      if (dsc.keypadPanicAlarm) {
        dsc.keypadPanicAlarm = false;
        //panelStatusChangeCallback(panicStatus, true,0);
      }

      // Publishes trouble status
      if (dsc.troubleChanged || forceRefresh) {
        dsc.troubleChanged = false; // Resets the trouble status flag
        if (dsc.trouble) panelStatusChangeCallback(trStatus, true, 0); // Trouble alarm tripped
        else panelStatusChangeCallback(trStatus, false, 0); // Trouble alarm restored
      }

      // Publishes status per partition
      for (byte partition = 0; partition < dscPartitions; partition++) {

        if (dsc.disabled[partition] || partitionStatus[partition].locked) continue;



        // Publishes alarm status
        if (dsc.alarmChanged[partition] || forceRefresh) {
          //ESP_LOGD("test","dsc alarm=%d",dsc.alarm[partition]);
          dsc.alarmChanged[partition] = false; // Resets the partition alarm status flag
          if (dsc.alarm[partition]) {
            dsc.readyChanged[partition] = false; //if we are triggered no need to trigger a ready state change
            dsc.armedChanged[partition] = false; // no need to display armed changed
            partitionStatusChangeCallback( String(FPSTR(STATUS_TRIGGERED)).c_str(), partition + 1);
          }
        }

        // Publishes armed/disarmed status
        if (dsc.armedChanged[partition] || forceRefresh) {
          dsc.armedChanged[partition] = false; // Resets the partition armed status flag
          if (dsc.armed[partition]) {
            panelStatusChangeCallback(armStatus, true, partition + 1);
            if ((dsc.armedAway[partition] || dsc.armedStay[partition]) && dsc.noEntryDelay[partition]) partitionStatusChangeCallback( String(FPSTR(STATUS_NIGHT)).c_str(), partition + 1);
            else if (dsc.armedStay[partition])
              partitionStatusChangeCallback( String(FPSTR(STATUS_STAY)).c_str(), partition + 1);
            else partitionStatusChangeCallback( String(FPSTR(STATUS_ARM)).c_str(), partition + 1);
            clearZoneAlarms(partition + 1);
          } else if (!forceRefresh) {
            clearZoneBypass(partition + 1);
            partitionStatusChangeCallback( String(FPSTR(STATUS_OFF)).c_str(), partition + 1);
            panelStatusChangeCallback(armStatus, false, partition + 1);
          }
        }
        // Publishes exit delay status
        if (dsc.exitDelayChanged[partition]) {
          dsc.exitDelayChanged[partition] = false; // Resets the exit delay status flag
          if (dsc.exitDelay[partition]) partitionStatusChangeCallback( String(FPSTR(STATUS_PENDING)).c_str(), partition + 1);
          else if (!dsc.exitDelay[partition] && !dsc.armed[partition]) partitionStatusChangeCallback( String(FPSTR(STATUS_OFF)).c_str(), partition + 1);
        }

        // Publishes ready status

        if (dsc.readyChanged[partition] || forceRefresh) {

          dsc.readyChanged[partition] = false; // Resets the partition alarm status flag
          if (dsc.ready[partition] && !dsc.armed[partition]) {
            partitionStatusChangeCallback( String(FPSTR(STATUS_OFF)).c_str(), partition + 1);
            panelStatusChangeCallback(rdyStatus, true, partition + 1);
          } else {
            if (!dsc.armed[partition]) partitionStatusChangeCallback( String(FPSTR(STATUS_NOT_READY)).c_str(), partition + 1);
            panelStatusChangeCallback(rdyStatus, false, partition + 1);
          }

        }

        // Publishes fire alarm status
        if (dsc.fireChanged[partition] || forceRefresh) {
          dsc.fireChanged[partition] = false; // Resets the fire status flag
          if (dsc.fire[partition]) fireStatusChangeCallback(partition + 1, true); // Fire alarm tripped
          else fireStatusChangeCallback(partition + 1, false); // Fire alarm restored
        }
      }

      // Publishes zones 1-64 status in a separate topic per zone
      // Zone status is stored in the openZones[] and openZonesChanged[] arrays using 1 bit per zone, up to 64 zones:
      //   openZones[0] and openZonesChanged[0]: Bit 0 = Zone 1 ... Bit 7 = Zone 8
      //   openZones[1] and openZonesChanged[1]: Bit 0 = Zone 9 ... Bit 7 = Zone 16
      //   ...
      //   openZones[7] and openZonesChanged[7]: Bit 0 = Zone 57 ... Bit 7 = Zone 64
      
   
      if (dsc.openZonesStatusChanged) {
        char s1[4];
        for (byte zoneGroup = 0; zoneGroup < dscZones; zoneGroup++) {
          for (byte zoneBit = 0; zoneBit < 8; zoneBit++) {
            if (bitRead(dsc.openZonesChanged[zoneGroup], zoneBit)) { // Checks an individual open zone status flag
              bitWrite(dsc.openZonesChanged[zoneGroup], zoneBit, 0); // Resets the individual open zone status flag
              zone = zoneBit + (zoneGroup * 8);
              if (zone >= maxZones) continue;
              if (bitRead(dsc.openZones[zoneGroup], zoneBit)) {
                zoneStatusChangeCallback(zone+1,true);                  
                zoneStatus[zone].open = true;
              } else {
                zoneStatus[zone].open = false;
                zoneStatusChangeCallback(zone+1,false);                 
              }
            }
          }
        }
       }

      // Zone alarm status is stored in the alarmZones[] and alarmZonesChanged[] arrays using 1 bit per zone, up to 64 zones
      //   alarmZones[0] and alarmZonesChanged[0]: Bit 0 = Zone 1 ... Bit 7 = Zone 8
      //   alarmZones[1] and alarmZonesChanged[1]: Bit 0 = Zone 9 ... Bit 7 = Zone 16
      //   ...
      //   alarmZones[7] and alarmZonesChanged[7]: Bit 0 = Zone 57 ... Bit 7 = Zone 64	
      // ESP_LOGD("test","alarmzonestatuschanged=%d",dsc.alarmZonesStatusChanged);
      if (dsc.alarmZonesStatusChanged ) {
        dsc.alarmZonesStatusChanged = false; // Resets the alarm zones status flag
        for (byte zoneGroup = 0; zoneGroup < dscZones; zoneGroup++) {
          for (byte zoneBit = 0; zoneBit < 8; zoneBit++) {

            if (bitRead(dsc.alarmZonesChanged[zoneGroup], zoneBit)) { // Checks an individual alarm zone status flag
              bitWrite(dsc.alarmZonesChanged[zoneGroup], zoneBit, 0); // Resets the individual alarm zone status flag
              zone = zoneBit + (zoneGroup * 8);

              if (zone >= maxZones ) continue;
              if (bitRead(dsc.alarmZones[zoneGroup], zoneBit)) {
                zoneStatus[zone].alarm = true;
                // } else {
                //  zoneStatus[zone].alarm = false;
              }
            }
          }
        }
      }

      zoneStatusMsg = "";
      char s1[7];
      for (int x = 0; x < maxZones; x++) {

        //if (!zoneStatus[x].enabled) continue;   
        
        if (forceRefresh ) 
            zoneStatusChangeCallback(x+1,zoneStatus[x].open);
            
        if (zoneStatus[x].open) {
          sprintf(s1, "OP:%d", x + 1);
          if (zoneStatusMsg != "") zoneStatusMsg.append(",");
          zoneStatusMsg.append(s1);
        }

        if (zoneStatus[x].alarm) {
          sprintf(s1, "AL:%d", x + 1);
          if (zoneStatusMsg != "") zoneStatusMsg.append(",");
          zoneStatusMsg.append(s1);
        }
        if (zoneStatus[x].bypassed) {
          sprintf(s1, "BY:%d", x + 1);
          if (zoneStatusMsg != "") zoneStatusMsg.append(",");
          zoneStatusMsg.append(s1);
        }

        if (zoneStatus[x].tamper) {
          sprintf(s1, "TA:%d", x + 1);
          if (zoneStatusMsg != "") zoneStatusMsg.append(",");
          zoneStatusMsg.append(s1);
        }

        if (zoneStatus[x].batteryLow) {
          sprintf(s1, "BL:%d", x + 1);
          if (zoneStatusMsg != "") zoneStatusMsg.append(",");
          zoneStatusMsg.append(s1);
        }
      }
      if (zoneStatusMsg == "")
        zoneStatusMsg = "OK";

      if (zoneStatusMsg != previousZoneStatusMsg || forceRefresh)
        zoneMsgStatusCallback(zoneStatusMsg);

      previousZoneStatusMsg = zoneStatusMsg;

      systemMsg = "";
      if (bitRead(system1, 0)) {
        if (systemMsg != "") systemMsg.append(",");
        systemMsg.append("BAT");
        panelStatusChangeCallback(batStatus, true, 0);
      } else
        panelStatusChangeCallback(batStatus, false, 0);
      if (bitRead(system1, 1)) {
        if (systemMsg != "") systemMsg.append(",");
        systemMsg.append("BELL");
      }
      if (bitRead(system1, 2)) {
        if (systemMsg != "") systemMsg.append(",");
        systemMsg.append("SYS");
      }
      if (bitRead(system1, 3)) {
        if (systemMsg != "") systemMsg.append(",");
        systemMsg.append("TAMP");
      }
      if (bitRead(system1, 4)) {
        if (systemMsg != "") systemMsg.append(",");
        systemMsg.append("SUP");
      }
      if (bitRead(system1, 5)) {
        if (systemMsg != "") systemMsg.append(",");
        systemMsg.append("RF");
      }
      if (bitRead(system1, 6)) {
        if (systemMsg != "") systemMsg.append(",");
        systemMsg.append("B4");
      }
      if (bitRead(system1, 7)) {
        if (systemMsg != "") systemMsg.append(",");
        systemMsg.append("A4");
      }

      if (bitRead(system0, 1)) {
        if (systemMsg != "") systemMsg.append(",");
        systemMsg.append("AC");
      }
      if (bitRead(system0, 2)) {
        if (systemMsg != "") systemMsg.append(",");
        systemMsg.append("TEL");
      }
      if (bitRead(system0, 3)) {
        if (systemMsg != "") systemMsg.append(",");
        systemMsg.append("COM");
      }
      if (bitRead(system0, 4)) {
        if (systemMsg != "") systemMsg.append(",");
        systemMsg.append("ZF");
      }
      if (bitRead(system0, 5)) {
        if (systemMsg != "") systemMsg.append(",");
        systemMsg.append("ZT");
      }
      if (bitRead(system0, 6)) {
        if (systemMsg != "") systemMsg.append(",");
        systemMsg.append("DBAT");
      }
      if (bitRead(system0, 7)) {
        if (systemMsg != "") systemMsg.append(",");
        systemMsg.append("TIME");
      }
      if (systemMsg == "") systemMsg = "OK";

      if (previousSystemMsg != systemMsg || forceRefresh)
        troubleMsgStatusCallback(systemMsg);
      previousSystemMsg = systemMsg;

    }

    if (!forceDisconnect && dsc.handleModule()) {
      if (dsc.panelData[0] == 0x41) {
        for (byte zoneByte = 0; zoneByte < 4; zoneByte++) {
          byte zoneBit = 0;
          for (int x = 7; x >= 0; x--) {
            zone = zoneBit + (zoneByte * 8);
            if (zone >= maxZones) continue;
            if (!bitRead(dsc.moduleData[zoneByte + 2], x)) { // Checks an individual zone battery status flag for low
              zoneStatus[zone].batteryLow = true;
            } else if (!bitRead(dsc.moduleData[zoneByte + 6], x)) { // Checks an individual zone battery status flag for restore
              zoneStatus[zone].batteryLow = false;
            }
            zoneBit++;
          }
        }
      }

      if (debug > 1)
        printPacket("Moduledata:", dsc.moduleCmd, dsc.moduleData, 16);

      #ifdef DEBUGCOMMANDS
      if (debug > 2) {
        printTimestamp();
        Serial.print("[MODULE] ");
        Serial.print(dsc.panelData[0], HEX);
        Serial.print(": ");
        dsc.printModuleBinary(); // Optionally prints without spaces: printKeybusBinary(false);
        Serial.print(" ");
        dsc.printModuleMessage(); // Prints the decoded message
        Serial.println();
      }
      #endif

    }
    forceRefresh=false;
    firstrun = false;

  }

  void setStatus(byte partition, bool force = false, bool skip = false) {

    // if (dsc.status[partition] == partitionStatus[partition].lastStatus && line2Status != dsc.status[partition] && beeps == 0 && !force) return;
    if (dsc.status[partition] == partitionStatus[partition].lastStatus && beeps == 0 && !force) return;
    byte * currentSelection = & partitionStatus[partition].currentSelection;

    std::string lcdLine1;
    std::string lcdLine2;

    options = false;
    partitionStatus[partition].digits = 0;
    partitionStatus[partition].hex = false;
    partitionStatus[partition].decimalInput = false;

    ESP_LOGD("info", "status %02X, last status %02X,line2status %02X,selection %02X,partition=%d,skip=%d", dsc.status[partition], partitionStatus[partition].lastStatus, line2Status, * currentSelection, partition + 1, skip);
    switch (dsc.status[partition]) {
    case 0x01:
      lcdLine1 = "Partition ready";
      lcdLine2 = " ";
      break;
    case 0x02:
      lcdLine1 = "Stay         ";
      lcdLine2 = "zones open      ";
      break;
    case 0x03:
      lcdLine1 = "Zones open  <>";
      lcdLine2 = " ";
      break;
    case 0x04:
      lcdLine1 = "Armed:       ";
      lcdLine2 = "Stay            ";
      break;
    case 0x05:
      lcdLine1 = "Armed:       ";
      lcdLine2 = "Away            ";
      break;
    case 0x06:
      lcdLine1 = "Armed: Stay  ";
      lcdLine2 = "No entry delay  ";
      break;
    case 0x07:
      lcdLine1 = "Failed       ";
      lcdLine2 = "to arm          ";
      break;
    case 0x08:
      lcdLine1 = "Exit delay   ";
      lcdLine2 = "in progress     ";
      break;
    case 0x09:
      lcdLine1 = "Arming:      ";
      lcdLine2 = "No entry delay  ";
      break;
    case 0x0B:
      lcdLine1 = "Quick exit   ";
      lcdLine2 = "in progress     ";
      break;
    case 0x0C:
      lcdLine1 = "Entry delay  ";
      lcdLine2 = "in progress     ";
      break;
    case 0x0D:
      lcdLine1 = "Entry delay  ";
      lcdLine2 = "after alarm     ";
      break;
    case 0x0E:
      lcdLine1 = "Not          ";
      lcdLine2 = "available       ";
      break;
    case 0x10:
      lcdLine1 = "Keypad       ";
      lcdLine2 = "lockout         ";
      break;
    case 0x11:
      lcdLine1 = "Partition in alarm";
      lcdLine2 = "  ";
      break;
    case 0x12:
      lcdLine1 = "Battery check";
      lcdLine2 = "in progress     ";
      break;
    case 0x14:
      lcdLine1 = "Auto-arm     ";
      lcdLine2 = "in progress     ";
      break;
    case 0x15:
      lcdLine1 = "Arming with  ";
      lcdLine2 = "bypass zones    ";
      break;
    case 0x16:
      lcdLine1 = "Armed: Away  ";
      lcdLine2 = "No entry delay  ";
      break;
    case 0x17:
      lcdLine1 = "Power saving ";
      lcdLine2 = "Keypad blanked  ";
      break;
    case 0x19:
      lcdLine1 = "Alarm        ";
      lcdLine2 = "occurred        ";
      break;
    case 0x22:
      lcdLine1 = "Alarms occurred";
      lcdLine2 = "Press # to exit";
      break;
    case 0x2F:
      lcdLine1 = "Keypad LCD   ";
      lcdLine2 = "test            ";
      break;
    case 0x33:
      lcdLine1 = "Command      ";
      lcdLine2 = "output active   ";
      break;
    case 0x3D:
      lcdLine1 = "Alarm        ";
      lcdLine2 = "occurred        ";
      break;
    case 0x3E:
      lcdLine1 = "Disarmed     ";
      lcdLine2 = " ";
      break;
    case 0x40:
      lcdLine1 = "Keypad       ";
      lcdLine2 = "blanked         ";
      break;
    case 0x8A:
      lcdLine1 = "Activate     ";
      lcdLine2 = "stay/away zones ";
      break;
    case 0x8B:
      lcdLine1 = "Quick exit   ";
      lcdLine2 = " ";
      break;
    case 0x8E:
      lcdLine1 = "Invalid      ";
      lcdLine2 = "option          ";
      break;
    case 0x8F:
      lcdLine1 = "Invalid      ";
      lcdLine2 = "access code     ";
      break;
    case 0x9E:
      lcdLine1 = "Press (*) for <>";
      lcdLine2 = " ";
      break;
    case 0x9F:
      lcdLine1 = "Enter  ";
      lcdLine2 = "access code   ";
      break;
    case 0xA0:
      lcdLine1 = "Zone bypass   <>";
      lcdLine2 = " ";
      break;
    case 0xA1:
      lcdLine1 = "Trouble menu     <>    ";
      lcdLine2 = " ";
      break;
    case 0xA2:
      lcdLine1 = "Alarm memory     <> ";
      lcdLine2 = " ";
      break;
    case 0xA3:
      lcdLine1 = "Door         ";
      lcdLine2 = "chime enabled   ";
      break;
    case 0xA4:
      lcdLine1 = "Door         ";
      lcdLine2 = "chime disabled  ";
      break;
    case 0xA5:
      lcdLine1 = "Enter        ";
      lcdLine2 = "master code     ";
      break;
    case 0xA6:
      lcdLine1 = "*5:  Access Code";
      lcdLine2 = "code? (2 digits)";
      //digits = 2;
      break;
    case 0xA7:
      lcdLine1 = "*5 Enter new ";
      lcdLine2 = "4-digit code    ";
      partitionStatus[partition].digits = 4;
      break;
    case 0xA9:
      lcdLine1 = "*6: User functions";
      lcdLine2 = "function?  ";
      break;
    case 0xAA:
      lcdLine1 = " HHMM    MMDDYY   ";
      lcdLine2 = "";
      partitionStatus[partition].digits = 16;
      break;
    case 0xAB:
      lcdLine1 = "*6:          ";
      lcdLine2 = "Auto-arm time   ";
      break;
    case 0xAC:
      lcdLine1 = "*6:          ";
      lcdLine2 = "Auto-arm on     ";
      break;
    case 0xAD:
      lcdLine1 = "*6:          ";
      lcdLine2 = "Auto-arm off    ";
      break;
    case 0xAF:
      lcdLine1 = "*6:          ";
      lcdLine2 = "System test     ";
      break;
    case 0xB0:
      lcdLine1 = "*6:          ";
      lcdLine2 = "Enable DLS      ";
      break;
    case 0xB2:
      lcdLine1 = "*7:          ";
      lcdLine2 = "Command output  ";
      break;
    case 0xB7:
      lcdLine1 = "Enter        ";

      lcdLine2 = "installer code  ";
      break;
    case 0xB8:
      lcdLine1 = "Enter *      ";
      lcdLine2 = "function code   ";

      break;
    case 0xB9:
      lcdLine1 = "Zone Tamper <>";
      lcdLine2 = " ";
      break;
    case 0xBA:
      lcdLine1 = "Zones low battery <>";
      lcdLine2 = " ";
      break;
    case 0xBC:
      lcdLine1 = "*5 Enter new ";
      lcdLine2 = "6-digit code    ";
      partitionStatus[partition].digits = 6;
      break;
    case 0xC6:
      lcdLine1 = " Zone faults  <>";
      lcdLine2 = " ";
      break;
    case 0xC7:
      lcdLine1 = "Partition    ";
      lcdLine2 = "disabled        ";
      break;
    case 0xC8:
      lcdLine1 = "Service req.  <>";
      lcdLine2 = " ";
      break;
    case 0xCE:
      lcdLine1 = "Active camera";
      lcdLine2 = "monitor select. ";
      break;
    case 0xD0:
      lcdLine1 = "*2: Keypads  ";
      lcdLine2 = "low battery     ";
      break;
    case 0xD1:
      lcdLine1 = "*2: Keyfobs  ";
      lcdLine2 = "low battery     ";
      break;
    case 0xD4:
      lcdLine1 = "*2: Sensors  ";
      lcdLine2 = "RF Delinquency  ";
      break;
    case 0xE4:
      lcdLine1 = "*8: Installer menu";
      lcdLine2 = "Section? ";
      partitionStatus[partition].decimalInput = false;
      break;
    case 0xE5:
      lcdLine1 = "Keypad       ";
      lcdLine2 = "slot assignment ";
      break;
    case 0xE6:
      lcdLine1 = "Input (2 digits)";
      lcdLine2 = " ";
      partitionStatus[partition].digits = 2;
      break;
    case 0xE7:
      lcdLine1 = "Input:       ";
      partitionStatus[partition].digits = 3;
      lcdLine2 = "3 digits    ";
      partitionStatus[partition].decimalInput = true;
      break;
    case 0xE8:
      lcdLine1 = "Input:       ";
      partitionStatus[partition].digits = 4;
      lcdLine2 = "4 digits        ";
      break;
    case 0xE9:
      lcdLine1 = "Input:       ";
      partitionStatus[partition].digits = 5;
      lcdLine2 = "5 digits    ";
      break;
    case 0xEA:
      lcdLine1 = "Input HEX:   ";
      partitionStatus[partition].digits = 2;
      partitionStatus[partition].hex = true;
      lcdLine2 = "2 digits    ";
      //setlcdrec 2
      break;
    case 0xEB:
      lcdLine1 = "Input hex(4dig)";
      partitionStatus[partition].digits = 4;
      partitionStatus[partition].hex = true;
      lcdLine2 = " ";
      //setlcdrec 4
      break;
    case 0xEC:
      lcdLine1 = "Input hex(6dig)";
      partitionStatus[partition].digits = 6;
      partitionStatus[partition].hex = true;
      lcdLine2 = " ";
      //setlcdreceive 6
      break;
    case 0xED:
      lcdLine1 = "Input HEX:   ";
      partitionStatus[partition].digits = 32;
      partitionStatus[partition].hex = true;
      lcdLine2 = "32 digits  ";
      //setlcdreceive 32

      break;
    case 0xEE:
      lcdLine1 = "Input: options     ";
      options = true;
      lcdLine2 = "option per zone ";
      break;
    case 0xEF:
      lcdLine1 = "Module       ";
      lcdLine2 = "supervision   ";
      break;
    case 0xF0:
      lcdLine1 = "Function     ";
      lcdLine2 = "key 1      ";
      break;
    case 0xF1:
      lcdLine1 = "Function     ";
      lcdLine2 = "key 2        ";
      break;
    case 0xF2:
      lcdLine1 = "Function     ";
      lcdLine2 = "key 3       ";
      break;
    case 0xF3:
      lcdLine1 = "Function     ";
      lcdLine2 = "key 4      ";
      break;
    case 0xF4:
      lcdLine1 = "Function     ";
      lcdLine2 = "key 5           ";
      break;
    case 0xF5:
      lcdLine1 = "Wireless mod.";
      lcdLine2 = "placement test  ";
      break;
    case 0xF6:
      lcdLine1 = "Activate     ";
      lcdLine2 = "device for test ";
      break;
    case 0xF7:
      lcdLine1 = "*8: Installer";
      partitionStatus[partition].decimalInput = false;
      partitionStatus[partition].digits = 0;
      lcdLine2 = "menu, 2 digits  ";
      break;
    case 0xF8:
      lcdLine1 = "Keypad    ";
      lcdLine2 = "programming";
      break;
    case 0xFA:
      lcdLine1 = "Input:   ";
      partitionStatus[partition].digits = 6;
      lcdLine2 = "6 digits ";
      break;
    default:
      lcdLine2 = dsc.status[partition];
      partitionStatus[partition].digits = 0;
    }

    if (dsc.status[partition] != 0xA9) partitionStatus[partition].eventViewer = false;

    if (partitionStatus[partition].digits == 0) partitionStatus[partition].newData = false;

    if (millis() - partitionStatus[partition].keyPressTime > 3000 && dsc.status[partition] > 0x8B) {
      if (!partitionStatus[partition].inprogram) {
        partitionStatus[partition].locked = true;
        partitionStatus[partition].lastStatus = dsc.status[partition];
        return;
      } else
        partitionStatus[partition].locked = false;

    } else if (dsc.status[partition] > 0x8B && !partitionStatus[partition].locked) {
      partitionStatus[partition].inprogram = true;
    }
    if (dsc.status[partition] < 0x8B) {
      partitionStatus[partition].locked = false;
      partitionStatus[partition].inprogram = false;
      activePartition = 1;
    }
    


    if (!skip) {

      // ESP_LOGD("test", "digits = %d,status=%02X,previoustatus=%02X,newdata=%d,locked=%d,partition=%d,selection=%d", partitionStatus[partition].digits, dsc.status[partition], partitionStatus[partition].lastStatus, partitionStatus[partition].newData, partitionStatus[partition].locked, partition + 1, *currentSelection);

      //if multi digit field, setup for 6E request to panel
      if (dsc.status[partition] != partitionStatus[partition].lastStatus && !partitionStatus[partition].locked && partitionStatus[partition].digits && !partitionStatus[partition].newData) {

        // ESP_LOGD("test", "in setlcd: digits = %d,status=%02X,previoustatus=%02X,newdata=%d,locked=%d", partitionStatus[partition].digits, dsc.status[partition], partitionStatus[partition].lastStatus, partitionStatus[partition].newData, partitionStatus[partition].locked);

        dsc.setLCDReceive(partitionStatus[partition].digits, partition + 1);
        partitionStatus[partition].editIdx = 0;
        partitionStatus[partition].hexMode = false;
        partitionStatus[partition].newData = true;
        lcdLine1 = "";
        lcdLine2 = "";

        //ok, we should have the data now so display it
      } else if (partitionStatus[partition].digits && partitionStatus[partition].newData && dsc.pgmBuffer.dataPending) {

        char s[8];
        if (partitionStatus[partition].digits > 16)
          lcdLine1 = " ";
        lcdLine2 = " ";
        int y;
        char c;
        if (partitionStatus[partition].hexMode) lcdLine1 = "*";

        if (partitionStatus[partition].decimalInput) {
          if (partitionStatus[partition].digits == 2)
            sprintf(decimalInputBuffer, "%2d", dsc.pgmBuffer.data[0]);
          else
            sprintf(decimalInputBuffer, "%03d", dsc.pgmBuffer.data[0]);
        }
   
        for (int x = 0; x < partitionStatus[partition].digits ; x++) { 
          y = (x / 2)  + (x/8); //skip every 5th byte since it's a checksum
          if (partitionStatus[partition].decimalInput)
            c = decimalInputBuffer[x] - 48;
          else
            c = x % 2 ? dsc.pgmBuffer.data[y] & 0x0F : (dsc.pgmBuffer.data[y] & 0xF0) >> 4;
          std::string tpl;
          tpl = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";

          if (dsc.status[partition] == 0xAA) {
            tpl = "XXXX    XXXXXX  XXXXXXXXXXXXXXXX";
          }
          if (tpl[x] == 'X') {
            if (x == partitionStatus[partition].editIdx)
              sprintf(s, "[%1X]", c);
            else
              sprintf(s, "%1X", c);
          } else
            sprintf(s, " ");

          if (partitionStatus[partition].digits < 17)
            lcdLine2.append(s);
          else {
            if (x < 16)
              lcdLine1.append(s);
            else
              lcdLine2.append(s);
          }
        }

      } else if (partitionStatus[partition].digits) {
        lcdLine1 = "";
        lcdLine2 = "";
      }

      if (dsc.status[partition] < 0x04) {
        if ( * currentSelection > 1 && * currentSelection < 6) {
          std::string s = String(FPSTR(statusMenu[ * currentSelection])).c_str();
          int pos = s.find(":");
          lcdLine1 = (s.substr(0, pos));
          lcdLine2 = (s.substr(pos + 1));
        } else {
          byte c = dsc.ready[partition] ? 0 : 1;
          std::string s = String(FPSTR(statusMenuLabels[c])).c_str();
          int pos = s.find(":");
          lcdLine1 = (s.substr(0, pos));
          lcdLine2 = (s.substr(pos + 1));
          * currentSelection = 1;
        }

        if (partitionStatus[partition].selectedZone && partitionStatus[partition].selectedZone < maxZones) {
          char s[16];
          sprintf(s, "Zone %02d  <>", partitionStatus[partition].selectedZone);
          lcdLine2 = s;
        }

      } else if (dsc.status[partition] == 0xA0) { //bypass
        if ( * currentSelection == 0xFF || * currentSelection == 0 || dsc.status[partition] != partitionStatus[partition].lastStatus)
          *
          currentSelection = getNextEnabledZone(0xFF, partition + 1);
        if ( * currentSelection < maxZones && * currentSelection > 0) {
          char s[16];
          char bypassStatus = ' ';
          if (zoneStatus[ * currentSelection - 1].bypassed)
            bypassStatus = 'B';
          else if (zoneStatus[ * currentSelection - 1].open)
            bypassStatus = 'O';
          sprintf(s, "%02d   %c", * currentSelection, bypassStatus);
          lcdLine2 = s;
        }
      } else if (dsc.status[partition] == 0x11) { //alarms

        if ( * currentSelection == 0xFF || * currentSelection == 0 || dsc.status[partition] != partitionStatus[partition].lastStatus)
          *
          currentSelection = getNextAlarmedZone(0xFF, partition + 1);
        if ( * currentSelection < maxZones && * currentSelection > 0) {
          char s[16];
          sprintf(s, "zone %02d", * currentSelection);
          lcdLine2 = s;
        } else lcdLine2 = " ";
      } else if (dsc.status[partition] == 0xA2) { //alarm memory

        if ( * currentSelection == 0xFF || dsc.status[partition] != partitionStatus[partition].lastStatus)
          *
          currentSelection = getNextOption(0xFF);

        if ( * currentSelection < maxZones && * currentSelection > 0) {
          char s[16];
          lcdLine2 = "";
          char bypassStatus = ' ';

          sprintf(s, "zone %02d", * currentSelection);
          lcdLine2 = s;
        } else {
          lcdLine1 = "No alarms";
          lcdLine2 = "in memory";
        }
      } else if (dsc.status[partition] == 0x9E) { // main menu

        if ( * currentSelection == 0xFF || dsc.status[partition] != partitionStatus[partition].lastStatus) {
          * currentSelection = 1;
        }
        lcdLine2 = String(FPSTR(mainMenu[ * currentSelection])).c_str();

      } else if (dsc.status[partition] == 0xB2) { // output menu

        if ( * currentSelection == 0xFF || dsc.status[partition] != partitionStatus[partition].lastStatus) {
          * currentSelection = 1;
        }
        lcdLine2 = String(FPSTR(outputMenu[ * currentSelection])).c_str();

      } else if (dsc.status[partition] == 0xA9 && !partitionStatus[partition].eventViewer) { // user menu

        if ( * currentSelection == 0xFF || dsc.status[partition] != partitionStatus[partition].lastStatus) {
          * currentSelection = 1;
        }
        lcdLine2 = String(FPSTR(userMenu[ * currentSelection])).c_str();

      } else if (dsc.status[partition] == 0xA1) { //trouble

        if ( * currentSelection == 0xFF) {
          * currentSelection = getNextOption(0xFF);
        }
        if ( * currentSelection < 9) {
          lcdLine2 = String(FPSTR(troubleMenu[ * currentSelection])).c_str();

        }

      } else if (dsc.status[partition] == 0xC8) { //service

        if ( * currentSelection == 0xFF)
          *
          currentSelection = getNextOption(0xFF);
        if ( * currentSelection < 9) {
          lcdLine2 = String(FPSTR(serviceMenu[ * currentSelection])).c_str();

        }

      } else if (dsc.status[partition] == 0xA6) { //user code
        if ( * currentSelection == 0xFF)
          *
          currentSelection = getNextUserCode(0xFF);
        if ( * currentSelection < 96) {
          char s[16];
          char programmed = ' ';
          if (checkUserCode( * currentSelection))
            programmed = 'P';
          sprintf(s, "%02d   %c", * currentSelection, programmed);
          lcdLine2 = s;
        }
      } else                 // Sends the access code when needed by the panel for arming
        if (dsc.status[partition] == 0x9F && dsc.accessCodePrompt && isInt(accessCode, 10)) {
        dsc.accessCodePrompt = false;
        dsc.write(accessCode, partition + 1);
        if (debug > 0) ESP_LOGD("Debug", "got access code prompt for partition %d", partition + 1);
      }  

      if (options) {
        lcdLine2 = getOptionsString();
      }
      
    

    }
    

    if (lcdLine1 != "") line1DisplayCallback(lcdLine1, partition + 1);
    if (lcdLine2 != "") line2DisplayCallback(lcdLine2, partition + 1);

    partitionStatus[partition].lastStatus = dsc.status[partition];
    
  }

  // Processes status data not natively handled within the library
  void processStatus() {
    #ifndef dscClassicSeries
    byte partition = 0;
    switch (dsc.panelData[0]) {
    case 0x0F:
    case 0x0A:
      processProgramZones(4);
      if (dsc.panelData[3] == 0xBA)
        processLowBatteryZones();
      if (dsc.panelData[3] == 0xA1) { //system status
        system0 = dsc.panelData[4];
      }
      if (dsc.panelData[3] == 0xC8) { //service required menu
        system1 = dsc.panelData[4];
      }

      break;
    case 0x5D:
    case 0x63:

      if ((dsc.panelData[2] & 0x04) == 0x04) { // Alarm memory zones 1-32
        processProgramZones(3);
      }
      break;

    case 0xA5:

      processEventBufferAA(true);
      break;
    case 0xAA:

      processEventBufferAA();
      break;
    case 0x6E:
      printPanel_0x6E();
      break;
    case 0x69:
    case 0x64:
      printBeeps(2);
      break;
    case 0x75: //tones 1
    case 0x7D:
      //ESP_LOGD("info", "Sent tones cmd %02X,%02X", dsc.panelData[0], dsc.panelData[3]);
      break; //tones 2
    case 0x87: //relay cmd
      processRelayCmd();
      break;
    case 0xB1:
      getEnabledZonesB1(2, 1, 1);
      getEnabledZonesB1(6, 1, 2);
      break;
    case 0xE6:

      switch (dsc.panelData[2]) {
      case 0x01:
      case 0x02:
      case 0x03:
      case 0x04:
      case 0x05:
      case 0x06:
      case 0x1D: //ESP_LOGD("info", "Sent tones cmd %02X,%02X", dsc.panelData[0], dsc.panelData[4]);
        break; //tones 3-8
      case 0x19:
        printBeeps(4);
        break;
      case 0x1A:
        break;
      case 0x20:
      case 0x21:
        processProgramZones(5);
        break; // Programming zone lights 33-64 //bypass?
      case 0x18:
        //ESP_LOGD("info", "zone lights 33");
        if ((dsc.panelData[4] & 0x04) == 0x04)
          processProgramZones(5);
        break; // Alarm memory zones 33-64
      case 0x2B:
        getEnabledZonesE6(4, 1, dsc.panelData[3]);
        break;
      case 0x2C:
        getEnabledZonesE6(4, 33, dsc.panelData[3]);
        break;
      };

      break;
    case 0xEB:

      if (dsc.panelData[7] == 1)
        switch (dsc.panelData[8]) {
        case 0xAE:
          line2DisplayCallback("Walk test end", activePartition);
          break;
        case 0xAF:
          line2DisplayCallback("Walk test beging", activePartition);
          break;
        };
      processEventBufferEC(true);
      break;
    case 0xEC:
      processEventBufferEC();
      break;
    }
    #endif
  }

  void printPanelTone(byte panelByte) {

    if (dsc.panelData[panelByte] == 0) {
      //stream->print(F("none"));

      return;
    }

    if ((dsc.panelData[panelByte] & 0x80) == 0x80) {
      //stream->print(F("constant tone "));

    }

    if ((dsc.panelData[panelByte] & 0x70) != 0) {
      // stream->print((panelData[panelByte] & 0x70) >> 4);
      //stream->print(F(" beep "));
    }

    if ((dsc.panelData[panelByte] & 0x0F) != 0) {
      //  stream->print(panelData[panelByte] & 0x0F);
      //  stream->print(F("s interval"));
    }
  }

  void printBeeps(byte panelByte) {
    dsc.statusChanged = true;
    beeps = dsc.panelData[panelByte] / 2;
    char s[4];
    sprintf(s, "%d", beeps);
    for (byte partition = 0; partition < dscPartitions; partition++) {
      if (dsc.disabled[partition] || partitionStatus[partition].locked) continue;
      beepsCallback(s, partition + 1);
      if (beeps == 2 && partitionStatus[partition].digits) {
        dsc.setLCDReceive(partitionStatus[partition].digits, partition);
        partitionStatus[partition].editIdx = 0;
        partitionStatus[partition].hexMode = false;
        partitionStatus[partition].newData = true;
      }
    }
    beepTime = millis();
  }

  void printPanel_0x6E() {
    char s1[5];
    group1msg = "";
    if (partitionStatus[dsc.pgmBuffer.partition - 1].decimalInput) {
      if (dsc.panelData[2] <= 0x63) group1msg.append("0");
      if (dsc.panelData[2] <= 0x09) group1msg.append("0");
      sprintf(s1, "%02d", dsc.panelData[2]);
      group1msg.append(s1);
    } else {
      for (byte panelByte = 2; panelByte <= 5; panelByte++) {
        sprintf(s1, "%02x", dsc.panelData[panelByte]);
        group1msg.append(s1);
      }

    }
    if (dsc.pgmBuffer.partition) {
      if (dsc.pgmBuffer.idx == dsc.pgmBuffer.len) setStatus(dsc.pgmBuffer.partition - 1, true);
      //if (group1msg != "") lightsCallback(group1msg, dsc.pgmBuffer.partition - 1);
    }
  }

  void processLowBatteryZones() {
    // if (dsc.panelData[0] == 0x0A && dsc.panelData[3] == 0xBA) { //low battery zones
    for (byte panelByte = 4; panelByte < 8; panelByte++) {
      for (byte zoneBit = 0; zoneBit < 8; zoneBit++) {
        zone = zoneBit + ((panelByte - 4) * 8);
        if (zone >= maxZones) continue;
        if (bitRead(dsc.panelData[panelByte], zoneBit)) {
          zoneStatus[zone].batteryLow = true;
        } else
          zoneStatus[zone].batteryLow = false;
      }
    }
    // }
  }

  void processRelayCmd() {
    byte rchan;
    byte pgm = 2;
    for (byte relayByte = 2; relayByte < 4; relayByte++) {
      for (byte x = 0; x < 8; x++) {

        if (relayByte == 3) {
          if (x < 2)
            pgm = 0;
          else if (x == 2 || x == 3)
            continue;
          else if (x > 3)
            pgm = 6;

        }
        rchan = pgm + x;
        if (bitRead(dsc.panelData[relayByte], x)) {
          relayStatus[rchan] = true;
        } else {
          relayStatus[rchan] = false;
        }
        if (previousRelayStatus[rchan] != relayStatus[rchan])
          relayChannelChangeCallback(rchan + 1, relayStatus[rchan]);
        previousRelayStatus[rchan] = relayStatus[rchan];
      }
    }

  }

  void processProgramZones(byte startByte) {
    byte byteCount = 0;
    byte zoneStart = 0;
    byte zone;
    if (startByte == 5) zoneStart = 4;
    for (byte zoneGroup = zoneStart; zoneGroup < zoneStart + 4; zoneGroup++) {
      programZones[zoneGroup] = dsc.panelData[startByte + byteCount];
      byteCount++;
    }

    byteCount = 0;
    char s1[5];
    if (startByte == 4 || startByte == 3) group1msg = "";
    if (startByte == 5) group2msg = "";
    for (byte zoneGroup = zoneStart; zoneGroup < zoneStart + 4; zoneGroup++) {
      for (byte zoneBit = 0; zoneBit < 8; zoneBit++) {
        zone = (zoneBit + 1) + (zoneGroup * 8);
        if (bitRead(dsc.panelData[startByte + byteCount], zoneBit)) {
          sprintf(s1, "%02d ", zone);
          if (startByte == 4 || startByte == 3) group1msg.append(s1);
          if (startByte == 5) group2msg.append(s1);

        }
      }
      byteCount++;
    }
    group1msg.append(group2msg);
    //lightsCallback(group1msg, defaultPartition);
    if (options)
      dsc.statusChanged = true;
  }

  void processEventBufferAA(bool showEvent = false) {
    #ifndef dscClassicSeries
    if (extendedBufferFlag) return; // Skips 0xAA data when 0xEC extended event buffer data is available

    char eventInfo[45] = "";
    char charBuffer[4];
    if (!showEvent) {
      strcat(eventInfo, "E:");
      itoa(dsc.panelData[7], charBuffer, 10);
      if (dsc.panelData[7] < 10) strcat(eventInfo, "00");
      else if (dsc.panelData[7] < 100) strcat(eventInfo, "0");
      strcat(eventInfo, charBuffer);
      strcat(eventInfo, " ");
    }
    byte dscYear3 = dsc.panelData[2] >> 4;
    byte dscYear4 = dsc.panelData[2] & 0x0F;
    byte dscMonth = dsc.panelData[2 + 1] << 2;
    dscMonth >>= 4;
    byte dscDay1 = dsc.panelData[2 + 1] << 6;
    dscDay1 >>= 3;
    byte dscDay2 = dsc.panelData[2 + 2] >> 5;
    byte dscDay = dscDay1 | dscDay2;
    byte dscHour = dsc.panelData[2 + 2] & 0x1F;
    byte dscMinute = dsc.panelData[2 + 3] >> 2;

    if (dscYear3 >= 7) strcat(eventInfo, "19");
    else strcat(eventInfo, "20");
    itoa(dscYear3, charBuffer, 10);
    strcat(eventInfo, charBuffer);
    itoa(dscYear4, charBuffer, 10);
    strcat(eventInfo, charBuffer);
    strcat(eventInfo, ".");
    if (dscMonth < 10) strcat(eventInfo, "0");
    itoa(dscMonth, charBuffer, 10);
    strcat(eventInfo, charBuffer);
    strcat(eventInfo, ".");
    if (dscDay < 10) strcat(eventInfo, "0");
    itoa(dscDay, charBuffer, 10);
    strcat(eventInfo, charBuffer);
    strcat(eventInfo, " ");
    if (dscHour < 10) strcat(eventInfo, "0");
    itoa(dscHour, charBuffer, 10);
    strcat(eventInfo, charBuffer);
    strcat(eventInfo, ":");
    if (dscMinute < 10) strcat(eventInfo, "0");
    itoa(dscMinute, charBuffer, 10);
    strcat(eventInfo, charBuffer);
    
     if (dsc.panelData[6] == 0 && dsc.panelData[7] == 0) {
      //timestamp
      return;
    }   

    byte partition = dsc.panelData[3] >> 6;
    strcat(eventInfo, " P:");
    itoa(partition, charBuffer, 10);
    strcat(eventInfo, charBuffer);
    strcat(eventInfo, " ");

    if (showEvent)
      eventStatusMsg = eventInfo;
    else
      line1DisplayCallback(eventInfo, activePartition);

    switch (dsc.panelData[5] & 0x03) {
    case 0x00:
      printPanelStatus0(6, activePartition, showEvent);
      break;
    case 0x01:
      printPanelStatus1(6, activePartition, showEvent);
      break;
    case 0x02:
      printPanelStatus2(6, activePartition, showEvent);
      break;
    case 0x03:
      printPanelStatus3(6, activePartition, showEvent);
      break;
    }
    if (showEvent && eventStatusMsg != "") {
      eventInfoCallback(eventStatusMsg);
      eventTime = millis();
    }
    #endif
  }

  void processEventBufferEC(bool showEvent = false) {
    #ifndef dscClassicSeries
    if (!extendedBufferFlag) extendedBufferFlag = true;

    char eventInfo[45] = "";
    char charBuffer[4];
    if (!showEvent) {
      strcat(eventInfo, "E:");
      int eventNumber = dsc.panelData[9] + ((dsc.panelData[4] >> 6) * 256);
      itoa(eventNumber, charBuffer, 10);
      if (eventNumber < 10) strcat(eventInfo, "00");
      else if (eventNumber < 100) strcat(eventInfo, "0");
      strcat(eventInfo, charBuffer);
      strcat(eventInfo, " ");
    }
    
    byte dscYear3 = dsc.panelData[3] >> 4;
    byte dscYear4 = dsc.panelData[3] & 0x0F;
    byte dscMonth = dsc.panelData[4] << 2;
    dscMonth >>= 4;
    byte dscDay1 = dsc.panelData[4] << 6;
    dscDay1 >>= 3;
    byte dscDay2 = dsc.panelData[5] >> 5;
    byte dscDay = dscDay1 | dscDay2;
    byte dscHour = dsc.panelData[5] & 0x1F;
    byte dscMinute = dsc.panelData[6] >> 2;

    if (dscYear3 >= 7) strcat(eventInfo, "19");
    else strcat(eventInfo, "20");
    itoa(dscYear3, charBuffer, 10);
    strcat(eventInfo, charBuffer);
    itoa(dscYear4, charBuffer, 10);
    strcat(eventInfo, charBuffer);
    strcat(eventInfo, ".");
    if (dscMonth < 10) strcat(eventInfo, "0");
    itoa(dscMonth, charBuffer, 10);
    strcat(eventInfo, charBuffer);
    strcat(eventInfo, ".");
    if (dscDay < 10) strcat(eventInfo, "0");
    itoa(dscDay, charBuffer, 10);
    strcat(eventInfo, charBuffer);
    strcat(eventInfo, " ");
    if (dscHour < 10) strcat(eventInfo, "0");
    itoa(dscHour, charBuffer, 10);
    strcat(eventInfo, charBuffer);
    strcat(eventInfo, ":");
    if (dscMinute < 10) strcat(eventInfo, "0");
    itoa(dscMinute, charBuffer, 10);
    strcat(eventInfo, charBuffer);

    if (dsc.panelData[2] != 0) {
      strcat(eventInfo, " P:");

      byte bitCount = 0;
      for (byte bit = 0; bit <= 7; bit++) {
        if (bitRead(dsc.panelData[2], bit)) {
          itoa((bitCount + 1), charBuffer, 10);
        }
        bitCount++;
      }
      strcat(eventInfo, charBuffer);
    }
    strcat(eventInfo, " ");

    if (showEvent)
      eventStatusMsg = eventInfo;
    else
      line1DisplayCallback(eventInfo, activePartition);

    switch (dsc.panelData[7]) {
    case 0x00:
      printPanelStatus0(8, activePartition, showEvent);
      break;
    case 0x01:
      printPanelStatus1(8, activePartition, showEvent);
      break;
    case 0x02:
      printPanelStatus2(8, activePartition, showEvent);
      break;
    case 0x03:
      printPanelStatus3(8, activePartition, showEvent);
      break;
    case 0x04:
      printPanelStatus4(8, activePartition, showEvent);
      break;
    case 0x05:
      printPanelStatus5(8, activePartition, showEvent);
      break;
    case 0x14:
      printPanelStatus14(8, activePartition, showEvent);
      break;
    case 0x16:
      printPanelStatus16(8, activePartition, showEvent);
      break;
    case 0x17:
      printPanelStatus17(8, activePartition, showEvent);
      break;
    case 0x18:
      printPanelStatus18(8, activePartition, showEvent);
      break;
    case 0x1B:
      printPanelStatus1B(8, activePartition, showEvent);
      break;
    }
    if (showEvent && eventStatusMsg != "") {
      eventInfoCallback(eventStatusMsg);
      eventTime = millis();
    }
    #endif
  }

  void printPanelStatus0(byte panelByte, byte partition, bool showEvent = false) {
    bool decoded = true;

    std::string lcdLine1;
    std::string lcdLine2;
    switch (dsc.panelData[panelByte]) {
    case 0x49:
      lcdLine1 = "Duress alarm";
      lcdLine2 = " ";
      break;
    case 0x4A:
      lcdLine1 = "Disarmed:";
      lcdLine2 = "Alarm mem";
      break;
    case 0x4B:
      lcdLine1 = "Recent";
      lcdLine2 = "closing alarm";
      break;
    case 0x4C:
      lcdLine1 = "Zone exp";
      lcdLine2 = "suprvis. alarm";
      break;
    case 0x4D:
      lcdLine1 = "Zone exp";
      lcdLine2 = "suprvis. rest";
      break;
    case 0x4E:
      lcdLine1 = "Keypad Fire";
      lcdLine2 = "alarm";
      break;
    case 0x4F:
      lcdLine1 = "Keypad Aux";
      lcdLine2 = "alarm";
      break;
    case 0x50:
      lcdLine1 = "Keypad Panic";
      lcdLine2 = "alarm";
      break;
    case 0x51:
      lcdLine1 = "Aux input";
      lcdLine2 = "alarm";
      break;
    case 0x52:
      lcdLine1 = "Keypad Fire";
      lcdLine2 = "alarm rest";
      break;
    case 0x53:
      lcdLine1 = "Keypad Aux";
      lcdLine2 = "alarm rest";
      break;
    case 0x54:
      lcdLine1 = "Keypad Panic";
      lcdLine2 = "alarm rest";
      break;
    case 0x55:
      lcdLine1 = "Aux input";
      lcdLine2 = "alarm rest";
      break;
      // 0x56 - 0x75: Zone tamper, zones 1-32
      // 0x76 - 0x95: Zone tamper restored, zones 1-32      
    case 0x98:
      lcdLine1 = "Keypad";
      lcdLine2 = "lockout";
      break;
      // 0x99 - 0xBD: Armed: Access codes 1-34, 40-42      
    case 0xBE:
      lcdLine1 = "Armed:";
      lcdLine2 = "Partial";
      break;
    case 0xBF:
      lcdLine1 = "Armed:";
      lcdLine2 = "Special";
      break;
      // 0xC0 - 0xE4: Disarmed: Access codes 1-34, 40-42      
    case 0xE5:
      lcdLine1 = "Auto-arm";
      lcdLine2 = "canc";
      break;
    case 0xE6:
      lcdLine1 = "Disarmed:";
      lcdLine2 = "Special";
      break;
    case 0xE7:
      lcdLine1 = "Panel bat";
      lcdLine2 = "trble";
      break;
    case 0xE8:
      lcdLine1 = "Panel AC";
      lcdLine2 = "trble";
      break;
    case 0xE9:
      lcdLine1 = "Bell trble";
      lcdLine2 = " ";
      break;
    case 0xEA:
      lcdLine1 = "Fire zone";
      lcdLine2 = "trble";
      break;
    case 0xEB:
      lcdLine1 = "Panel aux sup";
      lcdLine2 = "trble";
      break;
    case 0xEC:
      lcdLine1 = "Tel line";
      lcdLine2 = "trble";
      break;
    case 0xEF:
      lcdLine1 = "Panel bat";
      lcdLine2 = "rest";
      break;
    case 0xF0:
      lcdLine1 = "Panel AC";
      lcdLine2 = "rest";
      break;
    case 0xF1:
      lcdLine1 = "Bell rest";
      lcdLine2 = " ";
      break;
    case 0xF2:
      lcdLine1 = "Fire zone";
      lcdLine2 = "trble rest";
      break;
    case 0xF3:
      lcdLine1 = "Panel aux sup";
      lcdLine2 = "rest";
      break;
    case 0xF4:
      lcdLine1 = "Tel line";
      lcdLine2 = "rest";
      break;
    case 0xF7:
      lcdLine1 = "Phone 1 FTC";
      lcdLine2 = " ";
      break;
    case 0xF8:
      lcdLine1 = "Phone 2 FTC";
      lcdLine2 = " ";
      break;
    case 0xF9:
      lcdLine1 = "Event buffer";
      lcdLine2 = "threshold";
      break;
    case 0xFA:
      lcdLine1 = "DLS lead-in";
      lcdLine2 = " ";
      break;
    case 0xFB:
      lcdLine1 = "DLS lead-out";
      lcdLine2 = " ";
      break;
    case 0xFE:
      lcdLine1 = "Periodic test";
      lcdLine2 = "trans";
      break;
    case 0xFF:
      lcdLine1 = "System test";
      lcdLine2 = " ";
      break;
    default:
      decoded = false;
    }

    char lcdMessage[20];
    char charBuffer[4];

    if (dsc.panelData[panelByte] >= 0x09 && dsc.panelData[panelByte] <= 0x28) {
      strcpy(lcdMessage, "Zone alarm:");
      byte zone = dsc.panelData[panelByte] - 8;
      if (zone > 0 && zone < maxZones)
        zoneStatus[zone - 1].alarm = true;
      itoa(dsc.panelData[panelByte] - 8, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine1 = lcdMessage;
      lcdLine2 = " ";
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0x29 && dsc.panelData[panelByte] <= 0x48) {
      lcdLine1 = "Zone alarm";
      strcpy(lcdMessage, " restored:");
      byte zone = dsc.panelData[panelByte] - 40;
      // if (zone > 0 && zone < maxZones) 
      // zoneStatus[zone-1].alarm=false;      
      itoa(dsc.panelData[panelByte] - 40, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0x56 && dsc.panelData[panelByte] <= 0x75) {
      strcpy(lcdMessage, "Zone tamper:");
      byte zone = dsc.panelData[panelByte] - 85;
      if (zone > 0 && zone < maxZones)
        zoneStatus[zone - 1].tamper = true;
      itoa(zone, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine1 = lcdMessage;
      lcdLine2 = " ";
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0x76 && dsc.panelData[panelByte] <= 0x95) {
      lcdLine1 = "Zone tamper";
      strcpy(lcdMessage, " restored: ");
      byte zone = dsc.panelData[panelByte] - 117;
      if (zone > 0 && zone < maxZones)
        zoneStatus[zone - 1].tamper = false;
      itoa(zone, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0x99 && dsc.panelData[panelByte] <= 0xBD) {
      lcdLine1 = "Armed:";
      byte dscCode = dsc.panelData[panelByte] - 0x98;
      if (dscCode >= 35) dscCode += 5;
      if (dscCode == 40) strcpy(lcdMessage, "Master code:");
      else strcpy(lcdMessage, "Access code:");
      itoa(dscCode, charBuffer, 10);
      std::string c=getUserName(charBuffer);
      strcat(lcdMessage, c.c_str());
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0xC0 && dsc.panelData[panelByte] <= 0xE4) {
      lcdLine1 = "Disarmed:";
      byte dscCode = dsc.panelData[panelByte] - 0xBF;
      if (dscCode >= 35) dscCode += 5;
      if (dscCode == 40) strcpy(lcdMessage, "Master code:");
      else strcpy(lcdMessage, "Access code:");
      itoa(dscCode, charBuffer, 10);
      std::string c=getUserName(charBuffer);
      strcat(lcdMessage, c.c_str());
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (!decoded) {
      lcdLine1 = "Unknown data0";
      lcdLine2 = " ";
    }

    if (showEvent)
      eventStatusMsg.append(lcdLine1.append(" ").append(lcdLine2));
    else
      line2DisplayCallback(lcdLine1.append(" ").append(lcdLine2), partition);

  }

  void printPanelStatus1(byte panelByte, byte partition, bool showEvent = false) {
    bool decoded = true;
    std::string lcdLine1;
    std::string lcdLine2;
    switch (dsc.panelData[panelByte]) {
    case 0x03:
      lcdLine1 = "Cross zone";
      lcdLine2 = "alarm";
      break;
    case 0x04:
      lcdLine1 = "Delinquency";
      lcdLine2 = "alarm";
      break;
    case 0x05:
      lcdLine1 = "Late to close";
      lcdLine2 = " ";
      break;
      // 0x24 - 0x28: Access codes 33-34, 40-42      
    case 0x29:
      lcdLine1 = "Download";
      lcdLine2 = "forced ans";
      break;
    case 0x2B:
      lcdLine1 = "Armed:";
      lcdLine2 = "Auto-arm";
      break;
      // 0x2C - 0x4B: Zone battery restored, zones 1-32
      // 0x4C - 0x6B: Zone battery low, zones 1-32
      // 0x6C - 0x8B: Zone fault restored, zones 1-32
      // 0x8C - 0xAB: Zone fault, zones 1-32      
    case 0xAC:
      lcdLine1 = "Exit inst";
      lcdLine2 = "prog";
      break;
    case 0xAD:
      lcdLine1 = "Enter inst";
      lcdLine2 = "prog";
      break;
    case 0xAE:
      lcdLine1 = "Walk test";
      lcdLine2 = "end";
      break;
    case 0xAF:
      lcdLine1 = "Walk test";
      lcdLine2 = "begin";
      break;
      // 0xB0 - 0xCF: Zones bypassed, zones 1-32      
    case 0xD0:
      lcdLine1 = "Command";
      lcdLine2 = "output 4";
      break;
    case 0xD1:
      lcdLine1 = "Exit fault";
      lcdLine2 = "pre-alert";
      break;
    case 0xD2:
      lcdLine1 = "Armed:";
      lcdLine2 = "Entry delay";
      break;
    case 0xD3:
      lcdLine1 = "Downlook rem";
      lcdLine2 = "trig";
      break;
    default:
      decoded = false;
    }

    char lcdMessage[20];
    char charBuffer[4];
    if (dsc.panelData[panelByte] >= 0x24 && dsc.panelData[panelByte] <= 0x28) {
      byte dscCode = dsc.panelData[panelByte] - 0x03;
      if (dscCode >= 35) dscCode += 5;
      if (dscCode == 40) strcpy(lcdMessage, "Master code:");
      else strcpy(lcdMessage, "Access code:");
      itoa(dscCode, charBuffer, 10);
      std::string c=getUserName(charBuffer);
      strcat(lcdMessage, c.c_str());
      lcdLine1 = lcdMessage;
      lcdLine2 = " ";
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0x2C && dsc.panelData[panelByte] <= 0x4B) {
      lcdLine1 = "Zone bat";
      strcpy(lcdMessage, "rest:");
      zoneStatus[dsc.panelData[panelByte] - 42].batteryLow = false;
      itoa(dsc.panelData[panelByte] - 43, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine2 = lcdMessage;
      decoded = true;
      dsc.statusChanged = true;
    }

    if (dsc.panelData[panelByte] >= 0x4C && dsc.panelData[panelByte] <= 0x6B) {
      lcdLine1 = "Zone bat";
      strcpy(lcdMessage, "low:");
      zoneStatus[dsc.panelData[panelByte] - 74].batteryLow = true;
      itoa(dsc.panelData[panelByte] - 75, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine2 = lcdMessage;
      decoded = true;
      dsc.statusChanged = true;
    }

    if (dsc.panelData[panelByte] >= 0x6C && dsc.panelData[panelByte] <= 0x8B) {
      lcdLine1 = "Zone fault";
      strcpy(lcdMessage, "rest:");
      // zoneStatus[dsc.panelData[panelByte] - 106].open=false;
      itoa(dsc.panelData[panelByte] - 107, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine2 = lcdMessage;
      decoded = true;
      dsc.statusChanged = true;
    }

    if (dsc.panelData[panelByte] >= 0x8C && dsc.panelData[panelByte] <= 0xAB) {
      strcpy(lcdMessage, "Zone fault:");
      itoa(dsc.panelData[panelByte] - 139, charBuffer, 10);
      //zoneStatus[dsc.panelData[panelByte] - 138].open=true;
      strcat(lcdMessage, charBuffer);
      lcdLine1 = lcdMessage;
      lcdLine2 = " ";
      decoded = true;
      dsc.statusChanged = true;
    }

    if (dsc.panelData[panelByte] >= 0xB0 && dsc.panelData[panelByte] <= 0xCF) {
      strcpy(lcdMessage, "Zone bypass:");
      itoa(dsc.panelData[panelByte] - 175, charBuffer, 10);
      // zoneStatus[dsc.panelData[panelByte] - 174].bypassed=true;
      strcat(lcdMessage, charBuffer);
      lcdLine1 = lcdMessage;
      lcdLine2 = " ";
      decoded = true;
      dsc.statusChanged = true;
    }

    if (!decoded) {
      lcdLine1 = "Unknown data1";
      lcdLine2 = " ";
    }

    if (showEvent)
      eventStatusMsg.append(lcdLine1.append(" ").append(lcdLine2));
    else
      line2DisplayCallback(lcdLine1.append(" ").append(lcdLine2), partition);

  }

  void printPanelStatus2(byte panelByte, byte partition, bool showEvent = false) {
    bool decoded = true;
    std::string lcdLine1;
    std::string lcdLine2;
    switch (dsc.panelData[panelByte]) {
    case 0x2A:
      lcdLine1 = "Quick exit";
      lcdLine2 = " ";
      break;
    case 0x63:
      lcdLine1 = "Keybus fault";
      lcdLine2 = "rest";
      break;
    case 0x64:
      lcdLine1 = "Keybus fault";
      lcdLine2 = " ";
      break;
    case 0x66:
      lcdLine1 = "Zone bypass";
      lcdLine2 = "program";
      break;
    case 0x67:
      lcdLine1 = "Command";
      lcdLine2 = "output 1";
      break;
    case 0x68:
      lcdLine1 = "Command";
      lcdLine2 = "output 2";
      break;
    case 0x69:
      lcdLine1 = "Command";
      lcdLine2 = "output 3";
      break;
    case 0x8C:
      lcdLine1 = "Cold start";
      lcdLine2 = " ";
      break;
    case 0x8D:
      lcdLine1 = "Warm start";
      lcdLine2 = " ";
      break;
    case 0x8E:
      lcdLine1 = "Panel factory";
      lcdLine2 = "default";
      break;
    case 0x91:
      lcdLine1 = "Swinger shutdown";
      lcdLine2 = " ";
      break;
    case 0x93:
      lcdLine1 = "Disarmed:";
      lcdLine2 = "Keyswitch";
      break;
    case 0x96:
      lcdLine1 = "Armed:";
      lcdLine2 = "Keyswitch";
      break;
    case 0x97:
      lcdLine1 = "Armed:";
      lcdLine2 = "Keypad away";
      break;
    case 0x98:
      lcdLine1 = "Armed:";
      lcdLine2 = "Quick-arm";
      break;
    case 0x99:
      lcdLine1 = "Activate";
      lcdLine2 = "stay/away zones";
      break;
    case 0x9A:
      lcdLine1 = "Armed:";
      lcdLine2 = "Stay";
      break;
    case 0x9B:
      lcdLine1 = "Armed:";
      lcdLine2 = "Away";
      break;
    case 0x9C:
      lcdLine1 = "Armed:";
      lcdLine2 = "No ent del";
      break;
      // 0x9E - 0xC2: *1: Access codes 1-34, 40-42
      // 0xC3 - 0xC5: *5: Access codes 40-42
      // 0xC6 - 0xE5: Access codes 1-34, 40-42
      // 0xE6 - 0xE8: *6: Access codes 40-42
      // 0xE9 - 0xF0: Keypad restored: Slots 1-8
      // 0xF1 - 0xF8: Keypad trouble: Slots 1-8
      // 0xF9 - 0xFE: Zone expander restored: 1-6      
    case 0xFF:
      lcdLine1 = "Zone exp";
      lcdLine2 = "trble:1";
      break;
    default:
      decoded = false;
    }

    char lcdMessage[20];
    char charBuffer[4];
    if (dsc.panelData[panelByte] >= 0x67 && dsc.panelData[panelByte] <= 0x69) {
      strcpy(lcdMessage, "Cmd O/P:");
      itoa(dsc.panelData[panelByte] - 0x66, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine1 = lcdMessage;
      lcdLine2 = " ";
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0x9E && dsc.panelData[panelByte] <= 0xC2) {
      byte dscCode = dsc.panelData[panelByte] - 0x9D;
      lcdLine1 = "*1: ";
      if (dscCode >= 35) dscCode += 5;
      if (dscCode == 40) strcpy(lcdMessage, "Master code:");
      else strcpy(lcdMessage, "Access code:");
      itoa(dscCode, charBuffer, 10);
      std::string c=getUserName(charBuffer);
      strcat(lcdMessage, c.c_str());
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0xC3 && dsc.panelData[panelByte] <= 0xC5) {
      byte dscCode = dsc.panelData[panelByte] - 0xA0;
      lcdLine1 = "*5: ";
      if (dscCode >= 35) dscCode += 5;
      if (dscCode == 40) strcpy(lcdMessage, "Master code:");
      else strcpy(lcdMessage, "Access code:");
      itoa(dscCode, charBuffer, 10);
      std::string c=getUserName(charBuffer);
      strcat(lcdMessage, c.c_str());
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0xC6 && dsc.panelData[panelByte] <= 0xE5) {
      byte dscCode = dsc.panelData[panelByte] - 0xC5;
      if (dscCode >= 35) dscCode += 5;
      if (dscCode == 40) strcpy(lcdMessage, "Master code:");
      else strcpy(lcdMessage, "Access code:");
      itoa(dscCode, charBuffer, 10);
      std::string c=getUserName(charBuffer);
      strcat(lcdMessage, c.c_str());
      lcdLine1 = lcdMessage;
      lcdLine2 = " ";
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0xE6 && dsc.panelData[panelByte] <= 0xE8) {
      byte dscCode = dsc.panelData[panelByte] - 0xC3;
      lcdLine1 = "";
      if (dscCode >= 35) dscCode += 5;
      if (dscCode == 40) strcpy(lcdMessage, "Master code:");
      else strcpy(lcdMessage, "Access code:");
      itoa(dscCode, charBuffer, 10);
      std::string c=getUserName(charBuffer);
      strcat(lcdMessage, c.c_str());
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0xE9 && dsc.panelData[panelByte] <= 0xF0) {
      lcdLine1 = "Keypad rest";
      strcpy(lcdMessage, " slot:");
      itoa(dsc.panelData[panelByte] - 232, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0xF1 && dsc.panelData[panelByte] <= 0xF8) {
      lcdLine1 = "Keypad trble ";
      strcpy(lcdMessage, "slot: ");
      itoa(dsc.panelData[panelByte] - 240, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0xF9 && dsc.panelData[panelByte] <= 0xFE) {
      strcpy(lcdMessage, "Zone exp:");
      itoa(dsc.panelData[panelByte] - 248, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine1 = lcdMessage;
      lcdLine2 = " rest";
      decoded = true;
    }

    if (!decoded) {
      lcdLine1 = "Unknown data2";
      lcdLine2 = " ";
    }

    if (showEvent)
      eventStatusMsg.append(lcdLine1.append(" ").append(lcdLine2));
    else
      line2DisplayCallback(lcdLine1.append(" ").append(lcdLine2), partition);
  }

  void printPanelStatus3(byte panelByte, byte partition, bool showEvent = false) {
    bool decoded = true;
    std::string lcdLine1;
    std::string lcdLine2;
    switch (dsc.panelData[panelByte]) {
    case 0x05:
      lcdLine1 = "PC/RF5132:";
      lcdLine2 = "Suprvis. rest";
      break;
    case 0x06:
      lcdLine1 = "PC/RF5132:";
      lcdLine2 = "Suprvis. trble";
      break;
    case 0x09:
      lcdLine1 = "PC5204:";
      lcdLine2 = "Suprvis. rest";
      break;
    case 0x0A:
      lcdLine1 = "PC5204:";
      lcdLine2 = "Suprvis. trble";
      break;
    case 0x17:
      lcdLine1 = "Zone exp 7";
      lcdLine2 = "rest";
      break;
    case 0x18:
      lcdLine1 = "Zone exp 7";
      lcdLine2 = "trble";
      break;
      // 0x25 - 0x2C: Keypad tamper restored, slots 1-8
      // 0x2D - 0x34: Keypad tamper, slots 1-8
      // 0x35 - 0x3A: Module tamper restored, slots 9-14
      // 0x3B - 0x40: Module tamper, slots 9-14      
    case 0x41:
      lcdLine1 = "PC/RF5132:";
      lcdLine2 = "Tamper rest";
      break;
    case 0x42:
      lcdLine1 = "PC/RF5132: Tamper";
      lcdLine2 = " ";
      break;
    case 0x43:
      lcdLine1 = "PC5208: Tamper";
      lcdLine2 = "rest";
      break;
    case 0x44:
      lcdLine1 = "PC5208: Tamper";
      lcdLine2 = " ";
      break;
    case 0x45:
      lcdLine1 = "PC5204: Tamper";
      lcdLine2 = "rest";
      break;
    case 0x46:
      lcdLine1 = "PC5204: Tamper";
      lcdLine2 = " ";
      break;
    case 0x51:
      lcdLine1 = "Zone exp 7";
      lcdLine2 = "tamper rest";
      break;
    case 0x52:
      lcdLine1 = "Zone exp 7";
      lcdLine2 = "tamper";
      break;
    case 0xB3:
      lcdLine1 = "PC5204:";
      lcdLine2 = "Bat rest";
      break;
    case 0xB4:
      lcdLine1 = "PC5204:";
      lcdLine2 = "Bat trble";
      break;
    case 0xB5:
      lcdLine1 = "PC5204: Aux";
      lcdLine2 = "sup rest";
      break;
    case 0xB6:
      lcdLine1 = "PC5204: Aux";
      lcdLine2 = "sup trble";
      break;
    case 0xB7:
      lcdLine1 = "PC5204: Out 1";
      lcdLine2 = "rest";
      break;
    case 0xB8:
      lcdLine1 = "PC5204: Out 1";
      lcdLine2 = "trble";
      break;
    case 0xFF:
      lcdLine1 = "Ext status";
      lcdLine2 = " ";
      break;
    default:
      decoded = false;
    }

    char lcdMessage[20];
    char charBuffer[4];

    if (dsc.panelData[panelByte] <= 0x04) {
      strcpy(lcdMessage, "Zone exp. ");
      itoa(dsc.panelData[panelByte] + 2, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine1 = lcdMessage;
      lcdLine2 = "trouble";
      decoded = true;
    }
    if (dsc.panelData[panelByte] >= 0x25 && dsc.panelData[panelByte] <= 0x2C) {
      strcpy(lcdMessage, "keypad ");
      itoa(dsc.panelData[panelByte] - 0x24, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine1 = lcdMessage;
      lcdLine2 = "tamper rest";
      decoded = true;
    }
    if (dsc.panelData[panelByte] >= 0x2D && dsc.panelData[panelByte] <= 0x34) {
      strcpy(lcdMessage, "keypad ");
      itoa(dsc.panelData[panelByte] - 0x2c, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine1 = lcdMessage;
      lcdLine2 = "tamper";
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0x35 && dsc.panelData[panelByte] <= 0x3A) {
      strcpy(lcdMessage, "Zone expander ");
      itoa(dsc.panelData[panelByte] - 52, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine1 = lcdMessage;
      lcdLine2 = "tamper rest";
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0x3B && dsc.panelData[panelByte] <= 0x40) {
      strcpy(lcdMessage, "Zone expander ");
      itoa(dsc.panelData[panelByte] - 58, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine1 = lcdMessage;
      lcdLine2 = "tamper";
      decoded = true;
    }

    if (!decoded) {
      lcdLine1 = "Unknown data3";
      lcdLine2 = " ";
    }

    if (showEvent)
      eventStatusMsg.append(lcdLine1.append(" ").append(lcdLine2));
    else
      line2DisplayCallback(lcdLine1.append(" ").append(lcdLine2), partition);

  }

  void printPanelStatus4(byte panelByte, byte partition, bool showEvent = false) {
    bool decoded = true;
    std::string lcdLine1;
    std::string lcdLine2;
    switch (dsc.panelData[panelByte]) {
    case 0x86:
      lcdLine1 = "Period test";
      lcdLine2 = "trble";
      break;
    case 0x87:
      lcdLine1 = "Exit fault";
      lcdLine2 = " ";
      break;
    case 0x89:
      lcdLine1 = "Alarm canc";
      lcdLine2 = " ";
      break;
    default:
      decoded = false;
    }

    char lcdMessage[20];
    char charBuffer[4];

    if (dsc.panelData[panelByte] <= 0x1F) {
      strcpy(lcdMessage, "Zone alarm: ");
      byte zone = dsc.panelData[panelByte] - 33;
      if (zone > 0 && zone < maxZones)
        zoneStatus[zone - 1].alarm = true;
      itoa(dsc.panelData[panelByte] + 33, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine1 = lcdMessage;
      lcdLine2 = " ";
      decoded = true;
    } else if (dsc.panelData[panelByte] >= 0x20 && dsc.panelData[panelByte] <= 0x3F) {
      lcdLine1 = "Zone alarm";
      strcpy(lcdMessage, " rest: ");
      byte zone = dsc.panelData[panelByte] + 1;
      //   if (zone > 0 && zone < maxZones) 
      //    zoneStatus[zone-1].alarm=false;      
      itoa(dsc.panelData[panelByte] + 1, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine2 = lcdMessage;
      decoded = true;
    } else if (dsc.panelData[panelByte] >= 0x40 && dsc.panelData[panelByte] <= 0x5F) {
      strcpy(lcdMessage, "Zone tamper:");
      byte zone = dsc.panelData[panelByte] - 31;
      if (zone > 0 && zone < maxZones)
        zoneStatus[zone - 1].tamper = true;
      itoa(zone, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine1 = lcdMessage;
      lcdLine2 = " ";
      decoded = true;
    } else if (dsc.panelData[panelByte] >= 0x60 && dsc.panelData[panelByte] <= 0x7F) {
      lcdLine1 = "Zone tamper";
      strcpy(lcdMessage, " rest:");
      byte zone = dsc.panelData[panelByte] - 63;
      if (zone > 0 && zone < maxZones)
        zoneStatus[zone - 1].tamper = false;
      itoa(zone, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (!decoded) {
      lcdLine1 = "Unknown data4";
      lcdLine2 = " ";
    }

    if (showEvent)
      eventStatusMsg.append(lcdLine1.append(" ").append(lcdLine2));
    else
      line2DisplayCallback(lcdLine1.append(" ").append(lcdLine2), partition);

  }

  void printPanelStatus5(byte panelByte, byte partition, bool showEvent = false) {
    bool decoded = true;

    std::string lcdLine1;
    std::string lcdLine2;
    char lcdMessage[20];
    char charBuffer[4];

    if (dsc.panelData[panelByte] <= 0x39) {
      byte dscCode = dsc.panelData[panelByte] + 0x23;
      lcdLine1 = "Armed:";
      if (dscCode >= 40) dscCode += 3;
      strcpy(lcdMessage, "Access code:");
      itoa(dscCode, charBuffer, 10);
      std::string c=getUserName(charBuffer);
      strcat(lcdMessage, c.c_str());
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0x3A && dsc.panelData[panelByte] <= 0x73) {
      byte dscCode = dsc.panelData[panelByte] - 0x17;
      lcdLine1 = "Disarmed:";
      if (dscCode >= 40) dscCode += 3;
      strcpy(lcdMessage, "Access code:");
      itoa(dscCode, charBuffer, 10);
      std::string c=getUserName(charBuffer);
      strcat(lcdMessage, c.c_str());
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (!decoded) {
      lcdLine1 = "Unknown data5";
      lcdLine2 = " ";
    }

    if (showEvent)
      eventStatusMsg.append(lcdLine1.append(" ").append(lcdLine2));
    else
      line2DisplayCallback(lcdLine1.append(" ").append(lcdLine2), partition);

  }

  void printPanelStatus14(byte panelByte, byte partition, bool showEvent = false) {
    bool decoded = true;

    std::string lcdLine1;
    std::string lcdLine2;
    switch (dsc.panelData[panelByte]) {
    case 0xC0:
      lcdLine1 = "TLink";
      lcdLine2 = "com fault";
      break;
    case 0xC2:
      lcdLine1 = "Tlink";
      lcdLine2 = "net fault";
      break;
    case 0xC4:
      lcdLine1 = "TLink rec";
      lcdLine2 = "trouble";
      break;
    case 0xC5:
      lcdLine1 = "TLink receiver";
      lcdLine2 = "restored";
      break;
    default:
      decoded = false;
    }

    if (!decoded) {
      lcdLine1 = "Unknown data14";
      lcdLine2 = " ";
    }

    if (showEvent)
      eventStatusMsg.append(lcdLine1.append(" ").append(lcdLine2));
    else
      line2DisplayCallback(lcdLine1.append(" ").append(lcdLine2), partition);

  }

  void printPanelStatus16(byte panelByte, byte partition, bool showEvent = false) {
    bool decoded = true;
    std::string lcdLine1;
    std::string lcdLine2;

    switch (dsc.panelData[panelByte]) {
    case 0x80:
      lcdLine1 = "Trouble";
      lcdLine2 = "ack";
      break;
    case 0x81:
      lcdLine1 = "RF delin";
      lcdLine2 = "trouble";
      break;
    case 0x82:
      lcdLine1 = "RF delin";
      lcdLine2 = "rest";
      break;
    default:
      decoded = false;
    }

    if (!decoded) {
      lcdLine1 = "Unknown data16";
      lcdLine2 = "";
    }

    if (showEvent)
      eventStatusMsg.append(lcdLine1.append(" ").append(lcdLine2));
    else
      line2DisplayCallback(lcdLine1.append(" ").append(lcdLine2), partition);

  }

  void printPanelStatus17(byte panelByte, byte partition, bool showEvent = false) {
    bool decoded = true;
    std::string lcdLine1;
    std::string lcdLine2;
    char lcdMessage[20];
    char charBuffer[4];

    if (dsc.panelData[panelByte] >= 0x4A && dsc.panelData[panelByte] <= 0x83) {
      byte dscCode = dsc.panelData[panelByte] - 0x27;
      lcdLine1 = "*1: ";
      if (dscCode >= 40) dscCode += 3;
      strcpy(lcdMessage, "Access code:");
      itoa(dscCode, charBuffer, 10);
      std::string c=getUserName(charBuffer);
      strcat(lcdMessage, c.c_str());
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (dsc.panelData[panelByte] <= 0x24) {
      byte dscCode = dsc.panelData[panelByte] + 1;
      lcdLine1 = "*2: ";
      if (dscCode >= 40) dscCode += 3;
      strcpy(lcdMessage, "Access code:");
      itoa(dscCode, charBuffer, 10);
      std::string c=getUserName(charBuffer);
      strcat(lcdMessage, c.c_str());
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0x84 && dsc.panelData[panelByte] <= 0xBD) {
      byte dscCode = dsc.panelData[panelByte] - 0x61;
      lcdLine1 = "*2: ";
      if (dscCode >= 40) dscCode += 3;
      strcpy(lcdMessage, "Access code:");
      itoa(dscCode, charBuffer, 10);
      std::string c=getUserName(charBuffer);
      strcat(lcdMessage, c.c_str());
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0x25 && dsc.panelData[panelByte] <= 0x49) {
      byte dscCode = dsc.panelData[panelByte] - 0x24;
      lcdLine1 = "*3: ";
      if (dscCode >= 40) dscCode += 3;
      strcpy(lcdMessage, "Access code:");
      itoa(dscCode, charBuffer, 10);
      std::string c=getUserName(charBuffer);
      strcat(lcdMessage, c.c_str());
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0xBE && dsc.panelData[panelByte] <= 0xF7) {
      byte dscCode = dsc.panelData[panelByte] - 0x9B;
      lcdLine1 = "*3: ";
      if (dscCode >= 40) dscCode += 3;
      strcpy(lcdMessage, "Access code:");
      itoa(dscCode, charBuffer, 10);
      std::string c=getUserName(charBuffer);
      strcat(lcdMessage, c.c_str());
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (!decoded) {
      lcdLine1 = "Unknown data17";
      lcdLine2 = "";
    }

    if (showEvent)
      eventStatusMsg.append(lcdLine1.append(" ").append(lcdLine2));
    else
      line2DisplayCallback(lcdLine1.append(" ").append(lcdLine2), partition);

  }

  void printPanelStatus18(byte panelByte, byte partition, bool showEvent = false) {
    bool decoded = true;

    char lcdMessage[20];
    char charBuffer[4];
    std::string lcdLine1;
    std::string lcdLine2;
    if (dsc.panelData[panelByte] <= 0x39) {
      byte dscCode = dsc.panelData[panelByte] + 0x23;
      if (dscCode >= 40) dscCode += 3;
      strcpy(lcdMessage, "Access code:");
      itoa(dscCode, charBuffer, 10);
      std::string c=getUserName(charBuffer);
      strcat(lcdMessage, c.c_str());
      lcdLine1 = lcdMessage;
      lcdLine2 = " ";
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0x3A && dsc.panelData[panelByte] <= 0x95) {
      byte dscCode = dsc.panelData[panelByte] - 0x39;
      lcdLine1 = "*5: ";
      if (dscCode >= 40) dscCode += 3;
      strcpy(lcdMessage, "Access code:");
      itoa(dscCode, charBuffer, 10);
      std::string c=getUserName(charBuffer);
      strcat(lcdMessage, c.c_str());
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0x96 && dsc.panelData[panelByte] <= 0xF1) {
      byte dscCode = dsc.panelData[panelByte] - 0x95;
      lcdLine1 = "*6: ";
      if (dscCode >= 40) dscCode += 3;
      strcpy(lcdMessage, "Access code:");
      itoa(dscCode, charBuffer, 10);
      std::string c=getUserName(charBuffer);
      strcat(lcdMessage, c.c_str());
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (!decoded) {
      lcdLine1 = "Unknown data18";
      lcdLine2 = "";
    }

    if (showEvent)
      eventStatusMsg.append(lcdLine1.append(" ").append(lcdLine2));
    else
      line2DisplayCallback(lcdLine1.append(" ").append(lcdLine2), partition);

  }

  void printPanelStatus1B(byte panelByte, byte partition, bool showEvent = false) {
    bool decoded = true;
    std::string lcdLine1;
    std::string lcdLine2;
    switch (dsc.panelData[panelByte]) {
    case 0xF1:
      lcdLine1 = "System reset ";
      lcdLine2 = "trans";
      break;
    default:
      decoded = false;
    }

    if (!decoded) {
      lcdLine1 = "Unknown data1b";
      lcdLine2 = "";
    }

    if (showEvent)
      eventStatusMsg.append(lcdLine1.append(" ").append(lcdLine2));
    else
      line2DisplayCallback(lcdLine1.append(" ").append(lcdLine2), partition);
  }

};

#endif