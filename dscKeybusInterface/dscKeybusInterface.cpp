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

#include "dscKeybus.h"


#if defined(ESP32)
portMUX_TYPE dscKeybusInterface::timer1Mux = portMUX_INITIALIZER_UNLOCKED;

#if ESP_IDF_VERSION_MAJOR < 4
hw_timer_t * dscKeybusInterface::timer1 = NULL;

#else // ESP-IDF 4+
esp_timer_handle_t timer0;
const esp_timer_create_args_t timer0Parameters = {
  .callback = reinterpret_cast < esp_timer_cb_t > ( & dscKeybusInterface::dscDataInterrupt)
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
  #if ESP_IDF_VERSION_MAJOR < 4
  timer1 = timerBegin(0, 80, true);
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
  #if ESP_IDF_VERSION_MAJOR < 4
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
      //start expander
      updateModules();
      //end expander
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
  //if (millis() - starWaitTime > 5000 && starKeyCheck && writeDataPending) { // timeout after no response from * write
      // starKeyWait[partition-1] = false;
       //starKeyCheck = false;
     //  writeDataPending = false;
	//}

  return true;
}

bool dscKeybusInterface::handleModule() {
  if (!moduleDataCaptured) return false;
  moduleDataCaptured = false;

  if (moduleBitCount < 8) return false;

  // Determines if a keybus message is a response to a panel command
  switch (moduleCmd) {
  case 0x11:
  case 0x28:
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
  bool isStar = false;
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
      if (status[partition - 1] < 0x9E)  {
          isStar = true;
            starWaitTime=millis();            
      }
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
      writeCharsToQueue((byte * ) & writeKey, 0,1, isAlarm, false);
    else
      writeCharsToQueue((byte * ) & writeKey, partition, 1, false, isStar);

  }

}

bool
#if defined(ESP8266)
ICACHE_RAM_ATTR
#elif defined(ESP32)
IRAM_ATTR
#endif
dscKeybusInterface::redundantPanelData(byte  previousCmd[] , volatile byte  currentCmd[], byte checkedBytes) {

  bool redundantData = true;
  for (byte i = 0; i < checkedBytes; i++) {
    if (previousCmd[i] != currentCmd[i]) {
      redundantData = false;
      memcpy((void*)previousCmd,(void*)currentCmd,dscReadSize);      
      break;
    }
  }
  return redundantData;
}

bool dscKeybusInterface::validCRC() {
  byte byteCount = (panelBitCount - 1) / 8;
  int dataSum = 0;
  for (byte panelByte = 0; panelByte < byteCount; panelByte++) {
    if (panelByte != 1) dataSum += panelData[panelByte];
  }
  if (dataSum % 256 == panelData[byteCount]) return true;
  else return false;
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
  #if ESP_IDF_VERSION_MAJOR < 4
  timerStart(timer1);
  #else // IDF4+
  esp_timer_start_periodic(timer0, 250);
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
    if (virtualKeypad) digitalWrite(dscWritePin, LOW); // Restores the data line after a virtual keypad write
    previousClockHighTime = micros();
  }

  // Keypads and modules send data while the clock is low
  else {
    clockHighTime = micros() - previousClockHighTime; // Tracks the clock high time to find the reset between commands

    // Saves data and resets counters after the clock cycle is complete (high for at least 1ms)
    if (clockHighTime > 1000) {
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
          if (skipFirst) {
            skipFirst = false;
          } else 
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
      if (panelBufferLength == dscBufferSize) bufferOverflow = true;
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
dscKeybusInterface::dscDataInterrupt() {
  #if defined(ESP32)
  #if ESP_IDF_VERSION_MAJOR < 4
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
dscKeybusInterface::writeCharsToQueue(byte * keys,byte partition, byte len, bool alarm, bool star) {
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
dscKeybusInterface::dscKeybusInterface::updateWriteBuffer(byte * src, int bit,byte partition,int len,  bool alarm, bool star) {

  writeBufferLength = len;
  writeDataBit = bit;
  writeBufferIdx = 0;
  writeAlarm = alarm;
  starKeyCheck = false;
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
  if (inIdx == outIdx || (writeQueue[outIdx].partition > 4 && (cmd == 0x05 || cmd == 0x0A) ) || (cmd == 0x1B && writeQueue[outIdx].partition < 5)) return;
    updateWriteBuffer((byte * ) writeQueue[outIdx].data,  writeQueue[outIdx].writeBit,writeQueue[outIdx].partition,writeQueue[outIdx].len, writeQueue[outIdx].alarm, writeQueue[outIdx].star); //populate write buffer and set ready to send flag
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