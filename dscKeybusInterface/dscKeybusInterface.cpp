/*
    DSC Keybus Interface

    https://github.com/taligentx/dscKeybusInterface

    This library is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
<<<<<<< HEAD
 */



#include "dscKeybusInterface.h"

byte dscKeybusInterface::dscClockPin;
byte dscKeybusInterface::dscReadPin;
byte dscKeybusInterface::dscWritePin;
char dscKeybusInterface::writeKey;
byte dscKeybusInterface::writePartition;
byte dscKeybusInterface::writeByte;
byte dscKeybusInterface::writeBit;
byte dscKeybusInterface::writeModuleBit;
bool dscKeybusInterface::virtualKeypad;
bool dscKeybusInterface::processModuleData;
byte dscKeybusInterface::panelData[dscReadSize];
byte dscKeybusInterface::panelByteCount;
byte dscKeybusInterface::panelBitCount;
volatile bool dscKeybusInterface::writeKeyPending;
volatile bool dscKeybusInterface::writeModulePending;
volatile bool dscKeybusInterface::pendingDeviceUpdate;
volatile byte dscKeybusInterface::moduleData[dscReadSize];
volatile bool dscKeybusInterface::moduleDataCaptured;
volatile byte dscKeybusInterface::moduleByteCount;
volatile byte dscKeybusInterface::moduleBitCount;
volatile bool dscKeybusInterface::writeAlarm;
volatile bool dscKeybusInterface::writeAsterisk;
volatile bool dscKeybusInterface::wroteAsterisk;
volatile bool dscKeybusInterface::bufferOverflow;
volatile byte dscKeybusInterface::panelBufferLength;
volatile byte dscKeybusInterface::currentModuleIdx;
volatile byte dscKeybusInterface::moduleResponseCmd;
volatile byte dscKeybusInterface::moduleBufferLength;
volatile byte dscKeybusInterface::panelBuffer[dscBufferSize][dscReadSize];
volatile byte dscKeybusInterface::panelBufferBitCount[dscBufferSize];
volatile byte dscKeybusInterface::panelBufferByteCount[dscBufferSize];
byte dscKeybusInterface::moduleSlots[6];
volatile byte dscKeybusInterface::writeModuleBuffer[6];
volatile byte dscKeybusInterface::pendingZoneStatus[6];
moduleType dscKeybusInterface::modules[maxModules];
byte dscKeybusInterface::moduleIdx;
byte dscKeybusInterface::inIdx;
byte dscKeybusInterface::outIdx;
byte dscKeybusInterface::maxFields05; 
byte dscKeybusInterface::maxFields11;
byte dscKeybusInterface::maxZones;
byte dscKeybusInterface::panelVersion;
bool dscKeybusInterface::enableModuleSupervision;
//volatile byte  dscKeybusInterface::updateQueue[updateQueueSize];
volatile byte dscKeybusInterface::isrPanelData[dscReadSize];
volatile byte dscKeybusInterface::isrPanelByteCount;
volatile byte dscKeybusInterface::isrPanelBitCount;
volatile byte dscKeybusInterface::isrPanelBitTotal;
volatile byte dscKeybusInterface::isrModuleData[dscReadSize];
volatile byte dscKeybusInterface::isrModuleByteCount;
volatile byte dscKeybusInterface::isrModuleBitCount;
volatile byte dscKeybusInterface::isrModuleBitTotal;
volatile byte dscKeybusInterface::currentCmd;
bool dscKeybusInterface::debounce05;
volatile byte dscKeybusInterface::moduleCmd;
volatile byte dscKeybusInterface::moduleSubCmd;
volatile byte dscKeybusInterface::statusCmd;
volatile unsigned long dscKeybusInterface::clockHighTime;
volatile unsigned long dscKeybusInterface::keybusTime;
=======
  */
>>>>>>> new

#include "dscKeybus.h"


#if defined(ESP32)
portMUX_TYPE dscKeybusInterface::timer1Mux = portMUX_INITIALIZER_UNLOCKED;

#if ESP_IDF_VERSION_MAJOR < 444
hw_timer_t * dscKeybusInterface::timer1 = NULL;

#else // ESP-IDF 4+
esp_timer_handle_t timer0;
const esp_timer_create_args_t timer0Parameters = {
  .callback =  & dscKeybusInterface::dscDataInterrupt
};

#endif // ESP_IDF_VERSION_MAJOR
#endif // ESP32

dscKeybusInterface::dscKeybusInterface(byte setClockPin, byte setReadPin, byte setWritePin) {
  dscClockPin = setClockPin;
  dscReadPin = setReadPin;
  dscWritePin = setWritePin;
  if (dscWritePin != 255) virtualKeypad = true;
  //writeReady = false;
  processRedundantData = true;
  displayTrailingBits = false;
  processModuleData = false;
  currentDefaultPartition=1;
  pauseStatus = false;

  // start expander
#if defined(EXPANDER)   
  maxFields05 = 4;
  maxFields11 = 4;
  enableModuleSupervision = false;
  maxZones = 32;
  for (int x = 0; x < 6; x++) { //clear all statuses
    // pendingZoneStatus[x]=0xff;
    moduleSlots[x] = 0xff;
  }
#endif
  //end expander


}

void dscKeybusInterface::begin(Stream & _stream,byte setClockPin, byte setReadPin, byte setWritePin) {
  
  if (setClockPin > 0 && setReadPin > 0 && setWritePin > 0) {  
    dscClockPin = setClockPin;
    dscReadPin = setReadPin;
    dscWritePin = setWritePin;
  }
  
  pinMode(dscClockPin, INPUT);
  pinMode(dscReadPin, INPUT);
  if (virtualKeypad) pinMode(dscWritePin, OUTPUT);
  stream = & _stream;

  // Platform-specific timers trigger a read of the data line 250us after the Keybus clock changes

  // Arduino/AVR Timer1 calls ISR(TIMER1_OVF_vect) from dscClockInterrupt() and is disabled in the ISR for a one-shot timer
  #if defined(__AVR__)
  TCCR1A = 0;
  TCCR1B = 0;
  TIMSK1 |= (1 << TOIE1);

  // esp8266 timer1 calls dscDataInterrupt() from dscClockInterrupt() as a one-shot timer
  #elif defined(ESP8266)
  timer1_isr_init();
  timer1_attachInterrupt(dscDataInterrupt);
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);

  // esp32 timer1 calls dscDataInterrupt() from dscClockInterrupt()
  #elif defined(ESP32)
  #if ESP_IDF_VERSION_MAJOR < 444
  timer1 = timerBegin(1, 80, true);
  timerStop(timer1);
  timerAttachInterrupt(timer1, & dscDataInterrupt, true);
  timerAlarmWrite(timer1, 250, true);
  timerAlarmEnable(timer1);
  #else // IDF4+
  esp_timer_create( & timer0Parameters, & timer0);
  #endif // ESP_IDF_VERSION_MAJOR
  #endif // ESP32
  // Generates an interrupt when the Keybus clock rises or falls - requires a hardware interrupt pin on Arduino/AVR
  attachInterrupt(digitalPinToInterrupt(dscClockPin), dscClockInterrupt, CHANGE);
#if defined(EXPANDER)  
  if (maxZones > 32) {
    maxFields05 = 6;
    maxFields11 = 6;
  } else {
    maxFields05 = 4;
    maxFields11 = 4;
  }
#endif  

}

void dscKeybusInterface::stop() {

  // Disables Arduino/AVR Timer1 interrupts
  #if defined(__AVR__)
  TIMSK1 = 0;

  // Disables esp8266 timer1
  #elif defined(ESP8266)
  timer1_disable();
  timer1_detachInterrupt();

  // Disables esp32 timer0
  #elif defined(ESP32)
  #if ESP_IDF_VERSION_MAJOR < 444
  timerAlarmDisable(timer1);
  timerEnd(timer1);
  #else // ESP-IDF 4+
  esp_timer_stop(timer0);
  #endif // ESP_IDF_VERSION_MAJOR
  #endif // ESP32

  // Disables the Keybus clock pin interrupt
  detachInterrupt(digitalPinToInterrupt(dscClockPin));

  // Resets the panel capture data and counters
  panelBufferLength = 0;
  for (byte i = 0; i < dscReadSize; i++) isrPanelData[i] = 0;
  isrPanelBitTotal = 0;
  isrPanelBitCount = 0;
  isrPanelByteCount = 0;

  // Resets the keypad and module capture data
  for (byte i = 0; i < dscReadSize; i++) isrModuleData[i] = 0;
}

bool dscKeybusInterface::loop() {

  #if defined(ESP8266) || defined(ESP32)
  yield();
  #endif

  // Checks if Keybus data is detected and sets a status flag if data is not detected for 3s
  #if defined(ESP32)
  portENTER_CRITICAL( & timer1Mux);
  #else
  noInterrupts();
  #endif

  if (millis() - keybusTime > 3000) keybusConnected = false; // keybusTime is set in dscDataInterrupt() when the clock resets
  else keybusConnected = true;

  #if defined(ESP32)
  portEXIT_CRITICAL( & timer1Mux);
  #else
  interrupts();
  #endif

  if (previousKeybus != keybusConnected) {
    previousKeybus = keybusConnected;
    keybusChanged = true;
    if (!pauseStatus) statusChanged = true;
    if (!keybusConnected) return true;
  }

  // Skips processing if the panel data buffer is empty
  if (panelBufferLength == 0) return false;

  // Copies data from the buffer to panelData[]
  static byte panelBufferIndex = 1;
  byte dataIndex = panelBufferIndex - 1;
  //for (byte i = 0; i < dscReadSize; i++) panelData[i] = panelBuffer[dataIndex][i];
  memcpy((void*) panelData,(void*) &panelBuffer[dataIndex],dscReadSize);
  panelBitCount = panelBufferBitCount[dataIndex];
  panelByteCount = panelBufferByteCount[dataIndex];
  panelBufferIndex++;

  // Resets counters when the buffer is cleared
  #if defined(ESP32)
  portENTER_CRITICAL( & timer1Mux);
  #else
  noInterrupts();
  #endif

  if (panelBufferIndex > panelBufferLength) {
    panelBufferIndex = 1;
    panelBufferLength = 0;
  }

  #if defined(ESP32)
  portEXIT_CRITICAL( & timer1Mux);
  #else
  interrupts();
  #endif

  // Waits at startup for the 0x05 status command or a command with valid CRC data to eliminate spurious data.
  static bool startupCycle = true;
  if (startupCycle) {
    if (panelData[0] == 0) return false;
    else if (panelData[0] == 0x05 || panelData[0] == 0x1B) {
      if (panelByteCount == 6) keybusVersion1 = true;
      startupCycle = false;
#if defined(EXPANDER) 
      updateModules();
#endif      

    } else if (!validCRC()) return false;
  }

  // Skips redundant data sent constantly while in installer programming
  static byte previousCmd0A[dscReadSize];
  static byte previousCmd0F[dscReadSize];
  static byte previousCmdE6_20[dscReadSize];
  static byte previousCmdE6_21[dscReadSize];
  switch (panelData[0]) {
  case 0x0A: // Partition 1 status in programming
    if (redundantPanelData(previousCmd0A, panelData)) return false;
    break;

  case 0x0F: // Partition 2 status in programming
    if (redundantPanelData(previousCmd0F, panelData)) return false;
    break;

  case 0xE6:
    if (panelData[2] == 0x20 && redundantPanelData(previousCmdE6_20, panelData)) return false; // Partition 1 status in programming, zone lights 33-64
    if (panelData[2] == 0x21 && redundantPanelData(previousCmdE6_21, panelData)) return false; // Partition 2 status in programming
    break;
  }
  if (dscPartitions > 4) {
    static byte previousCmdE6_03[dscReadSize];
    if (panelData[0] == 0xE6 && panelData[2] == 0x03 && redundantPanelData(previousCmdE6_03, panelData, 8)) return false; // Status in alarm/programming, partitions 5-8
  }

  // Processes valid panel data
  switch (panelData[0]) {
  case 0x05: // Panel status: partitions 1-4
  case 0x1B:
    processPanelStatus();
    break; // Panel status: partitions 5-8
  case 0x16:
    processPanel_0x16();
    break; // Panel configuration
  case 0x27:
    processPanel_0x27();
    break; // Panel status and zones 1-8 status
  case 0x2D:
    processPanel_0x2D();
    break; // Panel status and zones 9-16 status    
  case 0x34:
    processPanel_0x34();
    break; // Panel status and zones 17-24 status
  case 0x3E:
    processPanel_0x3E();
    break; // Panel status and zones 25-32 status
  case 0x6E:
    processPanel_0x6E();
    break; // Program field output
  case 0x87:
    processPanel_0x87();
    break; // PGM outputs
  case 0xA5:
     processPanel_0xA5();
    break; // Date, time, system status messages - partitions 1-2
  case 0xE6:
    if (dscPartitions > 2) processPanel_0xE6();
    break; // Extended status command split into multiple subcommands to handle up to 8 partitions/64 zones
  case 0xEB:
    if (dscPartitions > 2) processPanel_0xEB();
    break; // Date, time, system status messages - partitions 1-8    
  }

  return true;
}

bool dscKeybusInterface::handleModule() {
  if (!moduleDataCaptured) 
      return false;
  moduleDataCaptured = false;

  if (moduleBitCount < 8) 
      return false;

  // Determines if a keybus message is a response to a panel command
  switch (moduleCmd) {
  case 0x11:
  case 0xD5:
    queryResponse = true;
    break;
  default:
    queryResponse = false;
    break;
  }

  return true;
}

// Sets up writes for multiple keys sent as a char array
void dscKeybusInterface::write(const char * receivedKeys, int partition) {
  for (uint8_t x = 0; x < strlen(receivedKeys); x++) {
    write(receivedKeys[x],partition);
  }
}

// Specifies the key value to be written by dscClockInterrupt() and selects the write partition.  
void dscKeybusInterface::write(const char receivedKey,int partition) {

  static bool setPartition;
  // Sets the write partition if set by virtual keypad key '/'
  if (partition < 1) partition=currentDefaultPartition;
  if (setPartition) {
    setPartition = false;
    if (receivedKey >= '1' && receivedKey <= '8') {
      currentDefaultPartition = receivedKey - 48;
    }
    return;
  }


  // Sets the binary to write for virtual keypad keys

  bool validKey = true;
  bool isAlarm = false;
  // Skips writing to disabled partitions or partitions not specified in dscKeybusInterface.h
  if (disabled[partition - 1] || dscPartitions < partition) {
    switch (receivedKey) {
    case '/':
      setPartition = true;
      validKey = false;
      break;
    }
    return;
  }

  // Sets binary for virtual keypad keys
  else {
    switch (receivedKey) {
    case '/':
      setPartition = true;
      validKey = false;
      break;
    case '0':
      writeKey = 0x00;
      break;
    case '1':
      writeKey = 0x05;
      break;
    case '2':
      writeKey = 0x0A;
      break;
    case '3':
      writeKey = 0x0F;
      break;
    case '4':
      writeKey = 0x11;
      break;
    case '5':
      writeKey = 0x16;
      break;
    case '6':
      writeKey = 0x1B;
      break;
    case '7':
      writeKey = 0x1C;
      break;
    case '8':
      writeKey = 0x22;
      break;
    case '9':
      writeKey = 0x27;
      break;
    case '*':
      writeKey = 0x28;
      break;
    case '#':
      writeKey = 0x2D;
      break;
    case 'f':
    case 'F':
      writeKey = 0xBB;
      isAlarm = true;
      break; // Keypad fire alarm
    case 'b':
    case 'B':
      writeKey = 0x82;
      break; // Enter event buffer
    case '>':
      writeKey = 0x87;
      break; // Event buffer right arrow
    case '<':
      writeKey = 0x88;
      break; // Event buffer left arrow
    case 'l':
    case 'L':
      writeKey = 0xA5;
      break; // LCD keypad data request
    case 's':
    case 'S':
      writeKey = 0xAF;
      writeAccessCode[partition - 1] = true;
      break; // Arm stay
    case 'w':
    case 'W':
      writeKey = 0xB1;
      writeAccessCode[partition - 1] = true;
      break; // Arm away
    case 'n':
    case 'N':
      writeKey = 0xB6;
      writeAccessCode[partition - 1] = true;
      break; // Arm with no entry delay (night arm)
    case 'a':
    case 'A':
      writeKey = 0xDD;
      isAlarm = true;
      break; // Keypad auxiliary alarm
    case 'c':
    case 'C':
      writeKey = 0xBB;
      break; // Door chime
    case 'r':
    case 'R':
      writeKey = 0xDA;
      break; // Reset
    case 'p':
    case 'P':
      writeKey = 0xEE;
      isAlarm = true;
      break; // Keypad panic alarm
    case 'x':
    case 'X':
      writeKey = 0xE1;
      break; // Exit
    case '[':
      writeKey = 0xD5;
      writeAccessCode[partition - 1] = true;
      break; // Command output 1
    case ']':
      writeKey = 0xDA;
      writeAccessCode[partition - 1] = true;
      break; // Command output 2
    case '{':
      writeKey = 0x70;
      writeAccessCode[partition - 1] = true;
      break; // Command output 3
    case '}':
      writeKey = 0xEC;
      writeAccessCode[partition - 1] = true;
      break; // Command output 4
    case '$':
      writeKey = 0xF7;
      break; //place holder character during text editing   

    default: {
      validKey = false;
      break;
    }
    }
  }

   if (validKey) {
    if (isAlarm)
      writeCharsToQueue((byte * ) & writeKey, 0,1, true);
    else
      writeCharsToQueue((byte * ) & writeKey, partition, 1, false);

  }

}

bool
#if defined(ESP8266)
ICACHE_RAM_ATTR
#elif defined(ESP32)
IRAM_ATTR
#endif
dscKeybusInterface::redundantPanelData(byte  previousCmd[] , volatile byte  currentCmd[], byte checkedBytes) {
  for (byte i = 0; i < checkedBytes; i++) {
    if (previousCmd[i] != currentCmd[i]) {
      memcpy((void*)previousCmd,(void*)currentCmd,dscReadSize);      
      return false;
    }
  }
  return true;
}

bool dscKeybusInterface::validCRC() {
  byte byteCount = (panelBitCount - 1) / 8;
  int dataSum = 0;
  for (byte panelByte = 0; panelByte < byteCount; panelByte++) {
    if (panelByte != 1) dataSum += panelData[panelByte];
  }
<<<<<<< HEAD
  if (dataSum % 256 == panelData[byteCount]) return true;
  else return false;
}


void dscKeybusInterface::setSupervisorySlot(byte address,bool set=true) {
       //set our response data for the 0x11 supervisory request
       if (panelVersion < 3) {
            switch (address) {
                     //for older versions we need to set 2 slots since they expect groups of 4 zones per slot
            case 9:   moduleSlots[2]=set?moduleSlots[2]&0x3f:moduleSlots[2]|~0x3f; //pc5108 
                      moduleSlots[2]=set?moduleSlots[2]&0xcf:moduleSlots[2]|~0xcf;
                      break; 
            case 10:  moduleSlots[2]=set?moduleSlots[2]&0xf3:moduleSlots[2]|~0xf3; //pc5108
                      moduleSlots[2]=set?moduleSlots[2]&0xfc:moduleSlots[2]|~0xfc;
                      break; 
            case 11:  moduleSlots[3]=set?moduleSlots[3]&0x3f:moduleSlots[3]|~0x3f;//pc5108
                      moduleSlots[3]=set?moduleSlots[3]&0xcf:moduleSlots[3]|~0xcf;
                      break; 
            case 18:  moduleSlots[3]=set?moduleSlots[3]&0xfc:moduleSlots[3]|~0xfc;break;  // pc5208 relay board reports as 18 but reports on slot 16
            default: return;
        }
           
       } else {
        switch (address) {
            //11111111 1 00111111 11111111 11111111 11111111 11111111 11111100 11111111 16
            //11111111 1 00111111 11111111 11111111 00111111 11111111 11111111 11111111 13
            // 1111111 1 00111111 11111111 00111111 11111111 11111111 11111111 11111111 slots 9
            //11111111 1 00111111 11111111 11111111 11111111 11111111 11111100 11111111 slots 16
            case 9:   moduleSlots[2]=set?moduleSlots[2]&0x3f:moduleSlots[2]|~0x3f;break; //pc5108 
            case 10:  moduleSlots[2]=set?moduleSlots[2]&0xcf:moduleSlots[2]|~0xcf;break; //pc5108
            case 11:  moduleSlots[2]=set?moduleSlots[2]&0xf3:moduleSlots[2]|~0xf3;break; //pc5108
            case 12:  if  (maxZones>32) {moduleSlots[2]=set?moduleSlots[2]&0xfc:moduleSlots[2]|~0xfc;}break; //pc5108
            case 13:  if  (maxZones>32) {moduleSlots[3]=set?moduleSlots[3]&0x3f:moduleSlots[3]|~0x3f;}break; //pc5108
            case 14:  if  (maxZones>32) {moduleSlots[3]=set?moduleSlots[3]&0xcf:moduleSlots[3]|~0xcf;}break; //pc5108
            case 16:  if  (maxZones>32) {moduleSlots[5]=set?moduleSlots[5]&0x3f:moduleSlots[5]|~0x3f;}break; //pc5108 (shows on slot24)// reports as 16 in panel
            //reports as 18 in panel
            case 18:  moduleSlots[3]=set?moduleSlots[3]&0xfc:moduleSlots[3]|~0xfc;break;  // pc5208 relay board shows on slot 16 but reports as 18
            default: return;
        }
       }
    
}

void dscKeybusInterface::addRequestToQueue(byte address) {
            if (!address) return;
           // updateQueue[inIdx]=address;
            inIdx=(inIdx + 1) % updateQueueSize;
}

zoneMaskType dscKeybusInterface::getUpdateMask(byte address) {

        //get our request byte and mask to send data for the zone that we need to publish info on. This gets sent on the 05 command
        //11111111 1 11111111 11111111 11111111 11111111 11111111 01111111 11111111 11111111 (12)
        //11111111 1 11111111 11111111 10111111 11111111 11111111 11111111 11111111 11111111 (9)
        //11111111 1 11111111 11111111 10111111 11111111 version 2 zone 9 -16
        //11111111 1 11111111 11111111 11101111 11111111  version 2 system zone 27-32

        zoneMaskType zm;
        switch (address) {
            case 9:  zm.idx=2;zm.mask=0xbf; break; //5208
            case 10: zm.idx=2;zm.mask=0xdf; break; //5208
            case 11: zm.idx=2;zm.mask=0xef; break;//5208
            case 12: if (maxZones>32) {zm.idx=5;zm.mask=0x7f;} break;
            case 13: if (maxZones>32) {zm.idx=5;zm.mask=0xbf;} break;
            case 14: if (maxZones>32) {zm.idx=5;zm.mask=0xdf;} break;
            case 16: if (maxZones>32) {zm.idx=5;zm.mask=0xef;} break;//5208 sends to slot 15
            default: zm.idx=0;zm.mask=0;
        }
        return zm;
}


//clears all emulated zones on the panel 
void dscKeybusInterface:: clearZoneRanges() {
for (int x=0;x<moduleIdx;x++) {
        modules[x].faultBuffer[0]=0x55;
        modules[x].faultBuffer[1]=0;
        modules[x].faultBuffer[2]=0x55;
        modules[x].faultBuffer[3]=0;
        modules[x].faultBuffer[4]= 0xaa ;  //cksum for 01010101 00000000 
        if (!modules[x].zoneStatusByte) continue;
        pendingZoneStatus[modules[x].zoneStatusByte]&=modules[x].zoneStatusMask; //set update slot
        addRequestToQueue(modules[x].address);  //update queue to indicate pending request
      }
}

//once we know what panelversion we have, we can then update the modules with the correct info here
void dscKeybusInterface::updateModules() {
    for (int x=0;x<moduleIdx;x++) {
        zoneMaskType zm=getUpdateMask(modules[x].address);
        if (!zm.idx) {
            //we don't have an update idx so it means our address is invalid. clear it
            modules[x].address=0;
        } else {
            modules[x].zoneStatusByte=zm.idx;
            modules[x].zoneStatusMask=zm.mask;
            if (enableModuleSupervision)
                setSupervisorySlot(modules[x].address,true);
        }
    }
}

//add new expander modules and init zone fields
void dscKeybusInterface::addModule(byte address) {
    
 if (!address) return;
 if (address > 12 && maxZones <=32) return;
 if (moduleIdx < maxModules ) {
   modules[moduleIdx].address=address;
  // for (int x=0;x<4;x++) modules[moduleIdx].fields[x]=0x55;//set our zones as closed by default (01 per channel)
   memset(modules[moduleIdx].fields,0x55,4);
   moduleIdx++;
 }
}

void dscKeybusInterface::addRelayModule() {
    if (enableModuleSupervision)
        setSupervisorySlot(18,true);
}

void dscKeybusInterface::removeModule(byte address) {
    int idx;
    for (idx=0;idx<moduleIdx;idx++) {
        if (modules[idx].address==address) break;
    }
    if (idx==moduleIdx) return;
   modules[idx].address=0;
   setSupervisorySlot(address,false); //remove it from the supervisory response

}

void dscKeybusInterface::setZoneFault(byte zone,bool fault) {
      
    byte address=0;
    byte channel=0;
    bool change=false;
    
    //we try and do as much setup here so that the ISR functions do the mimimal work.
    
    if (zone > maxZones) return;
    
    //get address and channel from zone range
        if (zone > 8 && zone < 17) {
            address=9;
            channel=zone-9;
        } else if (zone > 16 && zone < 25) {
            address=10;
            channel=zone-17;
        } else if (zone > 24 && zone < 33) {
            address=11;
            channel=zone-25;
        } else if (zone > 32 && zone < 41) {
            address=12;
            channel=zone-33;
        } else if (zone > 40 && zone < 49) {
            address=13;
            channel=zone-41;
        } else if (zone > 48 && zone < 57 ) {
            address=14;
            channel=zone-49;
        } else if (zone > 56 && zone < 65) {
            address=16;
            channel=zone-57;
        } 
    
     
    
    if (!address ) return; //invalid zone, so return
 
 
    //see if we are emulating this zone range 
    int idx;
    for (idx=0;idx<moduleIdx;idx++) {
        if (modules[idx].address==address) break;
    }

    if (idx==moduleIdx) return;  //address not found in our emulation list so return
    
    uint8_t chk1=0xff;
    uint8_t chk2=0xff;
           

    
    if (channel < 4) { //set / reset bits according to fault value (open=11,closed=01)
        channel=channel*2;
        modules[idx].fields[0]=fault?modules[idx].fields[0] | (zoneOpen << channel):modules[idx].fields[0] & ~(zoneClosed << channel);
        chk1=((modules[idx].fields[0] >> 4)+(modules[idx].fields[0]&0x0f)+(modules[idx].fields[1]>>4)+(modules[idx].fields[1]&0x0f)) % 0x10;
    } else {
        channel=(channel-4)*2;
        modules[idx].fields[2]=fault?modules[idx].fields[2] | (zoneOpen << (channel)):modules[idx].fields[2] & ~(zoneClosed << (channel));
        chk2=((modules[idx].fields[2]>>4)+(modules[idx].fields[2]&0x0f)+(modules[idx].fields[3]>>4)+(modules[idx].fields[3]&0x0f)) % 0x10;
    }

    //for (int x=0;x<5;x++)  modules[idx].faultBuffer[x]=0xFF;//clear buffer
    memset(modules[idx].faultBuffer,0xFF,5);

    if ( modules[idx].fields[0] != modules[idx].fields[1]) { //see if our current low channels changed from previous. 
         modules[idx].faultBuffer[0]=modules[idx].fields[0]; //populate faultbuffer with response data for low channel
         modules[idx].faultBuffer[1]=modules[idx].fields[1];
         modules[idx].faultBuffer[4]=(chk1 << 4) | 0x0F; 
         modules[idx].fields[1]=modules[idx].fields[0];  //copy current channel values to previous
         change=true;
    }
    if (modules[idx].fields[2] != modules[idx].fields[3]) {  //check high channels
         modules[idx].faultBuffer[2]=modules[idx].fields[2];
         modules[idx].faultBuffer[3]=modules[idx].fields[3];
         modules[idx].faultBuffer[4]= (modules[idx].faultBuffer[4] & 0xf0) | chk2 ; 
         modules[idx].fields[3]=modules[idx].fields[2];  //copy current channel values to previous
         change=true;
    }
    if (!change) return;  //nothing changed in our zones so return
    if (modules[idx].zoneStatusByte) { 
        pendingZoneStatus[modules[idx].zoneStatusByte]&=modules[idx].zoneStatusMask; //set update slot
        addRequestToQueue(address);  //update queue to indicate pending request
    }
}

#if defined(__AVR__)
void dscKeybusInterface::dscKeybusInterface::fillBuffer(byte *src,int len) {
#elif defined(ESP8266)
void  ICACHE_RAM_ATTR dscKeybusInterface::dscKeybusInterface::fillBuffer(byte *src,int len) {
#elif defined(ESP32)
void IRAM_ATTR dscKeybusInterface::dscKeybusInterface::fillBuffer(byte *src,int len) {
#endif
  moduleBufferLength=len;
  for(byte x=0;x<len;x++) writeModuleBuffer[x]=src[x];
  writeModulePending=true;   //set flag to send it  
}

#if defined(__AVR__)
void dscKeybusInterface::dscKeybusInterface::prepareResponse(byte address) {
#elif defined(ESP8266)
void  ICACHE_RAM_ATTR dscKeybusInterface::dscKeybusInterface::prepareResponse(byte address) {
#elif defined(ESP32)
void IRAM_ATTR dscKeybusInterface::dscKeybusInterface::prepareResponse(byte address) {
#endif
    if (!address) return; //cmds 05/11 return here
    for (int idx=0;idx<moduleIdx;idx++) {  //get the buffer data from the module record that matches the address we need
        if (modules[idx].address==address) {
             pendingZoneStatus[modules[idx].zoneStatusByte]|=~modules[idx].zoneStatusMask; //clear update slot
             fillBuffer((byte*) modules[idx].faultBuffer,5);
            return;
        }
    }

 
}

#if defined(__AVR__)
void dscKeybusInterface::dscKeybusInterface::processModuleResponse(byte cmd) {
#elif defined(ESP8266)
void ICACHE_RAM_ATTR dscKeybusInterface::dscKeybusInterface::processModuleResponse(byte cmd) {
#elif defined(ESP32)
void IRAM_ATTR dscKeybusInterface::dscKeybusInterface::processModuleResponse(byte cmd) {
#endif

/*
11111111 1 11111111 11111111 10111111 11111111 11111111 11111111 11111111 11111111 9
11111111 1 11111111 11111111 11011111 11111111 11111111 11111111 11111111 11111111 10
11111111 1 11111111 11111111 11101111 11111111 11111111 11111111 11111111 11111111 11
11111111 1 11111111 11111111 11111111 11111111 11111111 01111111 11111111 11111111 12
11111111 1 11111111 11111111 11111111 11111111 11111111 10111111 11111111 11111111 13
11111111 1 11111111 11111111 11111111 11111111 11111111 11011111 11111111 11111111 14
11111111 1 11111111 11111111 11111111 11111111 11111111 11101111 11111111 11111111 16
*/
     byte address=0;
     switch (cmd) {
       case 0x05:   if (inIdx == outIdx) return;
                    outIdx = (outIdx + 1) % updateQueueSize;
                    fillBuffer((byte*) pendingZoneStatus,maxFields05);
                    break;
//11111111 1 00111111 11111111 11111111 11111111 11111111 11111100 11111111 device 16 in slot 24  
//11111111 1 00111111 11111111 11110011 11111111 11111111 11111111 11111111  slot 11   
//11111111 1 00111111 11111111 00111111 11111111 11111111 11111111 11111111    slot 9     
       case 0x11:   if (!enableModuleSupervision) return;
                    fillBuffer((byte*) pendingZoneStatus,maxFields11);
                    break;
       case 0x28:   address=9;break;  // the address will depend on the panel request command.
       case 0x33:   address=10;break;
       case 0x39:   address=11;break;
       default:     return;            
    }
    
    moduleResponseCmd=cmd; //set command to respond on
    moduleSubCmd=0;
    currentModuleIdx=0;
    writeModuleBit=9; //set bit location where we start sending our own data on the command
    
    prepareResponse(address);

}


#if defined(__AVR__)
void dscKeybusInterface::processModuleResponse_0xE6(byte subcmd) {
#elif defined(ESP8266)
void  ICACHE_RAM_ATTR dscKeybusInterface::processModuleResponse_0xE6(byte subcmd) {
#elif defined(ESP32)
void IRAM_ATTR dscKeybusInterface::processModuleResponse_0xE6(byte subcmd) {
#endif

    byte address=0;
    switch (subcmd) {
       case 0x8:   address=12;break;
       case 0xA:   address=13;break;
       case 0xC:   address=14;break;
       case 0xE:   address=16;break;
       default:     return;            
    }
    moduleResponseCmd=0xE6;
    moduleSubCmd=subcmd;
    currentModuleIdx=0;
    writeModuleBit=17;
    prepareResponse(address);
=======
  if (dataSum % 256 == panelData[byteCount]) 
      return true;
>>>>>>> new

      return false;
}

// Called as an interrupt when the DSC clock changes to write data for virtual keypad and setup timers to read
// data after an interval.
void
#if defined(ESP8266)
ICACHE_RAM_ATTR
#elif defined(ESP32)
IRAM_ATTR
#endif
dscKeybusInterface::dscClockInterrupt() {

  // Data sent from the panel and keypads/modules has latency after a clock change (observed up to 160us for
  // keypad data).  The following sets up a timer for each platform that will call dscDataInterrupt() in
  // 250us to read the data line.

  // AVR Timer1 calls dscDataInterrupt() via ISR(TIMER1_OVF_vect) when the Timer1 counter overflows
  #if defined(__AVR__)
  TCNT1 = 61535; // Timer1 counter start value, overflows at 65535 in 250us
  TCCR1B |= (1 << CS10); // Sets the prescaler to 1

  // esp8266 timer1 calls dscDataInterrupt() in 250us
  #elif defined(ESP8266)
  timer1_write(1250);

  // esp32 timer1 calls dscDataInterrupt() in 250us
  #elif defined(ESP32)
  #if ESP_IDF_VERSION_MAJOR < 444
  timerStart(timer1);
  #else // IDF4+
  esp_timer_start_once(timer0, 250);
  #endif
  portENTER_CRITICAL( & timer1Mux);
  #endif

  static unsigned long previousClockHighTime;
  static bool skipData = false;
  #ifdef DEBOUNCE
  static bool skipFirst = false;
  #endif  

  // Panel sends data while the clock is high
  if (digitalRead(dscClockPin) == HIGH) {
    if (virtualKeypad) 
        digitalWrite(dscWritePin, LOW); // Restores the data line after a virtual keypad write
    previousClockHighTime = micros();
  }

  // Keypads and modules send data while the clock is low
  else {
    // Saves data and resets counters after the clock cycle is complete (high for at least 1ms)
    if (micros() - previousClockHighTime > 1000) { // Tracks the clock high time to find the reset between commands
      keybusTime = millis();
      // Skips incomplete and redundant data from status commands - these are sent constantly on the keybus at a high
      // rate, so they are always skipped.  Checking is required in the ISR to prevent flooding the buffer.
     
      if (isrPanelBitTotal < 8) 
          skipData = true;
      else {
      byte * pcmd=NULL;
      static byte previousCmd05[dscReadSize];
      static byte previousCmd1B[dscReadSize]; 
      
      switch (isrPanelData[0]) {
        case 0x05: pcmd=previousCmd05;break;
        case 0x1B: pcmd=previousCmd1B;break;
     }
      if (pcmd!=NULL) {
        if (redundantPanelData(pcmd, isrPanelData, isrPanelByteCount)) {
#ifdef DEBOUNCE            
          if (skipFirst) 
            skipFirst = false;
           else 
#endif              
              skipData = true;
        } 
#ifdef DEBOUNCE        
       else { // we skip the first cmd to remove spurious invalid ones during a changeover. Reported on a pc5005
          skipData = true;
          skipFirst = true;
        }  
#endif        
       }
      }
      // Stores new panel data in the panel buffer
      if (panelBufferLength == dscBufferSize) 
          bufferOverflow = true;
      else if (!skipData && panelBufferLength < dscBufferSize) {
        //for (byte i = 0; i < dscReadSize; i++) panelBuffer[panelBufferLength][i] = isrPanelData[i];
        memcpy((void*)&panelBuffer[panelBufferLength],(void*)isrPanelData,dscReadSize);       
        panelBufferBitCount[panelBufferLength] = isrPanelBitTotal;
        panelBufferByteCount[panelBufferLength] = isrPanelByteCount;
        panelBufferLength++;
      }

      if (processModuleData) {

        // Stores new keypad and module data - this data is not buffered
        if (moduleDataDetected) {
          moduleCmd = isrPanelData[0];
          moduleSubCmd = isrPanelData[2];
          moduleDataDetected = false;
          moduleDataCaptured = true; // Sets a flag for handleModule()
         // for (byte i = 0; i < dscReadSize; i++) moduleData[i] = isrModuleData[i];
          memcpy((void*)moduleData,(void*)isrModuleData,dscReadSize);
          moduleBitCount = isrPanelBitTotal;
          moduleByteCount = isrPanelByteCount;
        }

        // Resets the keypad and module capture data
        //for (byte i = 0; i < dscReadSize; i++) isrModuleData[i] = 0;
        memset((void*)isrModuleData,0,dscReadSize);       
      }

      // Resets the panel capture data and counters
      //for (byte i = 0; i < dscReadSize; i++) isrPanelData[i] = 0;
      memset((void*)isrPanelData,0,dscReadSize);
      isrPanelBitTotal = 0;
      isrPanelBitCount = 0;
      isrPanelByteCount = 0;
      skipData = false;
    }

    // Virtual keypad
    if (virtualKeypad && writeDataPending && writeBufferIdx < writeBufferLength) {
      static bool writeStart = false;
      if (isrPanelBitTotal == writeDataBit || (writeStart && isrPanelBitTotal > writeDataBit && isrPanelBitTotal < (writeDataBit + (writeBufferLength * 8)))) {

        writeStart = true;
        if (!((writeBuffer[writeBufferIdx] >> (7 - isrPanelBitCount)) & 0x01)) digitalWrite(dscWritePin, HIGH);

        if (isrPanelBitCount == 7) {
          writeBufferIdx++;

          if (writeBufferIdx == writeBufferLength) { //all bits written
            writeStart = false;
            writeDataPending = false;
            if (writeAlarm) {
              writeAlarm = false;
              writeBufferIdx = 0; //reset byte counter to resend
              writeDataPending = true;
            }
          }
        }
<<<<<<< HEAD
      } 
      else if (isrPanelData[0]==moduleResponseCmd && writeModulePending && !wroteAsterisk ) {
        if (isrPanelBitTotal == writeModuleBit || (writeStart && isrPanelBitTotal > writeModuleBit && isrPanelBitTotal <= writeModuleBit + (moduleBufferLength * 8))) {
           writeStart=true;
          if (!((writeModuleBuffer[currentModuleIdx] >> (7 - isrPanelBitCount)) & 0x01)) digitalWrite(dscWritePin, HIGH);
          // Resets counters when the write is complete
          if (isrPanelBitTotal == writeModuleBit + (moduleBufferLength * 8)) {
                writeStart = false;
                writeModulePending=false;
                moduleResponseCmd=0;
          }  else if (isrPanelBitCount==7) {
              currentModuleIdx++;
              if (currentModuleIdx==moduleBufferLength) {
                   writeStart = false;
                   writeModulePending=false;
                   moduleResponseCmd=0;
              }
          }
        }
=======

>>>>>>> new
      }
    }
  }
  #if defined(ESP32)
  portEXIT_CRITICAL( & timer1Mux);
  #endif
}

// Interrupt function called by AVR Timer1, esp8266 timer1, and esp32 timer1 after 250us to read the data line
void
#if defined(ESP8266)
ICACHE_RAM_ATTR
#elif defined(ESP32)
IRAM_ATTR
#endif
  #if ESP_IDF_VERSION_MAJOR < 444
dscKeybusInterface::dscDataInterrupt() {
    #else
  dscKeybusInterface::dscDataInterrupt( void* arg) {      
    #endif
  #if defined(ESP32)
  #if ESP_IDF_VERSION_MAJOR < 444
  timerStop(timer1);
  #else // IDF 4+
  esp_timer_stop(timer0);
  #endif
  portENTER_CRITICAL( & timer1Mux);
  #endif

  // Panel sends data while the clock is high
  if (digitalRead(dscClockPin) == HIGH) {

    // Reads panel data and sets data counters
    if (isrPanelByteCount < dscReadSize) { // Limits Keybus data bytes to dscReadSize
      if (isrPanelBitCount < 8) {
        // Data is captured in each byte by shifting left by 1 bit and writing to bit 0
        isrPanelData[isrPanelByteCount] <<= 1;
        if (digitalRead(dscReadPin) == HIGH) {
          isrPanelData[isrPanelByteCount] |= 1;
        }
      }

      if (isrPanelBitTotal == 7) {
        processPendingResponses(isrPanelData[0]);

      }  else

      if (isrPanelBitTotal == 15 && isrPanelData[0] == 0xE6) {
        processPendingResponses_0xE6(isrPanelData[2]); //check subcommand
      } 

      // Stores the stop bit by itself in byte 1 - this aligns the Keybus bytes with panelData[] bytes
      if (isrPanelBitTotal == 8) {

        isrPanelBitCount = 0;
        isrPanelByteCount++;
      }

      // Increments the bit counter if the byte is incomplete
      else if (isrPanelBitCount < 7) {
        isrPanelBitCount++;
      }

      // Byte is complete, set the counters for the next byte
      else {
        isrPanelBitCount = 0;
        isrPanelByteCount++;
      }

      isrPanelBitTotal++;
    }
  }

  // Keypads and modules send data while the clock is low
  else {

    // Keypad and module data is not buffered and skipped if the panel data buffer is filling
    if (processModuleData && isrPanelByteCount < dscReadSize && panelBufferLength <= 1) {

      // Data is captured in each byte by shifting left by 1 bit and writing to bit 0
      if (isrPanelBitCount < 8) {
        isrModuleData[isrPanelByteCount] <<= 1;
        if (digitalRead(dscReadPin) == HIGH) {
          isrModuleData[isrPanelByteCount] |= 1;
        } else {
          moduleDataDetected = true; // Keypads and modules send data by pulling the data line low
        }
      }

      // Stores the stop bit by itself in byte 1 - this aligns the Keybus bytes with moduleData[] bytes
      if (isrPanelBitTotal == 8) {
        isrModuleData[1] = 1; // Sets the stop bit manually to 1 in byte 1
      }
    }
  }
  #if defined(ESP32)
  portEXIT_CRITICAL( & timer1Mux);
  #endif

}

void
#if defined(ESP8266)
ICACHE_RAM_ATTR
#elif defined(ESP32)
IRAM_ATTR
#endif
dscKeybusInterface::writeCharsToQueue(byte * keys,byte partition, byte len, bool alarm) {
  writeQueueType req;
  req.len = len;
  memcpy(req.data,keys,len);
  req.alarm = alarm;
  req.writeBit = partitionToBits[partition];
  req.partition=partition;
  writeQueue[inIdx] = req;
  inIdx = (inIdx + 1) % writeQueueSize; //circular buffer - increment index
 
}

void
#if defined(ESP8266)
ICACHE_RAM_ATTR
#elif defined(ESP32)
IRAM_ATTR
#endif
dscKeybusInterface::dscKeybusInterface::updateWriteBuffer(byte * src, int bit,byte partition,int len,  bool alarm) {

  writeBufferLength = len;
  writeDataBit = bit;
  writeBufferIdx = 0;
  writeAlarm = alarm;
  writePartition=partition;
  writeBuffer=src;
  writeDataPending = true; //set flag to send it  
}

void
#if defined(ESP8266)
ICACHE_RAM_ATTR
#elif defined(ESP32)
IRAM_ATTR
#endif
dscKeybusInterface::dscKeybusInterface::processPendingResponses(byte cmd) {

  if (writeDataPending) return;

  switch (cmd) {
  case 0x1B:
  case 0x0A:
  case 0x05:
    processPendingQueue(cmd);
    return;
#if defined(EXPANDER)    
  case 0x11:
    if (!enableModuleSupervision) return;
    updateWriteBuffer((byte * ) moduleSlots, 9,1,maxFields11 );
    return; //setup supervisory slot response for devices
  case 0x28:
    prepareModuleResponse(9, 9);
    return; // the address will depend on the panel request command for the module
  case 0x33:
    prepareModuleResponse(10, 9);
    return;
  case 0x39:
    prepareModuleResponse(11, 9);
    return;
#endif    
  case 0x70:
    if (pending70) {
        pending70=false;
        processCmd70();
    }
    return; 
  case 0xD0:
    if (pendingD0) {
        pendingD0=false;
        updateWriteBuffer((byte *) cmdD0buffer,9,1,6);
    }
    return;

  default:
    return;
  }

}
void
#if defined(ESP8266)
ICACHE_RAM_ATTR
#elif defined(ESP32)
IRAM_ATTR
#endif
dscKeybusInterface::processPendingQueue(byte cmd) {

  //process queued 05/0b/1b requests
  if (inIdx == outIdx || (writeQueue[outIdx].partition > 4 && (cmd == 0x05 || cmd ==
 0x0A) ) || (cmd == 0x1B && writeQueue[outIdx].partition < 5)) return;
 
    updateWriteBuffer((byte * ) writeQueue[outIdx].data,  writeQueue[outIdx].writeBit,writeQueue[outIdx].partition,writeQueue[outIdx].len, writeQueue[outIdx].alarm); //populate write buffer and set ready to send flag
    outIdx = (outIdx + 1) % writeQueueSize; // advance index to next record
}

void
#if defined(ESP8266)
ICACHE_RAM_ATTR
#elif defined(ESP32)
IRAM_ATTR
#endif
dscKeybusInterface::processPendingResponses_0xE6(byte subcmd) {

  if (writeDataPending) return;
 
  switch (subcmd) {

#if defined(EXPANDER)
  case 0x08:
    prepareModuleResponse(12, 17);
    break;
  case 0x0A:
    prepareModuleResponse(13, 17);;
    break;
  case 0x0C:
    prepareModuleResponse(14, 17);;
    break;
  case 0x0E:
    prepareModuleResponse(16, 17);;
    break;
#endif
  default:
    return;
  }

}