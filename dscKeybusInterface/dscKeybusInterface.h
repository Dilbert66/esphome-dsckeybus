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
 */

#ifndef dscKeybusInterface_h
#define dscKeybusInterface_h

#if defined ESP32 && ESP_IDF_VERSION_MAJOR >= 4
#include "esp_timer.h"
#endif

<<<<<<< HEAD
    // Write control
    static byte writePartition;                       // Set to a partition number for virtual keypad
    bool writeReady;                                  // True if the library is ready to write a key

    // Prints output to the stream interface set in begin()
    void printPanelBinary(bool printSpaces = true);   // Includes spaces between bytes by default
    void printPanelCommand();                         // Prints the panel command as hex
    void printPanelMessage();                         // Prints the decoded panel message
    void printModuleBinary(bool printSpaces = true);  // Includes spaces between bytes by default
    void printModuleMessage();                        // Prints the decoded keypad or module message


    // These can be configured in the sketch setup() before begin()
    bool hideKeypadDigits;          // Controls if keypad digits are hidden for publicly posted logs (default: false)
    bool processRedundantData;      // Controls if repeated periodic commands are processed and displayed (default: false)
    static bool processModuleData;  // Controls if keypad and module data is processed and displayed (default: false)
    bool displayTrailingBits;       // Controls if bits read as the clock is reset are displayed, appears to be spurious data (default: false)

    // Panel time
    bool timestampChanged;          // True after the panel sends a timestamped message
    byte hour, minute, day, month;
    int year;

    // Sets panel time, the year can be sent as either 2 or 4 digits, returns true if the panel is ready to set the time
    bool setTime(unsigned int year, byte month, byte day, byte hour, byte minute, const char* accessCode);

    // Status tracking
    bool statusChanged;                   // True after any status change
    bool pauseStatus;                     // Prevent status from showing as changed, set in sketch to control when to update status
    bool keybusConnected, keybusChanged;  // True if data is detected on the Keybus
    byte accessCode[dscPartitions];
    bool accessCodeChanged[dscPartitions];
    bool accessCodePrompt;                // True if the panel is requesting an access code
    bool trouble, troubleChanged;
    bool powerTrouble, powerChanged;
    bool batteryTrouble, batteryChanged;
    bool keypadFireAlarm, keypadAuxAlarm, keypadPanicAlarm;
    bool ready[dscPartitions], readyChanged[dscPartitions];
    bool disabled[dscPartitions];
    bool armed[dscPartitions], armedAway[dscPartitions], armedStay[dscPartitions];
    bool noEntryDelay[dscPartitions], armedChanged[dscPartitions];
    bool alarm[dscPartitions], alarmChanged[dscPartitions];
    bool exitDelay[dscPartitions], exitDelayChanged[dscPartitions];
    byte exitState[dscPartitions], exitStateChanged[dscPartitions];
    bool entryDelay[dscPartitions], entryDelayChanged[dscPartitions];
    bool fire[dscPartitions], fireChanged[dscPartitions];
    bool openZonesStatusChanged;
    byte openZones[dscZones], openZonesChanged[dscZones];    // Zone status is stored in an array using 1 bit per zone, up to 64 zones
    bool alarmZonesStatusChanged;
    byte alarmZones[dscZones], alarmZonesChanged[dscZones];  // Zone alarm status is stored in an array using 1 bit per zone, up to 64 zones
    static bool debounce05;
    void setZoneFault(byte zone,bool fault) ;
    void addEmulatedZone(byte address);
    void removeEmulatedZone(byte address);
    void addModule(byte address); //add zone expanders
    void updateModules();
    void addRelayModule(); 
    void clearZoneRanges();
    static byte maxZones;
    static byte panelVersion;
    static bool enableModuleSupervision;
    
    // panelData[] and moduleData[] store panel and keypad data in an array: command [0], stop bit by itself [1],
    // followed by the remaining data.  These can be accessed directly in the sketch to get data that is not already
    // tracked in the library.  See dscKeybusPrintData-RTOS.c for the currently known DSC commands and data.
    //
    // panelData[] example:
    //   Byte 0     Byte 2   Byte 3   Byte 4   Byte 5
    //   00000101 0 10000001 00000001 10010001 11000111 [0x05] Status lights: Ready Backlight | Partition ready
    //            ^ Byte 1 (stop bit)
    static byte panelData[dscReadSize];
    static volatile byte moduleData[dscReadSize];
	
    // status[] and lights[] store the current status message and LED state for each partition.  These can be accessed
    // directly in the sketch to get data that is not already tracked in the library.  See printPanelMessages() and
    // printPanelLights() in dscKeybusPrintData.cpp to see how this data translates to the status message and LED status.
    byte status[dscPartitions];
    byte lights[dscPartitions];
    
    // Process keypad and module data, returns true if data is available
    bool handleModule();

    // True if dscBufferSize needs to be increased
    static volatile bool bufferOverflow;

    // Timer interrupt function to capture data - declared as public for use by AVR Timer2
    static void dscDataInterrupt();
    //static void dscDataInterrupt(dscKeybusInterface *dscPointer);
    // Deprecated
    bool handlePanel();               // Returns true if valid panel data is available.  Relabeled to loop()
    static volatile byte currentCmd;
    static volatile byte statusCmd,moduleCmd,moduleSubCmd;
        
  private:
    
    void processPanelStatus();
    void processPanelStatus0(byte partition, byte panelByte);
    void processPanelStatus2(byte partition, byte panelByte);
    void processPanelStatus4(byte partition, byte panelByte);
    void processPanel_0x27();
    void processPanel_0x2D();
    void processPanel_0x34();
    void processPanel_0x3E();
    void processPanel_0xA5();
    void processPanel_0xE6();
    void processPanel_0xE6_0x09();
    void processPanel_0xE6_0x0B();
    void processPanel_0xE6_0x0D();
    void processPanel_0xE6_0x0F();
    void processPanel_0xEB();

    void printPanelLights(byte panelByte);
    void printPanelMessages(byte panelByte);
    void printPanelBitNumbers(byte panelByte, byte startNumber);
    void printPanelStatus0(byte panelByte);
    void printPanelStatus1(byte panelByte);
    void printPanelStatus2(byte panelByte);
    void printPanelStatus3(byte panelByte);
    void printPanelStatus4(byte panelByte);
    void printPanelStatus14(byte panelByte);
    void printPanel_0x05();
    void printPanel_0x0A();
    void printPanel_0x11();
    void printPanel_0x16();
    void printPanel_0x1B();
    void printPanel_0x1C();
    void printPanel_0x27();
    void printPanel_0x28();
    void printPanel_0x2D();
    void printPanel_0x33();
    void printPanel_0x34();
    void printPanel_0x39();    
    void printPanel_0x3E();
    void printPanel_0x4C();
    void printPanel_0x58();
    void printPanel_0x5D();
    void printPanel_0x63();
    void printPanel_0x64();
    void printPanel_0x69();
    void printPanel_0x75();
    void printPanel_0x7A();
    void printPanel_0x7F();
    void printPanel_0x82();
    void printPanel_0x87();
    void printPanel_0x8D();
    void printPanel_0x94();
    void printPanel_0xA5();
    void printPanel_0xB1();
    void printPanel_0xBB();
    void printPanel_0xC3();
    void printPanel_0xCE();
    void printPanel_0xD5();
    void printPanel_0xE6();
    void printPanel_0xE6_0x03();
    void printPanel_0xE6_0x08();    
    void printPanel_0xE6_0x09();
    void printPanel_0xE6_0x0A();    
    void printPanel_0xE6_0x0B();
    void printPanel_0xE6_0x0C();    
    void printPanel_0xE6_0x0D();
    void printPanel_0xE6_0x0E();    
    void printPanel_0xE6_0x0F();
    void printPanel_0xE6_0x17();
    void printPanel_0xE6_0x18();
    void printPanel_0xE6_0x19();
    void printPanel_0xE6_0x1A();
    void printPanel_0xE6_0x1D();
    void printPanel_0xE6_0x20();
    void printPanel_0xE6_0x2B();
    void printPanel_0xE6_0x2C();
    void printPanel_0xE6_0x41();
    void printPanel_0xEB();

    void printModule_0x77();
    void printModule_0xBB();
    void printModule_0xDD();
    void printModule_Panel_0x11();
    void printModule_Panel_0xD5();
    void printModule_Notification();
    void printModule_Keys();
    
    void removeModule(byte address);
    static void setPendingZoneUpdate();
    static void processModuleResponse(byte cmd);
    static void processModuleResponse_0xE6(byte cmd);
    static void addRequestToQueue(byte slot);
    static void setSupervisorySlot(byte slot,bool set);
    static byte getPendingUpdate();
    static void fillBuffer(byte* src,int len);
    static zoneMaskType getUpdateMask(byte address);
    static void prepareResponse(byte);
   // static void setBuffer(const void* src,int len);
    //static volatile byte updateQueue[updateQueueSize];
    static byte outIdx,inIdx;
    static byte moduleIdx; 
    static moduleType modules[maxModules];
    static byte moduleSlots[6];
    volatile static byte pendingZoneStatus[6];
    volatile static byte writeModuleBuffer[6];
    bool validCRC();
    void writeKeys(const char * writeKeysArray);
    void setWriteKey(const char receivedKey);
    static void dscClockInterrupt();
    static bool redundantPanelData(byte previousCmd[], volatile byte currentCmd[], byte checkedBytes = dscReadSize);

    Stream* stream;
    const char* writeKeysArray;
    bool writeKeysPending;
    bool writeArm[dscPartitions];
    bool queryResponse;
    static byte maxFields05; 
    static byte maxFields11;
	
    bool previousKeybus;
    byte previousAccessCode[dscPartitions];
    byte previousLights[dscPartitions], previousStatus[dscPartitions];
    bool previousReady[dscPartitions];
	bool previousExitDelay[dscPartitions], previousEntryDelay[dscPartitions];
    byte previousExitState[dscPartitions];
    bool previousArmed[dscPartitions], previousArmedStay[dscPartitions];
    bool previousAlarm[dscPartitions];
    bool previousFire[dscPartitions];
	bool previousTrouble;
    byte previousOpenZones[dscZones], previousAlarmZones[dscZones];
   
    static byte dscClockPin;
    static byte dscReadPin;
    static byte dscWritePin;
    static byte writeByte, writeBit,writeModuleBit;
    static bool virtualKeypad;
    static char writeKey;
    static byte panelBitCount, panelByteCount;
    static volatile bool writeKeyPending,writeModulePending,pendingDeviceUpdate;
    static volatile bool writeAlarm, writeAsterisk, wroteAsterisk;
    static volatile bool moduleDataCaptured;
	static volatile unsigned long clockHighTime, keybusTime;
    static volatile byte panelBufferLength,moduleBufferLength,currentModuleIdx,moduleResponseCmd;
    static volatile byte panelBuffer[dscBufferSize][dscReadSize];
    static volatile byte panelBufferBitCount[dscBufferSize], panelBufferByteCount[dscBufferSize];
    static volatile byte moduleBitCount, moduleByteCount;

    static volatile byte isrPanelData[dscReadSize], isrPanelBitTotal, isrPanelBitCount, isrPanelByteCount;
    static volatile byte isrModuleData[dscReadSize], isrModuleBitTotal, isrModuleBitCount, isrModuleByteCount;
};
=======
// DSC Classic Series
#if defined dscClassicSeries
#include "dscClassic.h"

byte dscClassicInterface::dscClockPin;
byte dscClassicInterface::dscReadPin;
byte dscClassicInterface::dscPC16Pin;
byte dscClassicInterface::dscWritePin;
char dscClassicInterface::writeKey;
byte dscClassicInterface::writePartition;
bool dscClassicInterface::virtualKeypad;
bool dscClassicInterface::processModuleData;
byte dscClassicInterface::panelData[dscReadSize];
byte dscClassicInterface::pc16Data[dscReadSize];
byte dscClassicInterface::panelByteCount;
byte dscClassicInterface::panelBitCount;
volatile bool dscClassicInterface::writeKeyWait;
volatile byte dscClassicInterface::moduleData[dscReadSize];
volatile bool dscClassicInterface::moduleDataCaptured;
volatile byte dscClassicInterface::moduleByteCount;
volatile byte dscClassicInterface::moduleBitCount;
volatile bool dscClassicInterface::writeAlarm;
volatile bool dscClassicInterface::bufferOverflow;
volatile byte dscClassicInterface::panelBufferLength;
volatile byte dscClassicInterface::panelBuffer[dscBufferSize][dscReadSize];
volatile byte dscClassicInterface::pc16Buffer[dscBufferSize][dscReadSize];
volatile byte dscClassicInterface::panelBufferBitCount[dscBufferSize];
volatile byte dscClassicInterface::panelBufferByteCount[dscBufferSize];
volatile byte dscClassicInterface::isrPanelData[dscReadSize];
volatile byte dscClassicInterface::isrPC16Data[dscReadSize];
volatile byte dscClassicInterface::isrPanelByteCount;
volatile byte dscClassicInterface::isrPanelBitCount;
volatile byte dscClassicInterface::isrPanelBitTotal;
volatile byte dscClassicInterface::isrModuleData[dscReadSize];
volatile byte dscClassicInterface::isrModuleByteCount;
volatile byte dscClassicInterface::isrModuleBitCount;
volatile byte dscClassicInterface::isrModuleBitTotal;
volatile byte dscClassicInterface::moduleCmd;
volatile bool dscClassicInterface::readyLight;
volatile bool dscClassicInterface::lightBlink;
//volatile unsigned long dscClassicInterface::clockHighTime;
volatile unsigned long dscClassicInterface::keybusTime;
volatile unsigned long dscClassicInterface::writeCompleteTime;

// Interrupt function called after 250us by dscClockInterrupt() using AVR Timer1, disables the timer and calls
// dscDataInterrupt() to read the data line
#if defined(__AVR__)
ISR(TIMER1_OVF_vect) {
  TCCR1B = 0;  // Disables Timer1
  dscClassicInterface::dscDataInterrupt();
}
#endif  // __AVR__


// DSC Keypad Interface
#elif defined dscKeypad

#include "dscKeypad.h"

byte dscKeypadInterface::dscClockPin;
byte dscKeypadInterface::dscReadPin;
byte dscKeypadInterface::dscWritePin;
int  dscKeypadInterface::clockInterval;
volatile byte dscKeypadInterface::keyData;
volatile byte dscKeypadInterface::keyBufferLength;
volatile byte dscKeypadInterface::keyBuffer[dscBufferSize];
volatile bool dscKeypadInterface::bufferOverflow;
volatile bool dscKeypadInterface::commandReady;
volatile bool dscKeypadInterface::moduleDataDetected;
volatile bool dscKeypadInterface::alarmKeyDetected;
volatile bool dscKeypadInterface::alarmKeyResponsePending;
volatile byte dscKeypadInterface::clockCycleCount;
volatile byte dscKeypadInterface::clockCycleTotal;
volatile byte dscKeypadInterface::panelCommand[dscReadSize];
volatile byte dscKeypadInterface::isrPanelBitTotal;
volatile byte dscKeypadInterface::isrPanelBitCount;
volatile byte dscKeypadInterface::panelCommandByteCount;
volatile byte dscKeypadInterface::isrModuleData[dscReadSize];
volatile byte dscKeypadInterface::isrModuleBitTotal;
volatile byte dscKeypadInterface::isrModuleBitCount;
volatile byte dscKeypadInterface::isrModuleByteCount;
volatile byte dscKeypadInterface::panelCommandByteTotal;
volatile byte dscKeypadInterface::moduleData[dscReadSize];

#if defined(__AVR__)
ISR(TIMER1_OVF_vect) {
  dscKeypadInterface::dscClockInterrupt();
}
#endif  // __AVR__

// DSC Classic Keypad Interface
#elif defined dscClassicKeypad

#include "dscClassicKeypad.h"

byte dscClassicKeypadInterface::dscClockPin;
byte dscClassicKeypadInterface::dscReadPin;
byte dscClassicKeypadInterface::dscWritePin;
int  dscClassicKeypadInterface::clockInterval;
volatile byte dscClassicKeypadInterface::keyData;
volatile byte dscClassicKeypadInterface::keyBufferLength;
volatile byte dscClassicKeypadInterface::keyBuffer[dscBufferSize];
volatile bool dscClassicKeypadInterface::bufferOverflow;
volatile bool dscClassicKeypadInterface::commandReady;
volatile bool dscClassicKeypadInterface::moduleDataDetected;
volatile bool dscClassicKeypadInterface::alarmKeyDetected;
volatile bool dscClassicKeypadInterface::alarmKeyResponsePending;
volatile byte dscClassicKeypadInterface::clockCycleCount;
volatile byte dscClassicKeypadInterface::clockCycleTotal;
volatile byte dscClassicKeypadInterface::panelCommand[dscReadSize];
volatile byte dscClassicKeypadInterface::isrPanelBitTotal;
volatile byte dscClassicKeypadInterface::isrPanelBitCount;
volatile byte dscClassicKeypadInterface::panelCommandByteCount;
volatile byte dscClassicKeypadInterface::isrModuleData[dscReadSize];
volatile byte dscClassicKeypadInterface::isrModuleBitTotal;
volatile byte dscClassicKeypadInterface::isrModuleBitCount;
volatile byte dscClassicKeypadInterface::isrModuleByteCount;
volatile byte dscClassicKeypadInterface::panelCommandByteTotal;
volatile byte dscClassicKeypadInterface::moduleData[dscReadSize];
volatile unsigned long dscClassicKeypadInterface::intervalStart;
volatile unsigned long dscClassicKeypadInterface::beepInterval;
volatile unsigned long dscClassicKeypadInterface::repeatInterval;
volatile unsigned long dscClassicKeypadInterface::keyInterval;
volatile unsigned long dscClassicKeypadInterface::alarmKeyTime;
volatile unsigned long dscClassicKeypadInterface::alarmKeyInterval;
>>>>>>> new

#if defined(__AVR__)
ISR(TIMER1_OVF_vect) {
  dscClassicKeypadInterface::dscClockInterrupt();
}
#endif  // __AVR__


// DSC PowerSeries
#else
#include "dscKeybus.h"

byte dscKeybusInterface::dscClockPin;
byte dscKeybusInterface::dscReadPin;
byte dscKeybusInterface::dscWritePin;
char dscKeybusInterface::writeKey;
bool dscKeybusInterface::virtualKeypad;
bool dscKeybusInterface::processModuleData;
byte dscKeybusInterface::panelData[dscReadSize];
byte dscKeybusInterface::panelByteCount;
byte dscKeybusInterface::panelBitCount;
volatile byte dscKeybusInterface::moduleData[dscReadSize];
volatile bool dscKeybusInterface::moduleDataCaptured;
volatile bool dscKeybusInterface::moduleDataDetected;
volatile byte dscKeybusInterface::moduleByteCount;
volatile byte dscKeybusInterface::moduleBitCount;
volatile bool dscKeybusInterface::writeAlarm;
volatile bool dscKeybusInterface::bufferOverflow;
volatile byte dscKeybusInterface::panelBufferLength;
volatile byte dscKeybusInterface::panelBuffer[dscBufferSize][dscReadSize];
volatile byte dscKeybusInterface::panelBufferBitCount[dscBufferSize];
volatile byte dscKeybusInterface::panelBufferByteCount[dscBufferSize];
volatile byte dscKeybusInterface::isrPanelData[dscReadSize];
volatile byte dscKeybusInterface::isrPanelByteCount;
volatile byte dscKeybusInterface::isrPanelBitCount;
volatile byte dscKeybusInterface::isrPanelBitTotal;
volatile byte dscKeybusInterface::isrModuleData[dscReadSize];
volatile byte dscKeybusInterface::moduleCmd;
volatile byte dscKeybusInterface::moduleSubCmd;
volatile unsigned long dscKeybusInterface::keybusTime;
byte dscKeybusInterface::moduleSlots[6];
moduleType dscKeybusInterface::modules[maxModules];
byte dscKeybusInterface::moduleIdx;
bool dscKeybusInterface::enableModuleSupervision;
byte dscKeybusInterface::maxFields05; 
byte dscKeybusInterface::maxFields11;

writeQueueType dscKeybusInterface::writeQueue[writeQueueSize];
Stream* dscKeybusInterface::stream;


byte * dscKeybusInterface::writeBuffer;
byte dscKeybusInterface::cmdD0buffer[6];
bool dscKeybusInterface::pendingD0;
bool dscKeybusInterface::pending70;
bool dscKeybusInterface::pending6E;

volatile byte dscKeybusInterface::writePartition;
volatile byte dscKeybusInterface::writeBufferIdx;
volatile byte dscKeybusInterface::writeBufferLength;
volatile bool dscKeybusInterface::writeDataPending;
byte dscKeybusInterface::writeDataBit;
volatile pgmBufferType dscKeybusInterface::pgmBuffer;
volatile byte dscKeybusInterface::inIdx;
volatile byte dscKeybusInterface::outIdx;
byte dscKeybusInterface::maxZones;
byte dscKeybusInterface::panelVersion;
// Interrupt function called after 250us by dscClockInterrupt() using AVR Timer1, disables the timer and calls
// dscDataInterrupt() to read the data line
#if defined(__AVR__)
ISR(TIMER1_OVF_vect) {
  TCCR1B = 0;  // Disables Timer1
  dscKeybusInterface::dscDataInterrupt();
}
#endif  // __AVR__
#endif  // dscClassicSeries, dscKeypadInterface
#endif  // dscKeybusInterface_h
