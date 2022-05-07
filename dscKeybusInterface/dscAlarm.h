#ifndef dscalarm_h
#define dscalarm_h

#include "esphome.h"

#include "dscKeybusInterface.h"

//for documentation see project at https://github.com/Dilbert66/esphome-dsckeybus
#ifdef ESP32

#define dscClockPin 22 // esp32: GPIO22
#define dscReadPin 21 // esp32: GPIO21
#define dscWritePin 18 // esp32: GPIO18

#else

#define dscClockPin 5 // esp8266: GPIO5 
#define dscReadPin 4 // esp8266: GPIO4 
#define dscWritePin 15 // esp8266: GPIO15 

#endif

#define MAXZONES 32 //set to 64 if your system supports it
#define MODULESUPERVISION 0 //only enable this option if you want your virtual modules to be supervised by the panel and show errors if missing.  Not needed for operation.

//byte globalClockPin=id(dscClockPin);
//byte globalReadPin=id(dscReadPin);
//byte globalWritePin=id(dscWritePin);

dscKeybusInterface dsc(dscClockPin, dscReadPin, dscWritePin);
bool forceDisconnect;

void disconnectKeybus() {
  dsc.stop();
  dsc.keybusConnected = false;
  dsc.statusChanged = false;
  forceDisconnect = true;

}
enum troubleStatus {
  acStatus,
  batStatus,
  trStatus,
  fireStatus,
  panicStatus,
  rdyStatus,
  armStatus
};

class DSCkeybushome: public PollingComponent, public CustomAPIDevice {
  public: DSCkeybushome(const char * accessCode = "", unsigned long cmdWaitTime = 0): accessCode(accessCode),
  cmdWaitTime(cmdWaitTime) {}

  std:: function < void(uint8_t, bool) > zoneStatusChangeCallback;
  std:: function < void(std::string) > systemStatusChangeCallback;
  std:: function < void(troubleStatus, bool, int) > troubleStatusChangeCallback;
  std:: function < void(bool, int) > fireStatusChangeCallback;
  std:: function < void(std::string, int) > partitionStatusChangeCallback;
  std:: function < void(std::string, int) > partitionMsgChangeCallback;
  std:: function < void(std::string) > zoneMsgStatusCallback;
  std:: function < void(std::string, int) > troubleMsgStatusCallback;
  std:: function < void(uint8_t, bool) > relayChannelChangeCallback;
  std:: function < void(std::string, int) > line1DisplayCallback;
  std:: function < void(std::string, int) > line2DisplayCallback;
  std:: function < void(std::string, int) > eventInfoCallback;
  std:: function < void(std::string, int) > lightsCallback;
  std:: function < void(std::string, int) > beepsCallback;

  const std::string mainMenu[11] = {
    "Press # to exit",
    "Zone Bypass",
    "System Troubles",
    "Alarm Memory",
    "Door Chime",
    "Access Codes",
    "User Functions",
    "Output Contact",
    "",
    "No-Entry Arm",
    "Quick Arming"
  };

  const std::string troubleMenu[9] = {
    "Press # to exit",
    "Service Required *",
    "AC Failure",
    "Tel Line Trouble",
    "Comm Failure",
    "Zone Fault *",
    "Zone Tamper *",
    "Low Battery *",
    "System Time"
  };

  const std::string serviceMenu[9] = {
    "Press # to exit",
    "Low Battery",
    "Bell Circuit",
    "System Trouble",
    "System Tamper",
    "Mod Supervision",
    "RF Jam detected",
    "PC5204 Low Battery",
    "PC5204 AC Fail"
  };

  const std::string userMenu[7] = {
    "Press # to exit",
    "Time and Date",
    "Auto Arm/Disarm",
    "Auto Arm Time",
    "System Test",
    "System Serv/DLS",
    "Event Buffer"
  };

  const std::string statusMenu[5] = {
    "placeholder",
    "placeholder",
    "System Trouble:(*2) to view <>",
    "Bypass Active:(*1) to View <>",
    "Scroll to View:Open Zones <>"
  };

  const std::string statusMenuLabels[2] = {
    "System is Ready:Ready to Arm <>",
    "Secure System:Before Arming <>",
  };

  const std::string faultMenuLabels[2] = {
    "Press *2 for faults",
    "Press *3 for alarms"
  };

  const std::string STATUS_PENDING = "pending";
  const std::string STATUS_ARM = "armed_away";
  const std::string STATUS_STAY = "armed_home";
  const std::string STATUS_NIGHT = "armed_night";
  const std::string STATUS_OFF = "disarmed";
  const std::string STATUS_ONLINE = "online";
  const std::string STATUS_OFFLINE = "offline";
  const std::string STATUS_TRIGGERED = "triggered";
  const std::string STATUS_READY = "ready";
  const std::string STATUS_NOT_READY = "unavailable"; //ha alarm panel likes to see "unavailable" instead of not_ready when the system can't be armed
  const std::string MSG_ZONE_BYPASS = "zone_bypass_entered";
  const std::string MSG_ARMED_BYPASS = "armed_custom_bypass";
  const std::string MSG_NO_ENTRY_DELAY = "no_entry_delay";
  const std::string MSG_NONE = "no_messages";

  void onZoneStatusChange(std:: function < void(uint8_t zone, bool isOpen) > callback) {
    zoneStatusChangeCallback = callback;
  }
  void onSystemStatusChange(std:: function < void(std::string status) > callback) {
    systemStatusChangeCallback = callback;
  }
  void onFireStatusChange(std:: function < void(bool isOpen, int partition) > callback) {
    fireStatusChangeCallback = callback;
  }
  void onTroubleStatusChange(std:: function < void(troubleStatus ts, bool isOpen, int partition) > callback) {
    troubleStatusChangeCallback = callback;
  }
  void onPartitionStatusChange(std:: function < void(std::string status, int partition) > callback) {
    partitionStatusChangeCallback = callback;
  }
  void onPartitionMsgChange(std:: function < void(std::string msg, int partition) > callback) {
    partitionMsgChangeCallback = callback;
  }
  void onZoneMsgStatus(std:: function < void(std::string msg) > callback) {
    zoneMsgStatusCallback = callback;
  }
  void onTroubleMsgStatus(std:: function < void(std::string msg, int partition) > callback) {
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
  void onEventInfo(std:: function < void(std::string msg, int partition) > callback) {
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
  bool enable05Messages = true;
  unsigned long cmdWaitTime,
  beepTime;
  bool extendedBuffer,
  partitionChanged;
  int defaultPartition = 1;

  private: uint8_t zone;

  bool firstrun;

  struct partitionType {
    bool locked;
    unsigned long keyPressTime;
    byte lastStatus;
    byte status;
    byte lights;
    bool inprogram;
    byte digits;
    bool newData;
    bool hexMode;
    byte editIdx;
    bool decimalInput;
    bool hex;
    bool eventViewer;

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

  bool faultMenu[2];
  zoneType zoneStatus[MAXZONES];
  partitionType partitionStatus[dscPartitions];
  std::string zoneStatusMsg,
  previousZoneStatusMsg,
  systemMsg,
  previousSystemMsg,
  group1msg,
  group2msg;
  bool relayStatus[16],
  previousRelayStatus[16];
  bool sendCmd;
  byte system0,
  system1,
  previousSystem0,
  previousSystem1;
  byte programZones[dscZones];
  char decimalInputBuffer[6];
  byte line2Digit,
  line2Status;
  byte currentSelection;
  byte beeps,
  previousBeeps;
  uint8_t digitPtr;

  void setup() override {
    if (debug > 2)
      Serial.begin(115200);
    digitPtr = 0;
    set_update_interval(10);

    register_service( & DSCkeybushome::set_alarm_state, "set_alarm_state", {
      "state",
      "code",
      "partition"
    });
    register_service( & DSCkeybushome::alarm_disarm, "alarm_disarm", {
      "code"
    });
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
    currentSelection = 0xFF;
    systemStatusChangeCallback(STATUS_OFFLINE);
    forceDisconnect = false;
    #ifdef EXPANDER
    dsc.enableModuleSupervision = MODULESUPERVISION;
    dsc.debounce05 = (cmdWaitTime > 0);
    dsc.addModule(expanderAddr1);
    dsc.addModule(expanderAddr2);
    #endif
    dsc.maxZones = MAXZONES;
    dsc.resetStatus();
    dsc.processModuleData = true;
    dsc.begin();

  }

  void set_default_partition(int partition) {
    if (partition > 0 && partition < dscPartitions) {
      defaultPartition = partition;
      dsc.currentDefaultPartition = partition;
    }

  }

  void set_zone_fault(int zone, bool fault) {
    ESP_LOGD("Debug", "Setting Zone Fault: %d,%d", zone, fault);
    #ifdef EXPANDER
    dsc.setZoneFault(zone, fault);
    #endif

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

    ESP_LOGD("info", "key %d pressed, state=%02X,current=%02X,digits=%d,hex=%d,partition=%d,locked=%d", key, dsc.status[partition - 1], currentSelection + 1, partitionStatus[partition - 1].digits, partitionStatus[partition - 1].hex, partition, partitionStatus[partition - 1].locked);

    if (partitionStatus[partition - 1].locked) {
      line1DisplayCallback("System", partition);
      line2DisplayCallback("not available", partition);
      return;
    }

    if (partitionStatus[partition - 1].digits > 0) { //program mode data input 

      std::string tpl = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
      if (dsc.status[partition - 1] == 0xAA) {
        tpl = "XXXX    XXXXXX  XXXXXXXXXXXXXXXX";
      }

      if (key == '#') {
        partitionStatus[partition - 1].newData = false;
        if (key == '#' && partitionStatus[partition - 1].hex)
          dsc.setLCDSend(true, partition);
        else {
          dsc.write(key, partition);
        }
        return;

      } else if (key == '>') {
        if (!partitionStatus[partition - 1].hex && partitionStatus[partition - 1].editIdx + 1 == partitionStatus[partition - 1].digits) {
          dsc.setLCDSend(false, partition);
          return;
        }
        getNextIdx(tpl, partition);
        /*
        partitionStatus[partition-1].editIdx = partitionStatus[partition-1].editIdx + 1 < partitionStatus[partition-1].digits ? partitionStatus[partition-1].editIdx + 1 : partitionStatus[partition-1].editIdx = 0;
        */

      } else if (key == '<') {
        getPrevIdx(tpl, partition);
        /*
        partitionStatus[partition-1].editIdx = partitionStatus[partition-1].editIdx > 0 ? partitionStatus[partition-1].editIdx - 1 : partitionStatus[partition-1].digits - 1;
*/
      } else if (key != '*') {
        if (partitionStatus[partition - 1].decimalInput) {
          int num;
          decimalInputBuffer[partitionStatus[partition - 1].editIdx] = key;
          sscanf(decimalInputBuffer, "%d", & num);
          dsc.pgmBuffer.data[0] = num;
          //convert data to int and store it to data[0];

        } else {
          byte y = partitionStatus[partition - 1].editIdx / 2;
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
        if (!partitionStatus[partition - 1].hex && partitionStatus[partition - 1].editIdx + 1 == partitionStatus[partition - 1].digits) {
          dsc.setLCDSend(false, partition);
          return;
        }

        getNextIdx(tpl, partition);
        /*
        partitionStatus[partition-1].editIdx = partitionStatus[partition-1].editIdx + 1 < partitionStatus[partition-1].digits ? partitionStatus[partition-1].editIdx + 1 : partitionStatus[partition-1].editIdx = 0;
        */

      } else if (key == '*') {
        if (partitionStatus[partition - 1].hex) {
          partitionStatus[partition - 1].hexMode = partitionStatus[partition - 1].hexMode ? false : true;
        } else {
          dsc.write(key, partition);
          return;
        }
      }
      dsc.write("$", partition);
      setStatus(partition - 1, true);
      return;
    }

    if (key == '#') {
      currentSelection = 0xFF;
      //   line2Status = 0;
      // line2Digit = 0;
      dsc.write(key, partition);
      partitionStatus[partition - 1].eventViewer = false;
      setStatus(partition - 1, true);
      return;
    }

    if (dsc.status[partition - 1] < 0x04) {
      dsc.write(key, partition);
      if (key == '<') {
        currentSelection = getPreviousOpenZone(currentSelection, partition);
      } else
      if (key == '>') {
        currentSelection = getNextOpenZone(currentSelection, partition);
      };
      setStatus(partition - 1, true);
    } else if (dsc.status[partition - 1] == 0x9E) { // * mainmenu
      dsc.write(key, partition);
      if (key == '<') {
        currentSelection = currentSelection >= 11 ? 10 : (currentSelection > 0 ? currentSelection - 1 : 10);
        currentSelection = mainMenu[currentSelection] != "" ? currentSelection : currentSelection - 1;
        if (currentSelection < 11) line2DisplayCallback(mainMenu[currentSelection], partition);

      } else if (key == '>') {
        currentSelection = currentSelection >= 10 ? 0 : currentSelection + 1;
        currentSelection = mainMenu[currentSelection] != "" ? currentSelection : currentSelection + 1;
        if (currentSelection < 11) line2DisplayCallback(mainMenu[currentSelection], partition);
      } else if (key == '*' && currentSelection > 0) {
        char s[5];
        sprintf(s, "%d", currentSelection % 10);
        const char * out = strcpy(new char[3], s);
        currentSelection = 0xFF;
        dsc.write(s, partition);
      } else currentSelection = 0xFF;
    } else if (dsc.status[partition - 1] == 0xA1) { //trouble
      dsc.write(key, partition);
      if (key == '*' && currentSelection > 0) {
        char s[5];
        sprintf(s, "%d", currentSelection);
        const char * out = strcpy(new char[3], s);
        currentSelection = 0xFE;
        dsc.write(out, partition);
      } else if (key == '>') {
        currentSelection = getNextOption(currentSelection);
        if (currentSelection < 9) line2DisplayCallback(troubleMenu[currentSelection], partition);
      } else if (key == '<') {
        currentSelection = getPreviousOption(currentSelection);
        if (currentSelection < 9) line2DisplayCallback(troubleMenu[currentSelection], partition);
      } else currentSelection = 0xFF;
    } else if (dsc.status[partition - 1] == 0xC8) { //trouble
      dsc.write(key, partition);
      if (key == '*' && currentSelection > 0) {
        char s[5];
        sprintf(s, "%d", currentSelection);
        const char * out = strcpy(new char[3], s);
        currentSelection = 0xFF;
        dsc.write(out, partition);
      } else if (key == '>') {
        currentSelection = getNextOption(currentSelection);
        if (currentSelection < 9) line2DisplayCallback(serviceMenu[currentSelection], partition);
      } else if (key == '<') {
        currentSelection = getPreviousOption(currentSelection);
        if (currentSelection < 9) line2DisplayCallback(serviceMenu[currentSelection], partition);
      } else currentSelection = 0xFF;
    } else if (dsc.status[partition - 1] == 0xA9 && !partitionStatus[partition - 1].eventViewer) { // * user functions
      if (key == '<') {
        currentSelection = currentSelection >= 7 ? 6 : (currentSelection > 0 ? currentSelection - 1 : 6);
        currentSelection = userMenu[currentSelection] != "" ? currentSelection : currentSelection - 1;
        if (currentSelection < 7) line2DisplayCallback(userMenu[currentSelection], partition);
      } else if (key == '>') {
        currentSelection = currentSelection >= 6 ? 0 : currentSelection + 1;
        currentSelection = userMenu[currentSelection] != "" ? currentSelection : currentSelection + 1;
        if (currentSelection < 7) line2DisplayCallback(userMenu[currentSelection], partition);
      } else if (key == '*' && currentSelection > 0) {
        char s[5];
        if (currentSelection == 6) {
          partitionStatus[partition - 1].eventViewer = true;
          dsc.write('b', partition);
        } else {
          dsc.write(key, partition);
          sprintf(s, "%d", currentSelection % 6);
          const char * out = strcpy(new char[3], s);
          currentSelection = 0xFF;
          dsc.write(s, partition);
        }
      } else currentSelection = 0xFF;
    } else if (dsc.status[partition - 1] == 0xA2) { //alarm memory
      dsc.write(key, partition);
      if (key == '>') currentSelection = getNextOption(currentSelection);
      else if (key == '<') currentSelection = getPreviousOption(currentSelection);
      else currentSelection = 0xFF;
    } else if (dsc.status[partition - 1] == 0xA6) { //user codes
      dsc.write(key, partition);
      if (key == '*') {
        char s[5];
        sprintf(s, "%02d", currentSelection);
        const char * out = strcpy(new char[3], s);
        dsc.write(out, partition);
        // ESP_LOGD("info","Wrote key bypass %s",s);
      } else if (key == '>') currentSelection = getNextUserCode(currentSelection);
      else if (key == '<') currentSelection = getPreviousUserCode(currentSelection);
      setStatus(partition - 1, true);
    } else if (dsc.status[partition - 1] == 0x11) { //alarms
      dsc.write(key, partition);
      if (key == '>') currentSelection = getNextAlarmedZone(currentSelection, partition);
      else if (key == '<') currentSelection = getPreviousAlarmedZone(currentSelection, partition);
      setStatus(partition - 1, true);
    } else if (dsc.status[partition - 1] == 0xA0) { //bypass
      dsc.write(key, partition);
      if (key == '*') {
        char s[5];
        sprintf(s, "%02d", currentSelection + 1);
        const char * out = strcpy(new char[3], s);
        dsc.write(out, partition);
        // ESP_LOGD("info","Wrote key bypass %s",s);
      } else if (key == '>') currentSelection = getNextEnabledZone(currentSelection, partition);
      else if (key == '<') currentSelection = getPreviousEnabledZone(currentSelection, partition);
      setStatus(partition - 1, true);
    } else {
      dsc.write(key, partition);
      setStatus(partition - 1, false);
    }

  }

  void getPrevIdx(std::string tpl, byte partition) {
    do {
      partitionStatus[partition - 1].editIdx = partitionStatus[partition - 1].editIdx > 0 ? partitionStatus[partition - 1].editIdx - 1 : partitionStatus[partition - 1].digits - 1;
    } while (tpl[partitionStatus[partition - 1].editIdx] != 'X' && partitionStatus[partition - 1].editIdx != partitionStatus[partition - 1].digits - 1);

  }

  void getNextIdx(std::string tpl, byte partition) {
    do {
      partitionStatus[partition - 1].editIdx = partitionStatus[partition - 1].editIdx + 1 < partitionStatus[partition - 1].digits ? partitionStatus[partition - 1].editIdx + 1 : partitionStatus[partition - 1].editIdx = 0;
    } while (tpl[partitionStatus[partition - 1].editIdx] != 'X' && partitionStatus[partition - 1].editIdx != 0);

  }

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
    } else if (!partitionStatus[partition - 1].locked) dsc.write(keys, partition);
  }

  bool isInt(std::string s, int base) {
    if (s.empty() || std::isspace(s[0])) return false;
    char * p;
    strtol(s.c_str(), & p, base);
    return ( * p == 0);
  }

  void set_alarm_state(std::string state, std::string code = "", int partition = -1) {

    if (code.length() != 4 || !isInt(code, 10)) code = ""; // ensure we get a numeric 4 digit code
    const char * alarmCode = strcpy(new char[code.length() + 1], code.c_str());
    if (!partition) partition = defaultPartition;
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
    else if (state.compare("D") == 0 && (dsc.armed[partition] || dsc.exitDelay[partition])) {
      if (code.length() == 4) { // ensure we get 4 digit code
        dsc.write(alarmCode, partition);
      }
    }
  }

  void printPacket(const char * label, char cmd, volatile byte cbuf[], int len) {

    std::string s;
    char s1[4];
    for (int c = 0; c < len; c++) {
      sprintf(s1, "%02X ", cbuf[c]);
      s.append(s1);
    }
    ESP_LOGI(label, "%02X: %s", cmd, s.c_str());

  }

  bool getEnabledZones(byte inputByte, byte startZone, byte partition) {
    bool zonesEnabled = false;
    byte zone;
    for (byte panelByte = inputByte; panelByte <= inputByte + 3; panelByte++) {
      if (dsc.panelData[panelByte] != 0) {
        zonesEnabled = true;
        for (byte zoneBit = 0; zoneBit < 8; zoneBit++) {
          zone = (zoneBit + startZone) + ((panelByte - inputByte) * 8) - 1;
          if (zone >= MAXZONES) continue;
          zoneStatus[zone].partition = partition;
          if (bitRead(dsc.panelData[panelByte], zoneBit)) {
            zoneStatus[zone].enabled = true;
          } else {
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
        //ESP_LOGD("test","checking code %d, code=%d",option,code);
        if (bitRead(programZones[optionGroup], optionBit) && option == code) {
          return true;
        }

      }
    }
    return false;
  }

  byte getNextOption(byte start) {
    byte option, optionGroup, s;
    s = start >= MAXZONES ? 0 : start;
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
    s = start >= MAXZONES || start == 0 ? MAXZONES : start;
    for (byte optionGroup = dscZones - 1; optionGroup >= 0 && optionGroup < dscZones; optionGroup--) {
      for (byte optionBit = 7; optionBit >= 0 && optionBit < 8; optionBit--) {
        byte option = optionBit + 1 + (optionGroup * 8);
        //  ESP_LOGD("info","checking option %d,%d",option,s);
        if (bitRead(programZones[optionGroup], optionBit)) {
          //   ESP_LOGD("info","found bit %d",option);
          if (option < s) return option;
        }

      }
    }
    return 0;
  }

  byte getNextUserCode(byte start) {

    if (start < 95) return start + 1;
    else return 1;

  }

  byte getPreviousUserCode(byte start) {
    if (start > 1) return start - 1;
    else return 95;
  }

  byte getPreviousOpenZone(byte start, byte partition) {
    byte menu, option;

    if (start > MAXZONES || start < 2) start = MAXZONES + 0x10;

    //open zones start at selection 0x10 (16 decimal)
    //menu options start at 00 -> 06  . We do this so that when scrolling we scroll the menu options first
    for (option = start - 1; option >= 0 && option < MAXZONES + 0x10; option--) {
      zone = option >= 0x10 ? option - 0x10 : 0xFF;
      if (zone < MAXZONES && zoneStatus[zone].enabled && zoneStatus[zone].partition == partition && zoneStatus[zone].open) return option;
      if (option == 1) return 1; //ready /open
      if (option == 2 && dsc.lights[partition - 1] & 0x10) return 2; //trouble
      if (option == 3 && dsc.lights[partition - 1] & 0x08) return 3; //bypass
      if (option == 4 && !(dsc.lights[partition - 1] & 0x3)) return 4; //bypass 

    }

    return 0xFF;

  }

  byte getNextOpenZone(byte start, byte partition) {
    byte zone, option;
    //open zones start at selection 0x10 (16 decimal)
    //menu options start at 00 -> 04  . We do this so that when scrolling we scroll the menu options first
    if (start > MAXZONES) start = 0;
    for (option = start + 1; option < MAXZONES + 0x10; option++) {
      zone = option >= 0x10 ? option - 0x10 : 0xFF;
      if (zone < MAXZONES && zoneStatus[zone].enabled && zoneStatus[zone].partition == partition && zoneStatus[zone].open) return option;

      if (option == 1) return 1; //ready / open
      if (option == 2 && dsc.lights[partition - 1] & 0x10) return 2; //trouble
      if (option == 3 && dsc.lights[partition - 1] & 0x08) return 3; //bypass
      if (option == 4 && !(dsc.lights[partition - 1] & 0x3)) return 4; //bypass  
    }
    return 0;
  }

  byte getNextEnabledZone(byte start, byte partition) {
    byte zone;
    for (zone = start + 1; zone < MAXZONES; zone++) {
      if (zoneStatus[zone].partition == partition && zoneStatus[zone].enabled) return zone;
    }
    return 0xFF;
  }

  byte getPreviousEnabledZone(byte start, byte partition) {
    byte zone, s;
    s = start == 0xFF || start == 0 ? MAXZONES : start;
    for (zone = s - 1; zone >= 0 && zone < MAXZONES; zone--) {
      if (zoneStatus[zone].partition == partition && zoneStatus[zone].enabled) return zone;
    }
    return 0xFF;
  }

  byte getNextAlarmedZone(byte start, byte partition) {
    byte zone;
    for (zone = start + 1; zone < MAXZONES; zone++) {
      if (zoneStatus[zone].partition == partition && zoneStatus[zone].alarm) return zone;
    }
    return 0xFF;
  }

  byte getPreviousAlarmedZone(byte start, byte partition) {
    byte zone, s;
    s = start == 0xFF || start == 0 ? MAXZONES : start;
    for (zone = s - 1; zone >= 0 && zone < MAXZONES; zone--) {
      if (zoneStatus[zone].partition == partition && zoneStatus[zone].alarm) return zone;
    }
    return 0xFF;
  }

  void getBypassZones(byte partition) {
    if (dsc.status[partition] == 0xA0) { //bypass zones
      //ESP_LOGD("info", "Fetch bypass zones for partition %d",partition+1);
      for (byte zoneGroup = 0; zoneGroup < dscZones; zoneGroup++) {
        for (byte zoneBit = 0; zoneBit < 8; zoneBit++) {
          zone = zoneBit + (zoneGroup * 8);
          if (!(zoneStatus[zone].partition == partition + 1 && zoneStatus[zone].enabled) || zone >= MAXZONES) continue;
          if (bitRead(programZones[zoneGroup], zoneBit)) {
            zoneStatus[zone].bypassed = true;
          } else {
            zoneStatus[zone].bypassed = false;
          }

        }
      }
      setStatus(partition, true);
    }
  }

  void rtrim(std::string & s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
      std::not1(std::ptr_fun < int, int > (std::isspace))).base(), s.end());
  }

  void update() override {

    if (millis() - beepTime > 2000 && beeps > 0) {
      beeps = 0;
      for (byte partition = 1; partition <= dscPartitions; partition++) {
        if (dsc.disabled[partition - 1]) continue;
        beepsCallback("0", partition);
      }
    }

    if (!forceDisconnect && dsc.loop()) { //Processes data only when a valid Keybus command has been read
      //  dsc.forceRedundant = false;
      if (firstrun) {
        #ifdef EXPANDER
        dsc.clearZoneRanges(); // start with clear expanded zones
        #endif
        //add partition loop
        for (byte partition = 1; partition <= dscPartitions; partition++) {
          if (dsc.disabled[partition - 1]) continue;
          dsc.write("*", partition);
          dsc.write("27##", partition); //fetch low battery status
        }

      }
      //ESP_LOGD("info","cmd=%02X",dsc.panelData[0]);

      if (debug > 1)
        printPacket("Paneldata: ", dsc.panelData[0], dsc.panelData, 16);
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

      processStatus();
      for (byte partition = 0; partition < dscPartitions; partition++) {
        if (dsc.disabled[partition]) continue;
        getBypassZones(partition);
      }
      if (dsc.panelData[0] == 0x0A && dsc.panelData[3] == 0xBA) { //low battery zones
        for (byte panelByte = 4; panelByte < 8; panelByte++) {
          for (byte zoneBit = 0; zoneBit < 8; zoneBit++) {
            zone = zoneBit + ((panelByte - 4) * 8);
            if (zone >= MAXZONES) continue;
            if (bitRead(dsc.panelData[panelByte], zoneBit)) {
              zoneStatus[zone].batteryLow = true;
            } else
              zoneStatus[zone].batteryLow = false;
          }
        }
      }
      if (dsc.panelData[0] == 0x0A && dsc.panelData[3] == 0xA1) { //system status
        system0 = dsc.panelData[4];

      }
      if (dsc.panelData[0] == 0x0A && dsc.panelData[3] == 0xC8) { //service required menu
        system1 = dsc.panelData[4];

      }
      if (dsc.panelData[0] == 0xE6 && dsc.panelData[2] == 0x1A) { //system status comm/time/ac
        for (byte partition = 0; partition < dscPartitions; partition++) {
          if (dsc.disabled[partition]) continue;
          if (bitRead(dsc.panelData[6], 4)) //ac status bit
            troubleStatusChangeCallback(acStatus, false, partition + 1);
          else troubleStatusChangeCallback(acStatus, true, partition + 1);
        }
        // dsc.write("*");
        //dsc.write("*21##");

      }
      if (dsc.panelData[0] == 0xA5 && dsc.panelData[6] == 0 && (dsc.trouble || sendCmd)) { //periodic - sent every 4 minutes
        sendCmd = false;
        // dsc.write("*");
        // dsc.write("*21##");
        // we only send the system status request a minute or so after the trouble status so that we don't impact warning beeps sent after the trouble light is turned on. 

      }
      if (dsc.panelData[0] == 0x87) { //relay cmd
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

      if (dsc.panelData[0] == 0xB1) {
        getEnabledZones(2, 1, 1);
        getEnabledZones(6, 1, 2);
        /*
        for(byte z=0;z<MAXZONES;z++) {
            if (zoneStatus[z].enabled) {
                    ESP_LOGD("info","Zone %d enabled on partition %d",z+1,zoneStatus[z].partition);
           
            }
            
        }
        */
      }

      if (dsc.panelData[0] == 0xE6 && dsc.panelData[2] == 0x2C) {
        getEnabledZones(4, 33, dsc.panelData[2]);
      }
      /*
      if (dsc.panelData[0]==0x0A && dsc.panelData[3]==0xB9) { //tamper
          for (byte panelByte = 4; panelByte < 8; panelByte++) {
                  for (byte zoneBit = 0; zoneBit < 8; zoneBit++) {
                      zone=zoneBit + ((panelByte-4) *  8);
                      if (bitRead(dsc.panelData[panelByte],zoneBit)) {
                          if (zone < MAXZONES)
                              zoneStatus[zone].tamper=true;
                      } else  if (zone < MAXZONES)
                                  zoneStatus[zone].tamper=false;
                  }
          }
      }
      */

      firstrun = false;

    }

    if (!forceDisconnect && dsc.statusChanged) { // Processes data only when a valid Keybus command has been read and statuses were changed
      dsc.statusChanged = false; // Reset the status tracking flag

      if (debug == 1)
        printPacket("Paneldata: ", dsc.panelData[0], dsc.panelData, 16);

      for (byte partition = 0; partition < dscPartitions; partition++) {
        if (dsc.disabled[partition]) continue;
        setStatus(partition);
      }
      // If the Keybus data buffer is exceeded, the sketch is too busy to process all Keybus commands.  Call
      // handlePanel() more often, or increase dscBufferSize in the library: src/dscKeybusInterface.h
      if (dsc.bufferOverflow) ESP_LOGD("Error", "Keybus buffer overflow");
      dsc.bufferOverflow = false;

      // Checks if the interface is connected to the Keybus
      if (dsc.keybusChanged) {
        dsc.keybusChanged = false; // Resets the Keybus data status flag
        if (dsc.keybusConnected) {
          systemStatusChangeCallback(STATUS_ONLINE);
        } else systemStatusChangeCallback(STATUS_OFFLINE);
      }

      //non partition specific problems so we send to all keypads
      for (byte partition = 0; partition < dscPartitions; partition++) {
        if (dsc.disabled[partition]) continue;

        // Sends the access code when needed by the panel for arming
        if (dsc.status[partition] == 0x9F && dsc.writeAccessCode[partition] && isInt(accessCode, 10)) {
          dsc.accessCodePrompt = false;
          dsc.write(accessCode, partition + 1);
          if (debug > 0) ESP_LOGD("Debug", "got access code prompt for partition %d", partition + 1);
        }

        if (dsc.powerChanged) {
          dsc.powerChanged = false;

          if (dsc.powerTrouble) {
            troubleStatusChangeCallback(acStatus, false, partition + 1); //no ac
          } else troubleStatusChangeCallback(acStatus, true, partition + 1);

        }

        if (dsc.batteryChanged) {
          dsc.batteryChanged = false;
          if (dsc.batteryTrouble) {
            troubleStatusChangeCallback(batStatus, true, partition + 1);
          } else troubleStatusChangeCallback(batStatus, false, partition + 1);
          sendCmd = true;
        }

        if (dsc.keypadFireAlarm) {
          dsc.keypadFireAlarm = false;
          partitionMsgChangeCallback("Keypad Fire Alarm", partition + 1);
        }
        if (dsc.keypadPanicAlarm) {
          dsc.keypadPanicAlarm = false;
          troubleStatusChangeCallback(panicStatus, true, partition + 1);
          partitionMsgChangeCallback("Keypad Panic Alarm", partition + 1);
        }
        // Publishes trouble status
        if (dsc.troubleChanged) {
          dsc.troubleChanged = false; // Resets the trouble status flag
          if (dsc.trouble) troubleStatusChangeCallback(trStatus, true, partition + 1); // Trouble alarm tripped
          else troubleStatusChangeCallback(trStatus, false, partition + 1); // Trouble alarm restored
          // we set a flag to only send the system status request a minute or so after the trouble status so that we don't impact warning beeps sent after the trouble light is turned on.
          sendCmd = true;
        }
      }
      // Publishes status per partition
      for (byte partition = 0; partition < dscPartitions; partition++) {

        if (dsc.disabled[partition]) continue; //skip disabled or partitions in install programming	

        // Publishes alarm status
        if (dsc.alarmChanged[partition]) {
          dsc.alarmChanged[partition] = false; // Resets the partition alarm status flag
          if (dsc.alarm[partition]) {
            dsc.readyChanged[partition] = false; //if we are triggered no need to trigger a ready state change
            dsc.armedChanged[partition] = false; // no need to display armed changed
            partitionStatusChangeCallback(STATUS_TRIGGERED, partition + 1);
          }
        }

        // Publishes armed/disarmed status
        if (dsc.armedChanged[partition]) {
          dsc.armedChanged[partition] = false; // Resets the partition armed status flag
          if (dsc.armed[partition]) {
            troubleStatusChangeCallback(armStatus, true, partition + 1);
            if ((dsc.armedAway[partition] || dsc.armedStay[partition]) && dsc.noEntryDelay[partition]) partitionStatusChangeCallback(STATUS_NIGHT, partition + 1);
            else if (dsc.armedStay[partition])
              partitionStatusChangeCallback(STATUS_STAY, partition + 1);
            else partitionStatusChangeCallback(STATUS_ARM, partition + 1);
          } else {
            partitionStatusChangeCallback(STATUS_OFF, partition + 1);
            troubleStatusChangeCallback(armStatus, false, partition + 1);
          }

        }
        // Publishes exit delay status
        if (dsc.exitDelayChanged[partition]) {
          dsc.exitDelayChanged[partition] = false; // Resets the exit delay status flag
          if (dsc.exitDelay[partition]) partitionStatusChangeCallback(STATUS_PENDING, partition + 1);
          else if (!dsc.exitDelay[partition] && !dsc.armed[partition]) partitionStatusChangeCallback(STATUS_OFF, partition + 1);
        }

        // Publishes ready status
        // ESP_LOGD("test","ready changed status partition=%d,ready=%d,ready changed=%d",partition+1,dsc.ready[partition],dsc.readyChanged[partition]);
        if (dsc.readyChanged[partition]) {

          dsc.readyChanged[partition] = false; // Resets the partition alarm status flag
          if (dsc.ready[partition] && !dsc.armed[partition]) {
            partitionStatusChangeCallback(STATUS_OFF, partition + 1);
            troubleStatusChangeCallback(rdyStatus, true, partition + 1);
          } else {
            if (!dsc.armed[partition]) partitionStatusChangeCallback(STATUS_NOT_READY, partition + 1);
            troubleStatusChangeCallback(rdyStatus, false, partition + 1);
          }

        }

        // Publishes fire alarm status
        if (dsc.fireChanged[partition]) {
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
        dsc.openZonesStatusChanged = false; // Resets the open zones status flag
        for (byte zoneGroup = 0; zoneGroup < dscZones; zoneGroup++) {
          for (byte zoneBit = 0; zoneBit < 8; zoneBit++) {
            if (bitRead(dsc.openZonesChanged[zoneGroup], zoneBit)) { // Checks an individual open zone status flag
              bitWrite(dsc.openZonesChanged[zoneGroup], zoneBit, 0); // Resets the individual open zone status flag
              zone = zoneBit + (zoneGroup * 8);
              if (zone >= MAXZONES) continue;
              if (bitRead(dsc.openZones[zoneGroup], zoneBit)) {
                zoneStatusChangeCallback(zone + 1, true); // Zone open
                zoneStatus[zone].open = true;
              } else {
                zoneStatusChangeCallback(zone + 1, false); // Zone closed
                zoneStatus[zone].open = false;
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
      if (dsc.alarmZonesStatusChanged) {
        dsc.alarmZonesStatusChanged = false; // Resets the alarm zones status flag
        for (byte zoneGroup = 0; zoneGroup < dscZones; zoneGroup++) {
          for (byte zoneBit = 0; zoneBit < 8; zoneBit++) {
            if (bitRead(dsc.alarmZonesChanged[zoneGroup], zoneBit)) { // Checks an individual alarm zone status flag
              bitWrite(dsc.alarmZonesChanged[zoneGroup], zoneBit, 0); // Resets the individual alarm zone status flag
              zone = zoneBit + (zoneGroup * 8);
              if (zone >= MAXZONES) continue;
              if (bitRead(dsc.alarmZones[zoneGroup], zoneBit)) {
                zoneStatus[zone].alarm = true;
              } else {
                zoneStatus[zone].alarm = false;
              }
            }
          }
        }
      }

      zoneStatusMsg = "";
      char s1[7];
      for (int x = 0; x < MAXZONES; x++) {
        /*
            if (zoneStatus[x].open) {
                sprintf(s1,"OP:%d",x+1);
                if (zoneStatusMsg!="") zoneStatusMsg.append(",");
                zoneStatusMsg.append(s1);
            }
           */

        if (zoneStatus[x].alarm) {
          sprintf(s1, "AL:%d", x + 1);
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
      if (zoneStatusMsg == "") zoneStatusMsg = "OK";

      systemMsg = "";
      if (bitRead(system1, 0)) {
        if (systemMsg != "") systemMsg.append(",");
        systemMsg.append("BAT");
      }
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
      //  if (previousSystemMsg != systemMsg) 
      //   troubleMsgStatusCallback(systemMsg);
      previousSystemMsg = systemMsg;

    }

    if (!forceDisconnect && dsc.handleModule()) {
      if (dsc.panelData[0] == 0x41) {
        for (byte zoneByte = 0; zoneByte < 4; zoneByte++) {
          byte zoneBit = 0;
          for (int x = 7; x >= 0; x--) {
            zone = zoneBit + (zoneByte * 8);
            if (zone >= MAXZONES) continue;
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
    }
    // if (zoneStatusMsg != previousZoneStatusMsg)
    //   zoneMsgStatusCallback(zoneStatusMsg); 
    previousZoneStatusMsg = zoneStatusMsg;

  }

  void setStatus(byte partition, bool force = false, bool skip = false) {

    // if (dsc.status[partition] == partitionStatus[partition].lastStatus && line2Status != dsc.status[partition] && beeps == 0 && !force) return;
    if (dsc.status[partition] == partitionStatus[partition].lastStatus && beeps == 0 && !force) return;

    std::string lcdLine1;
    std::string lcdLine2;

    bool options = false;
    partitionStatus[partition].digits = 0;
    partitionStatus[partition].hex = false;
    partitionStatus[partition].decimalInput = false;

    ESP_LOGD("info", "status %02X, last status %02X,line2status %02X,selection %02X,partition=%d", dsc.status[partition], partitionStatus[partition].lastStatus, line2Status, currentSelection, partition + 1);
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
      lcdLine2 = " ";
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

    ESP_LOGD("test", "digits = %d,status=%02X,previoustatus=%02X,newdata=%d,locked=%d,partition=%d", partitionStatus[partition].digits, dsc.status[partition], partitionStatus[partition].lastStatus, partitionStatus[partition].newData, partitionStatus[partition].locked, partition + 1);

    if (millis() - partitionStatus[partition].keyPressTime > 1000 && dsc.status[partition] > 0x8B) {
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
    }

    if (!skip) {

      //field display  for decimal or hex entries
      if (partitionStatus[partition].lastStatus != dsc.status[partition] && !partitionStatus[partition].locked && partitionStatus[partition].digits && !partitionStatus[partition].newData) {
        dsc.setLCDReceive(partitionStatus[partition].digits, partition + 1);
        partitionStatus[partition].editIdx = 0;
        partitionStatus[partition].hexMode = false;
        partitionStatus[partition].newData = true;
        lcdLine1 = "";
        lcdLine2 = "";
        ESP_LOGD("test", "in setlcd: digits = %d,status=%02X,previoustatus=%02X,newdata=%d,locked=%d", partitionStatus[partition].digits, dsc.status[partition], partitionStatus[partition].lastStatus, partitionStatus[partition].newData, partitionStatus[partition].locked);

      } else if (partitionStatus[partition].digits && partitionStatus[partition].newData) {

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
        for (int x = 0; x < partitionStatus[partition].digits; x++) {
          y = x / 2;
          if (partitionStatus[partition].decimalInput)
            c = decimalInputBuffer[x] - 48;
          else
            c = x % 2 ? dsc.pgmBuffer.data[y] & 0x0F : (dsc.pgmBuffer.data[y] & 0xF0) >> 4;
          std::string tpl = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";

          if (dsc.status[partition] == 0xAA) {
            tpl = "XXXX    XXXXXX  XXXXXXXXXXXXXXXX";
          }
          if (tpl[x] == 'X') {
            if (x == partitionStatus[partition].editIdx)
              sprintf(s, "[%1X]", c);
            else
              sprintf(s, "%1X", c);
          } else sprintf(s, " ");

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

        if (currentSelection > 1 && currentSelection < 5) {
          int pos = statusMenu[currentSelection].find(":");
          lcdLine1 = statusMenu[currentSelection].substr(0, pos);
          lcdLine2 = statusMenu[currentSelection].substr(pos + 1);
        } else if (currentSelection >= 0x10 && currentSelection < MAXZONES + 0x10) {
          char s[16];
          lcdLine1 = "Open Zones";
          sprintf(s, "Zone %02d  <>", (currentSelection - 0x10) + 1);
          lcdLine2 = s;
        } else {
          byte c = dsc.ready[partition] ? 0 : 1;
          int pos = statusMenuLabels[c].find(":");
          lcdLine1 = statusMenuLabels[c].substr(0, pos);
          lcdLine2 = statusMenuLabels[c].substr(pos + 1);

        }
      } else if (dsc.status[partition] == 0xA0) { //bypass
        if (currentSelection == 0xFF || partitionStatus[partition].lastStatus != dsc.status[partition])
          currentSelection = getNextEnabledZone(0xFF, partition + 1);
        if (currentSelection < MAXZONES) {
          char s[16];
          char bypassStatus = ' ';
          if (zoneStatus[currentSelection].bypassed)
            bypassStatus = 'B';
          else if (zoneStatus[currentSelection].open)
            bypassStatus = 'O';
          sprintf(s, "%02d   %c", currentSelection + 1, bypassStatus);
          lcdLine2 = s;
        }
      } else if (dsc.status[partition] == 0x11) { //alarms

        if (currentSelection == 0xFF)
          currentSelection = getNextAlarmedZone(0xFF, partition + 1);
        if (currentSelection < MAXZONES) {
          char s[16];
          sprintf(s, "zone %02d", currentSelection + 1);
          lcdLine2 = s;
        }
      } else if (dsc.status[partition] == 0xA2) { //alarm memory

        if (currentSelection == 0xFF || partitionStatus[partition].lastStatus != dsc.status[partition])
          currentSelection = getNextOption(0xFF);

        if (currentSelection < MAXZONES) {
          char s[16];
          lcdLine2 = "";
          char bypassStatus = ' ';

          sprintf(s, "zone %02d", currentSelection);
          lcdLine2 = s;
        }
      } else if (dsc.status[partition] == 0x9E) { // main menu
        if (currentSelection == 0xFF || partitionStatus[partition].lastStatus != dsc.status[partition]) {
          currentSelection = 1;
          lcdLine2 = mainMenu[currentSelection];
        }
      } else if (dsc.status[partition] == 0xA9 && !partitionStatus[partition].eventViewer) { // user menu
        if (currentSelection == 0xFF || partitionStatus[partition].lastStatus != dsc.status[partition]) {
          currentSelection = 1;
          lcdLine2 = userMenu[currentSelection];
        }
      } else if (dsc.status[partition] == 0xA1) { //trouble

        if (currentSelection == 0xFF || partitionStatus[partition].lastStatus != dsc.status[partition]) {
          currentSelection = getNextOption(currentSelection);
          if (currentSelection < 9) {
            lcdLine2 = troubleMenu[currentSelection];
          }
        }
      } else if (dsc.status[partition] == 0xC8) { //service

        currentSelection = getNextOption(currentSelection);
        if (currentSelection < 9) {
          lcdLine2 = serviceMenu[currentSelection];
        }

      } else if (dsc.status[partition] == 0xA6) { //user code
        if (currentSelection > 95)
          currentSelection = getNextUserCode(currentSelection);
        if (currentSelection < 96) {
          char s[16];
          char programmed = ' ';
          if (checkUserCode(currentSelection))
            programmed = 'P';
          sprintf(s, "%02d   %c", currentSelection, programmed);
          lcdLine2 = s;
        }
      }

      if (options) {
        lcdLine2 = getOptionsString();
      }
      /* else if (line2Status == dsc.status[partition] && partitionStatus[partition].digits > 0) {
              char s[18];
              if (hex) {
                sprintf(s, "[%X]  %d partitionStatus[partition].digits", line2Digit, partitionStatus[partition].digits);
              } else {
                sprintf(s, "[%d] %d partitionStatus[partition].digits", line2Digit, partitionStatus[partition].digits);
              }
              lcdLine2=s;
              line2Status = 0;
              line2Digit = 0;

            }*/
    }

    if (lcdLine1 != "") line1DisplayCallback(lcdLine1, partition + 1);
    if (lcdLine2 != "") line2DisplayCallback(lcdLine2, partition + 1);

    //rtrim(lcdLine1);
    //rtrim(lcdLine2);
    //lcdLine1 = lcdLine1.append(" ").append(lcdLine2);
    // partitionMsgChangeCallback(partition + 1, lcdLine1);
    partitionStatus[partition].lastStatus = dsc.status[partition];
  }

  // Processes status data not natively handled within the library
  void processStatus() {
    #ifndef dscClassicSeries
    byte partition = 0;
    switch (dsc.panelData[0]) {

    case 0x0F:
    case 0x0A:
      //capture potential program digit if in programming mode
      //line2Digit = dsc.panelData[4];
      //line2Status = dsc.panelData[3];
      //dsc.statusChanged = true;
      processProgramZones(4); //bypass zones ?
      // }
      break;
    case 0x5D:
    case 0x63:

      if ((dsc.panelData[2] & 0x04) == 0x04) { // Alarm memory zones 1-32
        processProgramZones(3);
      }
      break;

    case 0xA5:
      if ((dsc.panelData[5] & 0x03) == 1)
        switch (dsc.panelData[6]) {
        case 0xAD:
          //ESP_LOGD("test", "A5 enter programming mode");
          break;
        case 0xAC:
          //ESP_LOGD("test", "A5 leave programming mode");
          break;
        }
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
      case 0x20:
      case 0x21:
        processProgramZones(5);
        break; // Programming zone lights 33-64 //bypass?
      case 0x18:
        //ESP_LOGD("info", "zone lights 33");
        if ((dsc.panelData[4] & 0x04) == 0x04)
          processProgramZones(5);
        break; // Alarm memory zones 33-64
      };
      break;
    case 0xEB:
      if (dsc.panelData[7] == 1)
        switch (dsc.panelData[8]) {
        case 0xAD:
          // ESP_LOGD("test", "EB enter programming mode");
          break;
        case 0xAC:
          //ESP_LOGD("test", "EB leave programming mode");
          break;
        };
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

  void printBeeps(byte panelByte, byte partition = -1) {
    if (partition < 1) partition = defaultPartition;
    dsc.statusChanged = true;
    beeps = dsc.panelData[panelByte] / 2;
    char s[4];
    sprintf(s, "%d", beeps);
    beepsCallback(s, partition);
    beepTime = millis();
    if (beeps == 2 && partitionStatus[partition - 1].digits)
      dsc.setLCDReceive(partitionStatus[partition - 1].digits, partition);
    partitionStatus[partition - 1].editIdx = 0;

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
      if (group1msg != "") lightsCallback(group1msg, dsc.pgmBuffer.partition - 1);
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
    ESP_LOGD("test", "program zones: ");
    lightsCallback(group1msg, defaultPartition);

  }

  void processEventBufferAA() {
    #ifndef dscClassicSeries
    if (extendedBuffer) return; // Skips 0xAA data when 0xEC extended event buffer data is available

    char eventInfo[45] = "Event: ";
    char charBuffer[4];
    itoa(dsc.panelData[7], charBuffer, 10);
    if (dsc.panelData[7] < 10) strcat(eventInfo, "00");
    else if (dsc.panelData[7] < 100) strcat(eventInfo, "0");
    strcat(eventInfo, charBuffer);
    strcat(eventInfo, " | ");

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
    byte partition = dsc.panelData[3] >> 6;
    // strcat(eventInfo, " | Partition ");
    // itoa(partition, charBuffer, 10);
    strcat(eventInfo, charBuffer);

    line1DisplayCallback(eventInfo, partition);
    eventInfoCallback(eventInfo, partition);

    switch (dsc.panelData[5] & 0x03) {
    case 0x00:
      printPanelStatus0(6, partition);
      break;
    case 0x01:
      printPanelStatus1(6, partition);
      break;
    case 0x02:
      printPanelStatus2(6, partition);
      break;
    case 0x03:
      printPanelStatus3(6, partition);
      break;
    }
    #endif
  }

  void processEventBufferEC() {
    #ifndef dscClassicSeries
    if (!extendedBuffer) extendedBuffer = true;

    char eventInfo[45] = "Event: ";
    char charBuffer[4];
    int eventNumber = dsc.panelData[9] + ((dsc.panelData[4] >> 6) * 256);
    itoa(eventNumber, charBuffer, 10);
    if (eventNumber < 10) strcat(eventInfo, "00");
    else if (eventNumber < 100) strcat(eventInfo, "0");
    strcat(eventInfo, charBuffer);
    strcat(eventInfo, " ");

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
      // strcat(eventInfo, " | Partition ");

      byte bitCount = 0;
      for (byte bit = 0; bit <= 7; bit++) {
        if (bitRead(dsc.panelData[2], bit)) {
          itoa((bitCount + 1), charBuffer, 10);
          line1DisplayCallback(eventInfo, bitCount + 1);
          eventInfoCallback(eventInfo, bitCount + 1);
          ESP_LOGD("info", "EC partition=%d,switch=%02X,event=%s", bitCount + 1, dsc.panelData[7], eventInfo);
          switch (dsc.panelData[7]) {
          case 0x00:
            printPanelStatus0(8, bitCount + 1);
            break;
          case 0x01:
            printPanelStatus1(8, bitCount + 1);
            break;
          case 0x02:
            printPanelStatus2(8, bitCount + 1);
            break;
          case 0x03:
            printPanelStatus3(8, bitCount + 1);
            break;
          case 0x04:
            printPanelStatus4(8, bitCount + 1);
            break;
          case 0x05:
            printPanelStatus5(8, bitCount + 1);
            break;
          case 0x14:
            printPanelStatus14(8, bitCount + 1);
            break;
          case 0x16:
            printPanelStatus16(8, bitCount + 1);
            break;
          case 0x17:
            printPanelStatus17(8, bitCount + 1);
            break;
          case 0x18:
            printPanelStatus18(8, bitCount + 1);
            break;
          case 0x1B:
            printPanelStatus1B(8, bitCount + 1);
            break;
          }

        }
        bitCount++;
      }
      //strcat(eventInfo, charBuffer);
    } else {
      line1DisplayCallback(eventInfo, defaultPartition);
      eventInfoCallback(eventInfo, defaultPartition);

      switch (dsc.panelData[7]) {
      case 0x00:
        printPanelStatus0(8, defaultPartition);
        break;
      case 0x01:
        printPanelStatus1(8, defaultPartition);
        break;
      case 0x02:
        printPanelStatus2(8, defaultPartition);
        break;
      case 0x03:
        printPanelStatus3(8, defaultPartition);
        break;
      case 0x04:
        printPanelStatus4(8, defaultPartition);
        break;
      case 0x05:
        printPanelStatus5(8, defaultPartition);
        break;
      case 0x14:
        printPanelStatus14(8, defaultPartition);
        break;
      case 0x16:
        printPanelStatus16(8, defaultPartition);
        break;
      case 0x17:
        printPanelStatus17(8, defaultPartition);
        break;
      case 0x18:
        printPanelStatus18(8, defaultPartition);
        break;
      case 0x1B:
        printPanelStatus1B(8, defaultPartition);
        break;
      }
    }
    #endif
  }

  void printPanelStatus0(byte panelByte, byte partition) {
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
      lcdLine2 = "Alarm memory";
      break;
    case 0x4B:
      lcdLine1 = "Recent";
      lcdLine2 = "closing alarm";
      break;
    case 0x4C:
      lcdLine1 = "Zone expander";
      lcdLine2 = "suprvis. alarm";
      break;
    case 0x4D:
      lcdLine1 = "Zone expander";
      lcdLine2 = "suprvis. restore";
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
      lcdLine1 = "Auxiliary input";
      lcdLine2 = "alarm";
      break;
    case 0x52:
      lcdLine1 = "Keypad Fire";
      lcdLine2 = "alarm restored";
      break;
    case 0x53:
      lcdLine1 = "Keypad Aux";
      lcdLine2 = "alarm restored";
      break;
    case 0x54:
      lcdLine1 = "Keypad Panic";
      lcdLine2 = "alarm restored";
      break;
    case 0x55:
      lcdLine1 = "Auxiliary input";
      lcdLine2 = "alarm restored";
      break;
    case 0x98:
      lcdLine1 = "Keypad";
      lcdLine2 = "lockout";
      break;
    case 0xBE:
      lcdLine1 = "Armed:";
      lcdLine2 = "Partial";
      break;
    case 0xBF:
      lcdLine1 = "Armed:";
      lcdLine2 = "Special";
      break;
    case 0xE5:
      lcdLine1 = "Auto-arm";
      lcdLine2 = "cancelled";
      break;
    case 0xE6:
      lcdLine1 = "Disarmed:";
      lcdLine2 = "Special";
      break;
    case 0xE7:
      lcdLine1 = "Panel battery";
      lcdLine2 = "trouble";
      break;
    case 0xE8:
      lcdLine1 = "Panel AC power";
      lcdLine2 = "trouble";
      break;
    case 0xE9:
      lcdLine1 = "Bell trouble";
      lcdLine2 = " ";
      break;
    case 0xEA:
      lcdLine1 = "Fire zone";
      lcdLine2 = "trouble";
      break;
    case 0xEB:
      lcdLine1 = "Panel aux supply";
      lcdLine2 = "trouble";
      break;
    case 0xEC:
      lcdLine1 = "Telephone line";
      lcdLine2 = "trouble";
      break;
    case 0xEF:
      lcdLine1 = "Panel battery";
      lcdLine2 = "restored";
      break;
    case 0xF0:
      lcdLine1 = "Panel AC power";
      lcdLine2 = "restored";
      break;
    case 0xF1:
      lcdLine1 = "Bell restored";
      lcdLine2 = " ";
      break;
    case 0xF2:
      lcdLine1 = "Fire zone";
      lcdLine2 = "trouble restored";
      break;
    case 0xF3:
      lcdLine1 = "Panel aux supply";
      lcdLine2 = "restored";
      break;
    case 0xF4:
      lcdLine1 = "Telephone line";
      lcdLine2 = "restored";
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
      lcdLine2 = "transmission";
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
      strcpy(lcdMessage, "Zone alarm: ");
      itoa(dsc.panelData[panelByte] - 8, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine1 = lcdMessage;
      lcdLine2 = " ";
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0x29 && dsc.panelData[panelByte] <= 0x48) {
      lcdLine1 = "Zone alarm";
      strcpy(lcdMessage, "restored: ");
      itoa(dsc.panelData[panelByte] - 40, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0x56 && dsc.panelData[panelByte] <= 0x75) {
      strcpy(lcdMessage, "Zone tamper: ");
      itoa(dsc.panelData[panelByte] - 85, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine1 = lcdMessage;
      lcdLine2 = " ";
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0x76 && dsc.panelData[panelByte] <= 0x95) {
      lcdLine1 = "Zone tamper";
      strcpy(lcdMessage, "restored: ");
      itoa(dsc.panelData[panelByte] - 117, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0x99 && dsc.panelData[panelByte] <= 0xBD) {
      lcdLine1 = "Armed:";
      byte dscCode = dsc.panelData[panelByte] - 0x98;
      if (dscCode >= 35) dscCode += 5;
      if (dscCode == 40) strcpy(lcdMessage, "Master code ");
      else strcpy(lcdMessage, "Access code ");
      itoa(dscCode, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0xC0 && dsc.panelData[panelByte] <= 0xE4) {
      lcdLine1 = "Disarmed:";
      byte dscCode = dsc.panelData[panelByte] - 0xBF;
      if (dscCode >= 35) dscCode += 5;
      if (dscCode == 40) strcpy(lcdMessage, "Master code ");
      else strcpy(lcdMessage, "Access code ");
      itoa(dscCode, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (!decoded) {
      lcdLine1 = "Unknown data";
      lcdLine2 = " ";
    }

    //partitionStatus[partition].lcdline1=lcdLine1;
    // partitionStatus[partition].lcdline2=lcdLine2;
    line2DisplayCallback(lcdLine1.append(" ").append(lcdLine2), partition);
    // if (lcdLine1 != "") line1DisplayCallback(lcdLine1,partition);
    // if (lcdLine2 != "") line2DisplayCallback(lcdLine2,partition);
  }

  void printPanelStatus1(byte panelByte, byte partition) {
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
    case 0x29:
      lcdLine1 = "Downloading";
      lcdLine2 = "forced answer";
      break;
    case 0x2B:
      lcdLine1 = "Armed:";
      lcdLine2 = "Auto-arm";
      break;
    case 0xAC:
      lcdLine1 = "Exit installer";
      lcdLine2 = "programming";
      break;
    case 0xAD:
      lcdLine1 = "Enter installer";
      lcdLine2 = "programming";
      break;
    case 0xAE:
      lcdLine1 = "Walk test";
      lcdLine2 = "end";
      break;
    case 0xAF:
      lcdLine1 = "Walk test";
      lcdLine2 = "begin";
      break;
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
      lcdLine1 = "Downlook remote";
      lcdLine2 = "trigger";
      break;
    default:
      decoded = false;
    }

    char lcdMessage[20];
    char charBuffer[4];
    if (dsc.panelData[panelByte] >= 0x24 && dsc.panelData[panelByte] <= 0x28) {
      byte dscCode = dsc.panelData[panelByte] - 0x03;
      if (dscCode >= 35) dscCode += 5;
      if (dscCode == 40) strcpy(lcdMessage, "Master code ");
      else strcpy(lcdMessage, "Access code ");
      itoa(dscCode, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine1 = lcdMessage;
      lcdLine2 = " ";
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0x2C && dsc.panelData[panelByte] <= 0x4B) {
      lcdLine1 = "Zone battery";
      strcpy(lcdMessage, "restored: ");
      itoa(dsc.panelData[panelByte] - 43, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0x4C && dsc.panelData[panelByte] <= 0x6B) {
      lcdLine1 = "Zone battery";
      strcpy(lcdMessage, "low: ");
      itoa(dsc.panelData[panelByte] - 75, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0x6C && dsc.panelData[panelByte] <= 0x8B) {
      lcdLine1 = "Zone fault";
      strcpy(lcdMessage, "restored: ");
      itoa(dsc.panelData[panelByte] - 107, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0x8C && dsc.panelData[panelByte] <= 0xAB) {
      strcpy(lcdMessage, "Zone fault: ");
      itoa(dsc.panelData[panelByte] - 139, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine1 = lcdMessage;
      lcdLine2 = " ";
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0xB0 && dsc.panelData[panelByte] <= 0xCF) {
      strcpy(lcdMessage, "Zone bypass: ");
      itoa(dsc.panelData[panelByte] - 175, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine1 = lcdMessage;
      lcdLine2 = " ";
      decoded = true;
    }

    if (!decoded) {
      lcdLine1 = "Unknown data";
      lcdLine2 = " ";
    }
    // partitionStatus[partition].lcdline1=lcdLine1;
    // partitionStatus[partition].lcdline2=lcdLine2;  
    line2DisplayCallback(lcdLine1.append(" ").append(lcdLine2), partition);
    //if (lcdLine1 != "" && dsc.status[partition] != 0xA9) line1DisplayCallback(lcdLine1,partition);
    // if (lcdLine2 != "") line2DisplayCallback(lcdLine2,partition);
  }

  void printPanelStatus2(byte panelByte, byte partition) {
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
      lcdLine2 = "restored";
      break;
    case 0x64:
      lcdLine1 = "Keybus fault";
      lcdLine2 = " ";
      break;
    case 0x66:
      lcdLine1 = "*1: Zone bypass";
      lcdLine2 = "programming";
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
      lcdLine2 = "No entry delay";
      break;
    case 0xFF:
      lcdLine1 = "Zone expander";
      lcdLine2 = "trouble: 1";
      break;
    default:
      decoded = false;
    }

    char lcdMessage[20];
    char charBuffer[4];

    if (dsc.panelData[panelByte] >= 0x9E && dsc.panelData[panelByte] <= 0xC2) {
      byte dscCode = dsc.panelData[panelByte] - 0x9D;
      lcdLine1 = "*1: ";
      if (dscCode >= 35) dscCode += 5;
      if (dscCode == 40) strcpy(lcdMessage, "Master code ");
      else strcpy(lcdMessage, "Access code ");
      itoa(dscCode, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0xC3 && dsc.panelData[panelByte] <= 0xC5) {
      byte dscCode = dsc.panelData[panelByte] - 0xA0;
      lcdLine1 = "*5: ";
      if (dscCode >= 35) dscCode += 5;
      if (dscCode == 40) strcpy(lcdMessage, "Master code ");
      else strcpy(lcdMessage, "Access code ");
      itoa(dscCode, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0xC6 && dsc.panelData[panelByte] <= 0xE5) {
      byte dscCode = dsc.panelData[panelByte] - 0xC5;
      if (dscCode >= 35) dscCode += 5;
      if (dscCode == 40) strcpy(lcdMessage, "Master code ");
      else strcpy(lcdMessage, "Access code ");
      itoa(dscCode, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine1 = lcdMessage;
      lcdLine2 = " ";
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0xE6 && dsc.panelData[panelByte] <= 0xE8) {
      byte dscCode = dsc.panelData[panelByte] - 0xC3;
      lcdLine1 = "";
      if (dscCode >= 35) dscCode += 5;
      if (dscCode == 40) strcpy(lcdMessage, "Master code ");
      else strcpy(lcdMessage, "Access code ");
      itoa(dscCode, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0xE9 && dsc.panelData[panelByte] <= 0xF0) {
      lcdLine1 = "Keypad restored:";
      strcpy(lcdMessage, "Slot ");
      itoa(dsc.panelData[panelByte] - 232, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0xF1 && dsc.panelData[panelByte] <= 0xF8) {
      lcdLine1 = "Keypad trouble:";
      strcpy(lcdMessage, "Slot ");
      itoa(dsc.panelData[panelByte] - 240, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0xF9 && dsc.panelData[panelByte] <= 0xFE) {
      strcpy(lcdMessage, "Zone expander ");
      itoa(dsc.panelData[panelByte] - 248, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine1 = lcdMessage;
      lcdLine2 = "restored";
      decoded = true;
    }

    if (!decoded) {
      lcdLine1 = "Unknown data";
      lcdLine2 = " ";
    }
    // partitionStatus[partition].lcdline1=lcdLine1;
    // partitionStatus[partition].lcdline2=lcdLine2;
    //ESP_LOGD("test","lcdline1=%s,lcdline2=%s",partitionStatus[partition].lcdline1.c_str(),partitionStatus[partition].lcdline2.c_str());
    //if (lcdLine1 != "" && dsc.status[partition] != 0xA9) line1DisplayCallback(lcdLine1,partition);
    //if (lcdLine2 != "") line2DisplayCallback(lcdLine2,partition);
    line2DisplayCallback(lcdLine1.append(" ").append(lcdLine2), partition);
  }

  void printPanelStatus3(byte panelByte, byte partition) {
    bool decoded = true;
    std::string lcdLine1;
    std::string lcdLine2;
    switch (dsc.panelData[panelByte]) {
    case 0x05:
      lcdLine1 = "PC/RF5132:";
      lcdLine2 = "Suprvis. restore";
      break;
    case 0x06:
      lcdLine1 = "PC/RF5132:";
      lcdLine2 = "Suprvis. trouble";
      break;
    case 0x09:
      lcdLine1 = "PC5204:";
      lcdLine2 = "Suprvis. restore";
      break;
    case 0x0A:
      lcdLine1 = "PC5204:";
      lcdLine2 = "Suprvis. trouble";
      break;
    case 0x17:
      lcdLine1 = "Zone expander 7";
      lcdLine2 = "restored";
      break;
    case 0x18:
      lcdLine1 = "Zone expander 7";
      lcdLine2 = "trouble";
      break;
    case 0x41:
      lcdLine1 = "PC/RF5132:";
      lcdLine2 = "Tamper restored";
      break;
    case 0x42:
      lcdLine1 = "PC/RF5132: Tamper";
      lcdLine2 = " ";
      break;
    case 0x43:
      lcdLine1 = "PC5208: Tamper";
      lcdLine2 = "restored";
      break;
    case 0x44:
      lcdLine1 = "PC5208: Tamper";
      lcdLine2 = " ";
      break;
    case 0x45:
      lcdLine1 = "PC5204: Tamper";
      lcdLine2 = "restored";
      break;
    case 0x46:
      lcdLine1 = "PC5204: Tamper";
      lcdLine2 = " ";
      break;
    case 0x51:
      lcdLine1 = "Zone expander 7";
      lcdLine2 = "tamper restored";
      break;
    case 0x52:
      lcdLine1 = "Zone expander 7";
      lcdLine2 = "tamper";
      break;
    case 0xB3:
      lcdLine1 = "PC5204:";
      lcdLine2 = "Battery restored";
      break;
    case 0xB4:
      lcdLine1 = "PC5204:";
      lcdLine2 = "Battery trouble";
      break;
    case 0xB5:
      lcdLine1 = "PC5204: Aux";
      lcdLine2 = "supply restored";
      break;
    case 0xB6:
      lcdLine1 = "PC5204: Aux";
      lcdLine2 = "supply trouble";
      break;
    case 0xB7:
      lcdLine1 = "PC5204: Output 1";
      lcdLine2 = "restored";
      break;
    case 0xB8:
      lcdLine1 = "PC5204: Output 1";
      lcdLine2 = "trouble";
      break;
    case 0xFF:
      lcdLine1 = "Extended status";
      lcdLine2 = " ";
      break;
    default:
      decoded = false;
    }

    char lcdMessage[20];
    char charBuffer[4];

    if (dsc.panelData[panelByte] <= 0x04) {
      strcpy(lcdMessage, "Zone expander ");
      itoa(dsc.panelData[panelByte] + 2, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine1 = lcdMessage;
      lcdLine2 = "trouble";
      return;
    }

    if (dsc.panelData[panelByte] >= 0x35 && dsc.panelData[panelByte] <= 0x3A) {
      strcpy(lcdMessage, "Zone expander ");
      itoa(dsc.panelData[panelByte] - 52, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine1 = lcdMessage;
      lcdLine2 = "tamper restored";
      return;
    }

    if (dsc.panelData[panelByte] >= 0x3B && dsc.panelData[panelByte] <= 0x40) {
      strcpy(lcdMessage, "Zone expander ");
      itoa(dsc.panelData[panelByte] - 58, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine1 = lcdMessage;
      lcdLine2 = "tamper";
      return;
    }

    if (!decoded) {
      lcdLine1 = "Unknown data";
      lcdLine2 = " ";
    }
    // partitionStatus[partition].lcdline1=lcdLine1;
    // partitionStatus[partition].lcdline2=lcdLine2; 
    line2DisplayCallback(lcdLine1.append(" ").append(lcdLine2), partition);
    //if (lcdLine1 != "" && dsc.status[partition] != 0xA9) line1DisplayCallback(lcdLine1,partition);
    // if (lcdLine2 != "") line2DisplayCallback(lcdLine2,partition);
  }

  void printPanelStatus4(byte panelByte, byte partition) {
    bool decoded = true;
    std::string lcdLine1;
    std::string lcdLine2;
    switch (dsc.panelData[panelByte]) {
    case 0x86:
      lcdLine1 = "Periodic test";
      lcdLine2 = "with trouble";
      break;
    case 0x87:
      lcdLine1 = "Exit fault";
      lcdLine2 = " ";
      break;
    case 0x89:
      lcdLine1 = "Alarm cancelled";
      lcdLine2 = " ";
      break;
    default:
      decoded = false;
    }

    char lcdMessage[20];
    char charBuffer[4];

    if (dsc.panelData[panelByte] <= 0x1F) {
      strcpy(lcdMessage, "Zone alarm: ");
      itoa(dsc.panelData[panelByte] + 33, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine1 = lcdMessage;
      lcdLine2 = " ";
      decoded = true;
    } else if (dsc.panelData[panelByte] >= 0x20 && dsc.panelData[panelByte] <= 0x3F) {
      lcdLine1 = "Zone alarm";
      strcpy(lcdMessage, "restored: ");
      itoa(dsc.panelData[panelByte] + 1, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine2 = lcdMessage;
      decoded = true;
    } else if (dsc.panelData[panelByte] >= 0x40 && dsc.panelData[panelByte] <= 0x5F) {
      strcpy(lcdMessage, "Zone tamper: ");
      itoa(dsc.panelData[panelByte] - 31, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine1 = lcdMessage;
      lcdLine2 = " ";
      decoded = true;
    } else if (dsc.panelData[panelByte] >= 0x60 && dsc.panelData[panelByte] <= 0x7F) {
      lcdLine1 = "Zone tamper";
      strcpy(lcdMessage, "restored: ");
      itoa(dsc.panelData[panelByte] - 63, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (!decoded) {
      lcdLine1 = "Unknown data";
      lcdLine2 = " ";
    }
    //partitionStatus[partition].lcdline1=lcdLine1;
    // partitionStatus[partition].lcdline2=lcdLine2;
    line2DisplayCallback(lcdLine1.append(" ").append(lcdLine2), partition);
    // if (lcdLine1 != "" && dsc.status[partition] != 0xA9) line1DisplayCallback(lcdLine1,partition);
    // if (lcdLine2 != "") line2DisplayCallback(lcdLine2,partition);
  }

  void printPanelStatus5(byte panelByte, byte partition) {
    bool decoded = true;

    std::string lcdLine1;
    std::string lcdLine2;
    char lcdMessage[20];
    char charBuffer[4];

    if (dsc.panelData[panelByte] <= 0x39) {
      byte dscCode = dsc.panelData[panelByte] + 0x23;
      lcdLine1 = "Armed: ";
      if (dscCode >= 40) dscCode += 3;
      strcpy(lcdMessage, "Access code ");
      itoa(dscCode, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0x3A && dsc.panelData[panelByte] <= 0x73) {
      byte dscCode = dsc.panelData[panelByte] - 0x17;
      lcdLine1 = "Disarmed: ";
      if (dscCode >= 40) dscCode += 3;
      strcpy(lcdMessage, "Access code ");
      itoa(dscCode, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (!decoded) {
      lcdLine1 = "Unknown data";
      lcdLine2 = " ";
    }
    //partitionStatus[partition].lcdline1=lcdLine1;
    //partitionStatus[partition].lcdline2=lcdLine2;
    line2DisplayCallback(lcdLine1.append(" ").append(lcdLine2), partition);
    //if (lcdLine1 != "" && dsc.status[partition] != 0xA9) line1DisplayCallback(lcdLine1,partition);
    // if (lcdLine2 != "") line2DisplayCallback(lcdLine2,partition);
  }

  void printPanelStatus14(byte panelByte, byte partition) {
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
      lcdLine2 = "network fault";
      break;
    case 0xC4:
      lcdLine1 = "TLink receiver";
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
      lcdLine1 = "Unknown data";
      lcdLine2 = " ";
    }
    //  partitionStatus[partition].lcdline1=lcdLine1;
    //partitionStatus[partition].lcdline2=lcdLine2;   
    line2DisplayCallback(lcdLine1.append(" ").append(lcdLine2), partition);
    //if (lcdLine1 != "" && dsc.status[partition] != 0xA9) line1DisplayCallback(lcdLine1,partition);
    //if (lcdLine2 != "") line2DisplayCallback(lcdLine2,partition);
  }

  void printPanelStatus16(byte panelByte, byte partition) {
    bool decoded = true;
    std::string lcdLine1;
    std::string lcdLine2;

    switch (dsc.panelData[panelByte]) {
    case 0x80:
      lcdLine1 = "Trouble";
      lcdLine2 = "acknowledged";
      break;
    case 0x81:
      lcdLine1 = "RF delinquency";
      lcdLine2 = "trouble";
      break;
    case 0x82:
      lcdLine1 = "RF delinquency";
      lcdLine2 = "restore";
      break;
    default:
      decoded = false;
    }

    if (!decoded) {
      lcdLine1 = "Unknown data";
      lcdLine2 = " ";
    }
    // partitionStatus[partition].lcdline1=lcdLine1;
    // partitionStatus[partition].lcdline2=lcdLine2;    
    line2DisplayCallback(lcdLine1.append(" ").append(lcdLine2), partition);
    // if (lcdLine1 != "" && dsc.status[partition] != 0xA9) line1DisplayCallback(lcdLine1,partition);
    //if (lcdLine2 != "") line2DisplayCallback(lcdLine2,partition);
  }

  void printPanelStatus17(byte panelByte, byte partition) {
    bool decoded = true;
    std::string lcdLine1;
    std::string lcdLine2;
    char lcdMessage[20];
    char charBuffer[4];

    if (dsc.panelData[panelByte] >= 0x4A && dsc.panelData[panelByte] <= 0x83) {
      byte dscCode = dsc.panelData[panelByte] - 0x27;
      lcdLine1 = "*1: ";
      if (dscCode >= 40) dscCode += 3;
      strcpy(lcdMessage, "Access code ");
      itoa(dscCode, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (dsc.panelData[panelByte] <= 0x24) {
      byte dscCode = dsc.panelData[panelByte] + 1;
      lcdLine1 = "*2: ";
      if (dscCode >= 40) dscCode += 3;
      strcpy(lcdMessage, "Access code ");
      itoa(dscCode, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0x84 && dsc.panelData[panelByte] <= 0xBD) {
      byte dscCode = dsc.panelData[panelByte] - 0x61;
      lcdLine1 = "*2: ";
      if (dscCode >= 40) dscCode += 3;
      strcpy(lcdMessage, "Access code ");
      itoa(dscCode, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0x25 && dsc.panelData[panelByte] <= 0x49) {
      byte dscCode = dsc.panelData[panelByte] - 0x24;
      lcdLine1 = "*3: ";
      if (dscCode >= 40) dscCode += 3;
      strcpy(lcdMessage, "Access code ");
      itoa(dscCode, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0xBE && dsc.panelData[panelByte] <= 0xF7) {
      byte dscCode = dsc.panelData[panelByte] - 0x9B;
      lcdLine1 = "*3: ";
      if (dscCode >= 40) dscCode += 3;
      strcpy(lcdMessage, "Access code ");
      itoa(dscCode, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (!decoded) {
      lcdLine1 = "Unknown data";
      lcdLine2 = " ";
    }
    //partitionStatus[partition].lcdline1=lcdLine1;
    //partitionStatus[partition].lcdline2=lcdLine2;    
    line2DisplayCallback(lcdLine1.append(" ").append(lcdLine2), partition);
    //if (lcdLine1 != "" && dsc.status[partition] != 0xA9) line1DisplayCallback(lcdLine1,partition);
    //if (lcdLine2 != "") line2DisplayCallback(lcdLine2,partition);

  }

  void printPanelStatus18(byte panelByte, byte partition) {
    bool decoded = true;

    char lcdMessage[20];
    char charBuffer[4];
    std::string lcdLine1;
    std::string lcdLine2;
    if (dsc.panelData[panelByte] <= 0x39) {
      byte dscCode = dsc.panelData[panelByte] + 0x23;
      if (dscCode >= 40) dscCode += 3;
      strcpy(lcdMessage, "Access code ");
      itoa(dscCode, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine1 = lcdMessage;
      lcdLine2 = " ";
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0x3A && dsc.panelData[panelByte] <= 0x95) {
      byte dscCode = dsc.panelData[panelByte] - 0x39;
      lcdLine1 = "*5: ";
      if (dscCode >= 40) dscCode += 3;
      strcpy(lcdMessage, "Access code ");
      itoa(dscCode, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (dsc.panelData[panelByte] >= 0x96 && dsc.panelData[panelByte] <= 0xF1) {
      byte dscCode = dsc.panelData[panelByte] - 0x95;
      lcdLine1 = "*6: ";
      if (dscCode >= 40) dscCode += 3;
      strcpy(lcdMessage, "Access code ");
      itoa(dscCode, charBuffer, 10);
      strcat(lcdMessage, charBuffer);
      lcdLine2 = lcdMessage;
      decoded = true;
    }

    if (!decoded) {
      lcdLine1 = "Unknown data";
      lcdLine2 = " ";
    }
    //partitionStatus[partition].lcdline1=lcdLine1;
    //partitionStatus[partition].lcdline2=lcdLine2;    
    line2DisplayCallback(lcdLine1.append(" ").append(lcdLine2), partition);
    // if (lcdLine1 != "" && dsc.status[partition] != 0xA9) line1DisplayCallback(lcdLine1,partition);
    // if (lcdLine2 != "") line2DisplayCallback(lcdLine2,partition);
  }

  void printPanelStatus1B(byte panelByte, byte partition) {
    bool decoded = true;
    std::string lcdLine1;
    std::string lcdLine2;
    switch (dsc.panelData[panelByte]) {
    case 0xF1:
      lcdLine1 = "System reset";
      lcdLine2 = "transmission";
      break;
    default:
      decoded = false;
    }

    if (!decoded) {
      lcdLine1 = "Unknown data";
      lcdLine2 = " ";
    }
    //partitionStatus[partition].lcdline1=lcdLine1;
    // partitionStatus[partition].lcdline2=lcdLine2;
    line2DisplayCallback(lcdLine1.append(" ").append(lcdLine2), partition);
    //  line1DisplayCallback(lcdLine1,partition);
    //  line2DisplayCallback(lcdLine2,partition);
  }

};

#endif