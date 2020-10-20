#ifndef dscalarm_h
#define dscalarm_h
#include "esphome.h"
#include "dscKeybusInterface.h"
//for documentation see project at https://github.com/Dilbert66/esphome-dsckeybus

#define dscClockPin D1  // esp8266: D1, D2, D8 (GPIO 5, 4, 15)
#define dscReadPin D2   // esp8266: D1, D2, D8 (GPIO 5, 4, 15)
#define dscWritePin D8  // esp8266: D1, D2, D8 (GPIO 5, 4, 15)
#define MAXZONES 32 //set to 64 if your system supports it
#define MODULESUPERVISION 0   //only enable this option if you want your virtual modules to be supervised by the panel and show errors if missing.  Not needed for operation.

dscKeybusInterface dsc(dscClockPin, dscReadPin, dscWritePin);
bool forceDisconnect;

void disconnectKeybus() {
  dsc.stop();
  dsc.keybusConnected = false;
  dsc.statusChanged = false;
  forceDisconnect = true;
 
}
enum troubleStatus {acStatus,batStatus,trStatus,fireStatus,panicStatus};

class DSCkeybushome : public PollingComponent, public CustomAPIDevice {
 public:
   DSCkeybushome( const char *accessCode="",  unsigned long cmdWaitTime=0)
   : accessCode(accessCode)
   , cmdWaitTime(cmdWaitTime)
  {}
 
  std::function<void (uint8_t, bool)> zoneStatusChangeCallback;
  std::function<void (std::string)> systemStatusChangeCallback;
  std::function<void (troubleStatus,bool)> troubleStatusChangeCallback;
  std::function<void (uint8_t, bool)> fireStatusChangeCallback;
  std::function<void (uint8_t,std::string)> partitionStatusChangeCallback; 
  std::function<void (uint8_t,std::string)> partitionMsgChangeCallback; 
  std::function<void (std::string)> zoneMsgStatusCallback; 
  std::function<void (std::string)> troubleMsgStatusCallback; 
  std::function<void (uint8_t,bool)> relayChannelChangeCallback;    
  

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
 
  void onZoneStatusChange(std::function<void (uint8_t zone, bool isOpen)> callback) { zoneStatusChangeCallback = callback; }
  void onSystemStatusChange(std::function<void (std::string status)> callback) { systemStatusChangeCallback = callback; }
  void onFireStatusChange(std::function<void (uint8_t partition, bool isOpen)> callback) { fireStatusChangeCallback = callback; }
  void onTroubleStatusChange(std::function<void (troubleStatus ts,bool isOpen)> callback) { troubleStatusChangeCallback = callback; }
  void onPartitionStatusChange(std::function<void (uint8_t partition,std::string status)> callback) { partitionStatusChangeCallback = callback; }
  void onPartitionMsgChange(std::function<void (uint8_t partition,std::string msg)> callback) { partitionMsgChangeCallback = callback; }
  void onZoneMsgStatus(std::function<void (std::string msg)> callback) { zoneMsgStatusCallback = callback; }
  void onTroubleMsgStatus(std::function<void (std::string msg)> callback) { troubleMsgStatusCallback = callback; }
  void onRelayChannelChange(std::function<void (uint8_t channel,bool state)> callback) { relayChannelChangeCallback = callback; }
  char expanderAddr1,expanderAddr2,expanderAddr3;
  byte debug;
  const char *accessCode;
  bool enable05Messages = true;
  unsigned long cmdWaitTime;
  
  private:
    uint8_t zone;
	byte lastStatus[dscPartitions];
    bool firstrun;
	
    struct zoneType {
        bool tamper;
        bool batteryLow;
        bool open;
        bool alarm;
    } ;
	
    zoneType zoneStatus[MAXZONES];
    std::string zoneStatusMsg,previousZoneStatusMsg,systemMsg,previousSystemMsg;  
    bool relayStatus[16],previousRelayStatus[16];
    bool sendCmd;
    byte system0,system1,previousSystem0,previousSystem1;
    
    
  void setup() override {
      if (debug > 2) 
        Serial.begin(115200);

	set_update_interval(10); 
    register_service(&DSCkeybushome::set_alarm_state,"set_alarm_state", {"partition","state","code"});
	register_service(&DSCkeybushome::alarm_disarm,"alarm_disarm",{"code"});
	register_service(&DSCkeybushome::alarm_arm_home,"alarm_arm_home");
	register_service(&DSCkeybushome::alarm_arm_night,"alarm_arm_night",{"code"});
	register_service(&DSCkeybushome::alarm_arm_away,"alarm_arm_away");
	register_service(&DSCkeybushome::alarm_trigger_panic,"alarm_trigger_panic");
	register_service(&DSCkeybushome::alarm_trigger_fire,"alarm_trigger_fire");
    register_service(&DSCkeybushome::alarm_keypress, "alarm_keypress",{"keys"});
    register_service(&DSCkeybushome::set_zone_fault,"set_zone_fault",{"zone","fault"});
    
    firstrun=true;
	systemStatusChangeCallback(STATUS_OFFLINE);
	forceDisconnect = false;
    dsc.enableModuleSupervision=MODULESUPERVISION;
    
	dsc.cmdWaitTime=cmdWaitTime;
    dsc.processModuleData = true;      // Controls if keypad and module data is processed and displayed (default: false)
	dsc.resetStatus();
    dsc.maxZones=MAXZONES;
    dsc.addModule(expanderAddr1);
    dsc.addModule(expanderAddr2);
    dsc.begin();
  }
  

void set_zone_fault (int zone, bool fault) {
	
	dsc.setZoneFault(zone,fault);
	
}


void alarm_disarm (std::string code) {
	
	set_alarm_state(1,"D",code);
	
}

void alarm_arm_home () {
	
	set_alarm_state(1,"S");
	
}

void alarm_arm_night (std::string code) {
	
	set_alarm_state(1,"N",code);
	
}

void alarm_arm_away () {
	
	set_alarm_state(1,"A");
	
}

void alarm_trigger_fire () {
	
	set_alarm_state(1,"F");
	
}

void alarm_trigger_panic () {
	
	set_alarm_state(1,"P");
	
}

 void alarm_keypress(std::string keystring) {
	  const char* keys =  strcpy(new char[keystring.length() +1],keystring.c_str());
	   if (debug > 0) ESP_LOGD("Debug","Writing keys: %s",keystring.c_str());
	   dsc.write(keys);
 }		

bool isInt(std::string s, int base){
   if(s.empty() || std::isspace(s[0])) return false ;
   char * p ;
   strtol(s.c_str(), &p, base) ;
   return (*p == 0) ;
}  
    

 void set_alarm_state(int partition,std::string state,std::string code="") {

	if (code.length() != 4 || !isInt(code,10) ) code=""; // ensure we get a numeric 4 digit code
	const char* alarmCode =  strcpy(new char[code.length() +1],code.c_str());
	if (partition) partition = partition-1; // adjust to 0-xx range

    // Arm stay
    if (state.compare("S") == 0 && !dsc.armed[partition] && !dsc.exitDelay[partition]) {
      dsc.writePartition = partition+1;         // Sets writes to the partition number
	  dsc.write('s');                             // Virtual keypad arm stay
    }
    // Arm away
    else if (state.compare("A") == 0 && !dsc.armed[partition] && !dsc.exitDelay[partition]) {
	  dsc.writePartition = partition+1;         // Sets writes to the partition number
      dsc.write('w');                             // Virtual keypad arm away
    }
	// Arm night  ** this depends on the accessCode setup in the yaml
	else if (state.compare("N") == 0 && !dsc.armed[partition] && !dsc.exitDelay[partition]) {
      //ensure you have the accessCode setup correctly in the yaml for this to work
      dsc.writePartition = partition+1;         // Sets writes to the partition number
      dsc.write('n');                             // Virtual keypad arm away
	  if (code.length() == 4 && !isInt(accessCode,10) ) { // if the code is sent and the yaml code is not active use this.
        dsc.write(alarmCode);
	  }
    }
	// Fire command
	else if (state.compare("F") == 0 ) {
      dsc.writePartition = partition+1;         // Sets writes to the partition number
      dsc.write('f');                             // Virtual keypad arm away
    }
	// Panic command
	else if (state.compare("P") == 0 ) {
      dsc.writePartition = partition+1;         // Sets writes to the partition number
      dsc.write('p');                             // Virtual keypad arm away
    }
    // Disarm
    else if (state.compare("D") == 0 && (dsc.armed[partition] || dsc.exitDelay[partition])) {
		dsc.writePartition = partition+1;         // Sets writes to the partition number
		if (code.length() == 4 ) { // ensure we get 4 digit code
			dsc.write(alarmCode);
		}
	}
}

void printPacket(const char* label,char cmd,volatile byte cbuf[], int len) {

      std::string s;
      char s1[4];
       for (int c=0;c<len;c++) {
           sprintf(s1,"%02X ",cbuf[c]);
            s.append(s1);
       }
       ESP_LOGI(label,"%02X: %s",cmd,s.c_str());

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

  void update() override {
    	 
    if (!forceDisconnect && dsc.loop() )  { //Processes data only when a valid Keybus command has been read
        if (firstrun) {
          dsc.clearZoneRanges(); // start with clear expanded zones
          dsc.write("*");
          dsc.write("27##"); //fetch low battery status
        }

   
        if (debug > 2 )  {  
            printPacket(" Paneldata:",dsc.panelData[0],dsc.panelData,12);        
            printTimestamp();
            Serial.print("[PANEL] ");
            dsc.printPanelBinary();   // Optionally prints without spaces: printPanelBinary(false);
            Serial.print(" [");
            dsc.printPanelCommand();  // Prints the panel command as hex
            Serial.print("] ");
            dsc.printPanelMessage();  // Prints the decoded message
            Serial.println();
        }
        
        if (dsc.panelData[0]==0x0A && dsc.panelData[3]==0xBA) { //low battery zones
            for (byte panelByte = 4; panelByte < 8; panelByte++) {
                    for (byte zoneBit = 0; zoneBit < 8; zoneBit++) {
                        zone = zoneBit + ((panelByte-4) *  8);
                        if (bitRead(dsc.panelData[panelByte],zoneBit)) {
                            if (zone < MAXZONES)
                                zoneStatus[zone].batteryLow=true;
                        } else  if (zone < MAXZONES)
                                    zoneStatus[zone].batteryLow=false;
                    }
            }
        }
        if (dsc.panelData[0]==0x0A && dsc.panelData[3]==0xA1) { //system status
                        system0=dsc.panelData[4];
   
        }
        if (dsc.panelData[0]==0x0A && dsc.panelData[3]==0xC8) { //service required menu
                        system1=dsc.panelData[4];
     
        }
        if (dsc.panelData[0]==0xE6 && dsc.panelData[2]==0x1A) { //system status comm/time/ac
                        if (bitRead(dsc.panelData[6],4))  //ac status bit
                             troubleStatusChangeCallback(acStatus,false ); 
                        else troubleStatusChangeCallback(acStatus,true );
                         dsc.write("*");
                         dsc.write("21##");
                               
        }
        if (dsc.panelData[0]==0x16 && (dsc.trouble || sendCmd) ) { 
                   sendCmd=false;
                   dsc.write("*");
                   dsc.write("21##");
               // we only send the system status request a minute or so after the trouble status so that we don't impact warning beeps sent after the trouble light is turned on. 

        }
        if (dsc.panelData[0]==0x87 ) { //relay cmd
            byte rchan;
           for (byte relayByte=2;relayByte<4;relayByte++) {
            for (byte x = 0; x < 8; x++) {
                 rchan=x + ((relayByte-2) *  8);
				if (bitRead(dsc.panelData[relayByte], x)) {
                   relayStatus[rchan]=true;
                } else { 
                   relayStatus[rchan]=false;
                }
                if (previousRelayStatus[rchan] != relayStatus[rchan])
                       relayChannelChangeCallback( rchan+1,relayStatus[rchan]);
                previousRelayStatus[rchan] = relayStatus[rchan];
			}
           } 
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
        
        firstrun=false;
    } 

    if (!forceDisconnect && dsc.statusChanged ) {   // Processes data only when a valid Keybus command has been read and statuses were changed
		dsc.statusChanged = false;                   // Reset the status tracking flag
		
        if (debug == 1 )  
            printPacket(" Paneldata:",dsc.panelData[0],dsc.panelData,12);
        
  
  
		// If the Keybus data buffer is exceeded, the sketch is too busy to process all Keybus commands.  Call
		// handlePanel() more often, or increase dscBufferSize in the library: src/dscKeybusInterface.h
		if (dsc.bufferOverflow) ESP_LOGD("Error","Keybus buffer overflow");
		dsc.bufferOverflow = false;

		// Checks if the interface is connected to the Keybus
		if (dsc.keybusChanged) {
			dsc.keybusChanged = false;  // Resets the Keybus data status flag
			if (dsc.keybusConnected) {
				systemStatusChangeCallback(STATUS_ONLINE);
			} else systemStatusChangeCallback(STATUS_OFFLINE);
		}

		// Sends the access code when needed by the panel for arming
		if (dsc.accessCodePrompt && dsc.writeReady && isInt(accessCode,10)) {
			dsc.accessCodePrompt = false;
			dsc.write(accessCode);
			if (debug > 0) ESP_LOGD("Debug","got access code prompt");
		}
        
		if (dsc.powerChanged ) {
			dsc.powerChanged=false;
			if (dsc.powerTrouble) {
               troubleStatusChangeCallback(acStatus,false ); //no ac
            } else troubleStatusChangeCallback(acStatus,true );
		}	

		if (dsc.batteryChanged ) {
			dsc.batteryChanged=false;
			if (dsc.batteryTrouble) {
                troubleStatusChangeCallback(batStatus,true ); 
            } else troubleStatusChangeCallback(batStatus,false );
		}	
		if (dsc.keypadFireAlarm ) {
			dsc.keypadFireAlarm=false;
			partitionMsgChangeCallback(1,"Keypad Fire Alarm");
		}
		if (dsc.keypadPanicAlarm ) {
			dsc.keypadPanicAlarm=false;
            troubleStatusChangeCallback(panicStatus,true ); 
			partitionMsgChangeCallback(1,"Keypad Panic Alarm");
		}
		// Publishes trouble status
		if (dsc.troubleChanged ) {
			dsc.troubleChanged = false;  // Resets the trouble status flag
			if (dsc.trouble) troubleStatusChangeCallback(trStatus,true );  // Trouble alarm tripped
			else troubleStatusChangeCallback(trStatus,false ); // Trouble alarm restored
            // we set a flag to only send the system status request a minute or so after the trouble status so that we don't impact warning beeps sent after the trouble light is turned on.
            sendCmd=true;
		}
	
		// Publishes status per partition
		for (byte partition = 0; partition < dscPartitions; partition++) {
			
		if (dsc.disabled[partition]) continue; //skip disabled or partitions in install programming	
				 
			if (lastStatus[partition] != dsc.status[partition]  ) {
				lastStatus[partition]=dsc.status[partition];
				char msg[50];
				sprintf(msg,"%02X: %s", dsc.status[partition], String(statusText(dsc.status[partition])).c_str());
				if (enable05Messages) partitionMsgChangeCallback(partition+1,msg);

			}

			// Publishes alarm status
			if (dsc.alarmChanged[partition] ) {
				dsc.alarmChanged[partition] = false;  // Resets the partition alarm status flag
				if (dsc.alarm[partition]) {
					dsc.readyChanged[partition] = false;  //if we are triggered no need to trigger a ready state change
					dsc.armedChanged[partition] = false;  // no need to display armed changed
					partitionStatusChangeCallback(partition+1,STATUS_TRIGGERED );
				}
			}
			
			// Publishes armed/disarmed status
			if (dsc.armedChanged[partition] ) {
				dsc.armedChanged[partition] = false;  // Resets the partition armed status flag
				if (dsc.armed[partition]) {
					if ((dsc.armedAway[partition] || dsc.armedStay[partition] )&& dsc.noEntryDelay[partition]) 	partitionStatusChangeCallback(partition+1,STATUS_NIGHT);
					else if (dsc.armedStay[partition]) partitionStatusChangeCallback(partition+1,STATUS_STAY );
					else partitionStatusChangeCallback(partition+1,STATUS_ARM);
				} else partitionStatusChangeCallback(partition+1,STATUS_OFF ); 

			}
			// Publishes exit delay status
			if (dsc.exitDelayChanged[partition] ) {
				dsc.exitDelayChanged[partition] = false;  // Resets the exit delay status flag
				if (dsc.exitDelay[partition]) partitionStatusChangeCallback(partition+1,STATUS_PENDING );  
				else if (!dsc.exitDelay[partition] && !dsc.armed[partition]) partitionStatusChangeCallback(partition+1,STATUS_OFF );
			}
			
			// Publishes ready status
			if (dsc.readyChanged[partition] ) {
				dsc.readyChanged[partition] = false;  // Resets the partition alarm status flag
				if (dsc.ready[partition] ) 	partitionStatusChangeCallback(partition+1,STATUS_OFF ); 
				else if (!dsc.armed[partition]) partitionStatusChangeCallback(partition+1,STATUS_NOT_READY );
			}

			// Publishes fire alarm status
			if (dsc.fireChanged[partition] ) {
				dsc.fireChanged[partition] = false;  // Resets the fire status flag
				if (dsc.fire[partition]) fireStatusChangeCallback(partition+1,true );  // Fire alarm tripped
				else fireStatusChangeCallback(partition+1,false ); // Fire alarm restored
			}
		}

		// Publishes zones 1-64 status in a separate topic per zone
		// Zone status is stored in the openZones[] and openZonesChanged[] arrays using 1 bit per zone, up to 64 zones:
		//   openZones[0] and openZonesChanged[0]: Bit 0 = Zone 1 ... Bit 7 = Zone 8
		//   openZones[1] and openZonesChanged[1]: Bit 0 = Zone 9 ... Bit 7 = Zone 16
		//   ...
		//   openZones[7] and openZonesChanged[7]: Bit 0 = Zone 57 ... Bit 7 = Zone 64
		if (dsc.openZonesStatusChanged  ) {
			dsc.openZonesStatusChanged = false;                           // Resets the open zones status flag
			for (byte zoneGroup = 0; zoneGroup < dscZones; zoneGroup++) {
				for (byte zoneBit = 0; zoneBit < 8; zoneBit++) {
					if (bitRead(dsc.openZonesChanged[zoneGroup], zoneBit)) {  // Checks an individual open zone status flag
						bitWrite(dsc.openZonesChanged[zoneGroup], zoneBit, 0);  // Resets the individual open zone status flag
						zone=zoneBit + (zoneGroup * 8);
						if (bitRead(dsc.openZones[zoneGroup], zoneBit)) {
							zoneStatusChangeCallback(zone+1, true);  // Zone open
                            if (zone < MAXZONES)
                                zoneStatus[zone].open=true;
						}
						else  {
                            zoneStatusChangeCallback(zone+1, false);        // Zone closed
                            if (zone < MAXZONES)
                                zoneStatus[zone].open=false;
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
		if (dsc.alarmZonesStatusChanged  ) {
			dsc.alarmZonesStatusChanged = false;                           // Resets the alarm zones status flag
			for (byte zoneGroup = 0; zoneGroup < dscZones; zoneGroup++) {
				for (byte zoneBit = 0; zoneBit < 8; zoneBit++) {
					if (bitRead(dsc.alarmZonesChanged[zoneGroup], zoneBit)) {  // Checks an individual alarm zone status flag
						bitWrite(dsc.alarmZonesChanged[zoneGroup], zoneBit, 0);  // Resets the individual alarm zone status flag
						zone=zoneBit  + (zoneGroup * 8);
						if (bitRead(dsc.alarmZones[zoneGroup], zoneBit)) {
                            if (zone < MAXZONES)
                                zoneStatus[zone].alarm=true;
						}
						else { 
                            if (zone < MAXZONES)
                                zoneStatus[zone].alarm=false;
                        }
					}
				}
			}
		}
        
        zoneStatusMsg="";
        char s1[7];
        for (int x=0;x<MAXZONES;x++) {
            /*
            if (zoneStatus[x].open) {
                sprintf(s1,"OP:%d",x+1);
                if (zoneStatusMsg!="") zoneStatusMsg.append(",");
                zoneStatusMsg.append(s1);
            }
           */
           
            if (zoneStatus[x].alarm) {
                sprintf(s1,"AL:%d",x+1);
                if (zoneStatusMsg!="") zoneStatusMsg.append(",");
                zoneStatusMsg.append(s1);
            }
            if (zoneStatus[x].tamper) {
                sprintf(s1,"TA:%d",x+1);
                if (zoneStatusMsg!="") zoneStatusMsg.append(",");
                zoneStatusMsg.append(s1);
            }
            if (zoneStatus[x].batteryLow) {
                sprintf(s1,"BL:%d",x+1);
                if (zoneStatusMsg!="") zoneStatusMsg.append(",");
                zoneStatusMsg.append(s1);
            }
        }
        if (zoneStatusMsg=="") zoneStatusMsg="OK";
        
            systemMsg="";
            if (bitRead(system1,0)) {
                if (systemMsg!="") systemMsg.append(",");
                systemMsg.append("BAT");
            }
            if (bitRead(system1,1)) {
                if (systemMsg!="") systemMsg.append(",");
                systemMsg.append("BELL");
            }
            if (bitRead(system1,2)) {
                if (systemMsg!="") systemMsg.append(",");
                systemMsg.append("SYS");
            }
            if (bitRead(system1,3)) {
                if (systemMsg!="") systemMsg.append(",");
                systemMsg.append("TAMP");
            }
            if (bitRead(system1,4)) {
                if (systemMsg!="") systemMsg.append(",");
                systemMsg.append("SUP");
            }
            if (bitRead(system1,5)) {
                if (systemMsg!="") systemMsg.append(",");
                systemMsg.append("RF");
            }
            if (bitRead(system1,6)) {
                if (systemMsg!="") systemMsg.append(",");
                systemMsg.append("B4");
            }
            if (bitRead(system1,7)) {
                if (systemMsg!="") systemMsg.append(",");
                systemMsg.append("A4");
            } 


            if (bitRead(system0,1)) {
                if (systemMsg!="") systemMsg.append(",");
                systemMsg.append("AC");
            }
            if (bitRead(system0,2)) {
                if (systemMsg!="") systemMsg.append(",");
                systemMsg.append("TEL");
            }
            if (bitRead(system0,3)) {
                if (systemMsg!="") systemMsg.append(",");
                systemMsg.append("COM");
            }
            if (bitRead(system0,4)) {
                if (systemMsg!="") systemMsg.append(",");
                systemMsg.append("ZF");
            }
            if (bitRead(system0,5)) {
                if (systemMsg!="") systemMsg.append(",");
                systemMsg.append("ZT");
            }
            if (bitRead(system0,6)) {
                if (systemMsg!="") systemMsg.append(",");
                systemMsg.append("DBAT");
            }
            if (bitRead(system0,7)) {
                if (systemMsg!="") systemMsg.append(",");
                systemMsg.append("TIME");
            }             
        if (systemMsg=="") systemMsg="OK";
        if (previousSystemMsg != systemMsg) 
           troubleMsgStatusCallback(systemMsg);
        previousSystemMsg=systemMsg;
        /*
        if (system0 != previousSystem0) {
            byte diff = system0 ^ previousSystem0;
            if (bitRead(diff,5)) {
                if (bitRead(system0,5))
                    previousSystem0|=0x20;
                else
                    previousSystem0&=0xdf;
                dsc.write("*25##");
             } else if (bitRead(diff,6)) {
               if (bitRead(system0,5))
                    previousSystem0|=0x30;
                else
                    previousSystem0&=0xbf;
                 dsc.write("*26##");
             } else if (bitRead(diff,7)) {
                 if (bitRead(system0,5))
                    previousSystem0|=0x80;
                else
                    previousSystem0&=0x7f;
                dsc.write("*27##");
             } else previousSystem0=system0;

        }
        */
	}

    
    if (!forceDisconnect && dsc.handleModule()) {
      
       if (dsc.panelData[0]==0x41) { 
        if (debug == 1) 
            printPacket("Moduledata:",dsc.panelData[0],dsc.moduleData,16);
        for (byte zoneByte = 0; zoneByte < 4; zoneByte++) {
            byte zoneBit=0;
            for (int x = 7; x >=0; x--) {
                zone=zoneBit + (zoneByte * 8);
                if (!bitRead(dsc.moduleData[zoneByte+2], x)) {  // Checks an individual zone battery status flag for low
                   if (zone < MAXZONES)                
                        zoneStatus[zone].batteryLow=true;                    
                } else if (!bitRead(dsc.moduleData[zoneByte+6], x)) {  // Checks an individual zone battery status flag for restore
                   if (zone < MAXZONES)                
                        zoneStatus[zone].batteryLow=false;
                }
                zoneBit++;
           }
        }
      } 
      
      if (debug > 2) {
        printPacket("Moduledata:",dsc.panelData[0],dsc.moduleData,16);
        printTimestamp();
        Serial.print("[MODULE] ");Serial.print(dsc.panelData[0],HEX);Serial.print(": ");
        dsc.printModuleBinary();   // Optionally prints without spaces: printKeybusBinary(false);
        Serial.print(" ");
        dsc.printModuleMessage();  // Prints the decoded message
        Serial.println();
      }
    }
    if (zoneStatusMsg != previousZoneStatusMsg)
        zoneMsgStatusCallback(zoneStatusMsg); 
    previousZoneStatusMsg=zoneStatusMsg;
  }
const __FlashStringHelper *statusText(uint8_t statusCode)
{
    switch (statusCode) {
        case 0x01: return F("Ready");
        case 0x02: return F("Stay zones open");
        case 0x03: return F("Zones open");
        case 0x04: return F("Armed stay");
        case 0x05: return F("Armed away");
        case 0x06: return F("No entry delay");
        case 0x07: return F("Failed to arm");
        case 0x08: return F("Exit delay");
        case 0x09: return F("No entry delay");
        case 0x0B: return F("Quick exit");
        case 0x0C: return F("Entry delay");
        case 0x0D: return F("Alarm memory");
        case 0x10: return F("Keypad lockout");
        case 0x11: return F("Alarm");
        case 0x14: return F("Auto-arm");
        case 0x15: return F("Arm with bypass");
        case 0x16: return F("No entry delay");
        case 0x17: return F("Power failure");//??? not sure
        case 0x22: return F("Alarm memory");
        case 0x33: return F("Busy");
        case 0x3D: return F("Disarmed");
        case 0x3E: return F("Disarmed");
        case 0x40: return F("Keypad blanked");
        case 0x8A: return F("Activate zones");
        case 0x8B: return F("Quick exit");
        case 0x8E: return F("Invalid option");
        case 0x8F: return F("Invalid code");
        case 0x9E: return F("Enter * code");
        case 0x9F: return F("Access code");
        case 0xA0: return F("Zone bypass");
        case 0xA1: return F("Trouble menu");
        case 0xA2: return F("Alarm memory");
        case 0xA3: return F("Door chime on");
        case 0xA4: return F("Door chime off");
        case 0xA5: return F("Master code");
        case 0xA6: return F("Access codes");
        case 0xA7: return F("Enter new code");
        case 0xA9: return F("User function");
        case 0xAA: return F("Time and Date");
        case 0xAB: return F("Auto-arm time");
        case 0xAC: return F("Auto-arm on");
        case 0xAD: return F("Auto-arm off");
        case 0xAF: return F("System test");
        case 0xB0: return F("Enable DLS");
        case 0xB2: return F("Command output");
        case 0xB7: return F("Installer code");
        case 0xB8: return F("Enter * code");
        case 0xB9: return F("Zone tamper");
        case 0xBA: return F("Zones low batt.");
        case 0xC6: return F("Zone fault menu");
        case 0xC8: return F("Service required");
        case 0xD0: return F("Keypads low batt");
        case 0xD1: return F("Wireless low bat");
        case 0xE4: return F("Installer menu");
        case 0xE5: return F("Keypad slot");
        case 0xE6: return F("Input: 2 digits");
        case 0xE7: return F("Input: 3 digits");
        case 0xE8: return F("Input: 4 digits");
        case 0xEA: return F("Code: 2 digits");
        case 0xEB: return F("Code: 4 digits");
        case 0xEC: return F("Input: 6 digits");
        case 0xED: return F("Input: 32 digits");
        case 0xEE: return F("Input: option");
        case 0xF0: return F("Function key 1");
        case 0xF1: return F("Function key 2");
        case 0xF2: return F("Function key 3");
        case 0xF3: return F("Function key 4");
        case 0xF4: return F("Function key 5");
        case 0xF8: return F("Keypad program");
        case 0xFF: return F("Disabled");
        default: return F("Unknown");
    }
}


};
#endif