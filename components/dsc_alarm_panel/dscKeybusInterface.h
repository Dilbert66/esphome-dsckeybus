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

#endif  // dscClassicSeries, dscKeypadInterface
#endif  // dscKeybusInterface_h
