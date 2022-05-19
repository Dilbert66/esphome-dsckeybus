#include "dscKeybus.h"


void dscKeybusInterface::setSupervisorySlot(byte address, bool set = true) {
  //set our response data for the 0x11 supervisory request
  if (panelVersion < 3) {
    switch (address) {
      //for older versions we need to set 2 slots since they expect groups of 4 zones per slot
    case 9:
      moduleSlots[2] = set ? moduleSlots[2] & 0x3f : moduleSlots[2] | ~0x3f; //pc5108 
      moduleSlots[2] = set ? moduleSlots[2] & 0xcf : moduleSlots[2] | ~0xcf;
      break;
    case 10:
      moduleSlots[2] = set ? moduleSlots[2] & 0xf3 : moduleSlots[2] | ~0xf3; //pc5108
      moduleSlots[2] = set ? moduleSlots[2] & 0xfc : moduleSlots[2] | ~0xfc;
      break;
    case 11:
      moduleSlots[3] = set ? moduleSlots[3] & 0x3f : moduleSlots[3] | ~0x3f; //pc5108
      moduleSlots[3] = set ? moduleSlots[3] & 0xcf : moduleSlots[3] | ~0xcf;
      break;
    case 18:
      moduleSlots[3] = set ? moduleSlots[3] & 0xfc : moduleSlots[3] | ~0xfc;
      break; // pc5208 relay board reports as 18 but reports on slot 16
    default:
      return;
    }

  } else {
    switch (address) {
      //11111111 1 00111111 11111111 11111111 11111111 11111111 11111100 11111111 18 
      //11111111 1 00111111 11111111 11111111 00111111 11111111 11111111 11111111 13
      // 1111111 1 00111111 11111111 00111111 11111111 11111111 11111111 11111111 slots 9

    case 9:
      moduleSlots[2] = set ? moduleSlots[2] & 0x3f : moduleSlots[2] | ~0x3f;
      break; //pc5108 
    case 10:
      moduleSlots[2] = set ? moduleSlots[2] & 0xcf : moduleSlots[2] | ~0xcf;
      break; //pc5108
    case 11:
      moduleSlots[2] = set ? moduleSlots[2] & 0xf3 : moduleSlots[2] | ~0xf3;
      break; //pc5108
    case 12:
      if (maxZones > 32) {
        moduleSlots[2] = set ? moduleSlots[2] & 0xfc : moduleSlots[2] | ~0xfc;
      }
      break; //pc5108
    case 13:
      if (maxZones > 32) {
        moduleSlots[3] = set ? moduleSlots[3] & 0x3f : moduleSlots[3] | ~0x3f;
      }
      break; //pc5108
    case 14:
      if (maxZones > 32) {
        moduleSlots[3] = set ? moduleSlots[3] & 0xcf : moduleSlots[3] | ~0xcf;
      }
      break; //pc5108
    case 16:
      if (maxZones > 32) {
        moduleSlots[5] = set ? moduleSlots[5] & 0x3f : moduleSlots[5] | ~0x3f; //slot24
      }
      break; //pc5108 (shows on slot24)// reports as 16 in panel
      //reports as 18 in panel
    case 18:
      moduleSlots[3] = set ? moduleSlots[3] & 0xfc : moduleSlots[3] | ~0xfc; //slot 16
      break; // pc5208 relay board shows on slot 16 but reports as 18
    default:
      return;
    }
  }

}

zoneMaskType dscKeybusInterface::getUpdateMask(byte address) {

  //get our request byte and mask to send data for the zone that we need to publish info on. This gets sent on the 05 command
  //11111111 1 11111111 11111111 11111111 11111111 11111111 01111111 11111111 11111111 (12)
  //11111111 1 11111111 11111111 10111111 11111111 11111111 11111111 11111111 11111111 (9)
  //11111111 1 11111111 11111111 11111111 11111101 11111111 11111111 11111111 11111111   it-100
  //11111111 1 11111111 11111111 10111111 11111111 version 2 zone 9 -16
  //11111111 1 11111111 11111111 11101111 11111111  version 2 system zone 27-32

  zoneMaskType zm;
  switch (address) {
  case 9:
    zm.idx = 2;
    zm.mask = 0xbf;
    break; //5108
  case 10:
    zm.idx = 2;
    zm.mask = 0xdf;
    break; //5108
  case 11:
    zm.idx = 2;
    zm.mask = 0xef;
    break; //5108
  case 12:
    if (maxZones > 32) {
      zm.idx = 5;
      zm.mask = 0x7f;
    }
    break;
  case 13:
    if (maxZones > 32) {
      zm.idx = 5;
      zm.mask = 0xbf;
    }
    break;
  case 14:
    if (maxZones > 32) {
      zm.idx = 5;
      zm.mask = 0xdf;
    }
    break;
  case 16:
    if (maxZones > 32) {
      zm.idx = 5;
      zm.mask = 0xef;
    }
    break; //5108 sends to slot 15
  default:
    zm.idx = 0;
    zm.mask = 0;
  }
  return zm;
}

//clears all emulated zones on the panel 
void dscKeybusInterface::clearZoneRanges() {
  byte zoneupdate[maxFields05];
  memset(zoneupdate, 0xFF, maxFields05);
  for (int x = 0; x < moduleIdx; x++) {
    modules[x].faultBuffer[0] = 0x55;
    modules[x].faultBuffer[1] = 0;
    modules[x].faultBuffer[2] = 0x55;
    modules[x].faultBuffer[3] = 0;
    modules[x].faultBuffer[4] = 0xaa; //cksum for 01010101 00000000 
    if (!modules[x].zoneStatusByte) continue;
    zoneupdate[modules[x].zoneStatusByte] &= modules[x].zoneStatusMask; //set update slot
  }
  writeCharsToQueue(zoneupdate, 1, maxFields05);
}

//once we know what panelversion we have, we can then update the modules with the correct info here
void dscKeybusInterface::updateModules() {

  //start expander
  //sanity check - we make sure that our module responses fit the cmd size
  if (keybusVersion1) {
    maxFields05 = 4;
    maxFields11 = 4;
    panelVersion = 2;
  } else {
    maxFields05 = 6;
    maxFields11 = 6;
    panelVersion = 3;
  }

  for (int x = 0; x < moduleIdx; x++) {
    zoneMaskType zm = getUpdateMask(modules[x].address);
    if (!zm.idx) {
      //we don't have an update idx so it means our address is invalid. clear it
      modules[x].address = 0;
    } else {
      modules[x].zoneStatusByte = zm.idx;
      modules[x].zoneStatusMask = zm.mask;
      if (enableModuleSupervision)
        setSupervisorySlot(modules[x].address, true);
    }
  }
}

//add new expander modules and init zone fields
void dscKeybusInterface::addModule(byte address) {

  if (!address) return;
  if (moduleIdx < maxModules) {
    modules[moduleIdx].address = address;
    memset(modules[moduleIdx].fields, 0x55, 4);
    moduleIdx++;
  }
}

void dscKeybusInterface::addRelayModule() {
  if (enableModuleSupervision)
    setSupervisorySlot(18, true);
}

void dscKeybusInterface::removeModule(byte address) {
  int idx;
  for (idx = 0; idx < moduleIdx; idx++) {
    if (modules[idx].address == address) break;
  }
  if (idx == moduleIdx) return;
  modules[idx].address = 0;
  setSupervisorySlot(address, false); //remove it from the supervisory response

}

void dscKeybusInterface::setZoneFault(byte zone, bool fault) {

  byte address = 0;
  byte channel = 0;
  bool change = false;

  //we try and do as much setup here so that the ISR functions do the mimimal work.
  if (zone > maxZones) return;

  //get address and channel from zone range
  if (zone > 8 && zone < 17) {
    address = 9;
    channel = zone - 9;
  } else if (zone > 16 && zone < 25) {
    address = 10;
    channel = zone - 17;
  } else if (zone > 24 && zone < 33) {
    address = 11;
    channel = zone - 25;
  } else if (zone > 32 && zone < 41) {
    address = 12;
    channel = zone - 33;
  } else if (zone > 40 && zone < 49) {
    address = 13;
    channel = zone - 41;
  } else if (zone > 48 && zone < 57) {
    address = 14;
    channel = zone - 49;
  } else if (zone > 56 && zone < 65) {
    address = 16;
    channel = zone - 57;
  }

  if (!address) return; //invalid zone, so return

  //see if we are emulating this zone range 
  int idx;
  for (idx = 0; idx < moduleIdx; idx++) {
    if (modules[idx].address == address) break;
  }

  if (idx == moduleIdx) return; //address not found in our emulation list so return

  uint8_t chk1 = 0xff;
  uint8_t chk2 = 0xff;

  if (channel < 4) { //set / reset bits according to fault value (open=11,closed=01)
    channel = channel * 2;
    modules[idx].fields[0] = fault ? modules[idx].fields[0] | (zoneOpen << channel) : modules[idx].fields[0] & ~(zoneClosed << channel);
    chk1 = ((modules[idx].fields[0] >> 4) + (modules[idx].fields[0] & 0x0f) + (modules[idx].fields[1] >> 4) + (modules[idx].fields[1] & 0x0f)) % 0x10;
  } else {
    channel = (channel - 4) * 2;
    modules[idx].fields[2] = fault ? modules[idx].fields[2] | (zoneOpen << (channel)) : modules[idx].fields[2] & ~(zoneClosed << (channel));
    chk2 = ((modules[idx].fields[2] >> 4) + (modules[idx].fields[2] & 0x0f) + (modules[idx].fields[3] >> 4) + (modules[idx].fields[3] & 0x0f)) % 0x10;
  }

  memset(modules[idx].faultBuffer, 0xFF, 5);

  if (modules[idx].fields[0] != modules[idx].fields[1]) { //see if our current low channels changed from previous. 
    modules[idx].faultBuffer[0] = modules[idx].fields[0]; //populate faultbuffer with response data for low channel
    modules[idx].faultBuffer[1] = modules[idx].fields[1];
    modules[idx].faultBuffer[4] = (chk1 << 4) | 0x0F;
    modules[idx].fields[1] = modules[idx].fields[0]; //copy current channel values to previous
    change = true;
  }
  if (modules[idx].fields[2] != modules[idx].fields[3]) { //check high channels
    modules[idx].faultBuffer[2] = modules[idx].fields[2];
    modules[idx].faultBuffer[3] = modules[idx].fields[3];
    modules[idx].faultBuffer[4] = (modules[idx].faultBuffer[4] & 0xf0) | chk2;
    modules[idx].fields[3] = modules[idx].fields[2]; //copy current channel values to previous
    change = true;
  }

  if (!change) return; //nothing changed in our zones so return
  byte zoneupdate[maxFields05];
  memset(zoneupdate, 0xFF, maxFields05); //set update slots to 1's. Only zero bits indicate a request
  if (modules[idx].zoneStatusByte) {
    zoneupdate[modules[idx].zoneStatusByte] &= modules[idx].zoneStatusMask; //set update slot
    writeCharsToQueue(zoneupdate, 1, maxFields05);
  }

}


void
#if defined(ESP8266)
ICACHE_RAM_ATTR 
#elif defined(ESP32)
IRAM_ATTR 
#endif
dscKeybusInterface::prepareModuleResponse(byte address, int bit) {

  for (int idx = 0; idx < moduleIdx; idx++) { //get the buffer data from the module record that matches the address we need
    if (modules[idx].address == address) {
      updateWriteBuffer((byte * ) modules[idx].faultBuffer, bit, 1,  5,false);
      return;
    }
  }
  
}

unsigned int dscKeybusInterface::dec2bcd(unsigned int num)
{
    unsigned int ones = 0;
    unsigned int tens = 0;
    unsigned int temp = 0;
    ones = num%10;
    temp = num/10;
    tens = temp<<4;
    return (tens + ones);
}

void dscKeybusInterface::setDateTime(unsigned int year,byte month,byte day,byte hour,byte minute) {

  int dataSum = 0;
  cmdD0buffer[0] = dec2bcd(year%100);
  cmdD0buffer[1] = dec2bcd(month);  
  cmdD0buffer[2] = day;
  cmdD0buffer[3] = dec2bcd(hour);
  cmdD0buffer[4] = dec2bcd(minute);  
  for (byte x = 0; x < 5; x++) {
    dataSum += cmdD0buffer[x];
  }
  cmdD0buffer[5] = dataSum % 256;
  pendingD0=true;
  byte zoneupdate[6];
  memset(zoneupdate, 0xFF, 6); //set update slots to 1's. Only zero bits indicate a request
  zoneupdate[3] &= 0xfd; //set update slot for cmd d0
  writeCharsToQueue(zoneupdate, 1, 6);
}

void
#if defined(ESP8266)
ICACHE_RAM_ATTR 
#elif defined(ESP32)
IRAM_ATTR 
#endif 
dscKeybusInterface::processCmd70() {
  if (pgmBuffer.idx + 5 > pgmBuffer.len) return;
  updateWriteBuffer((byte*) &pgmBuffer.data[pgmBuffer.idx], 9,1,5);
  pgmBuffer.idx += 5;    
  byte key = 0;
  if (pgmBuffer.sendhash) key=0x2D; //'#' // setup to send final # cmd to complete write update to panel
  if (pgmBuffer.idx < pgmBuffer.len) {
    pending70 = true;
    key = 0xAA; //more data available so set up also for next group send request
  }
  if (key) writeCharsToQueue( & key, pgmBuffer.partition);

}

void dscKeybusInterface::setLCDReceive(byte digits,byte partition) {
  if (!partition) partition=currentDefaultPartition;
  pgmBuffer.idx = 0;
  byte b = (digits / 2) + (digits % 2); //full bytes
  b+=b/4;//checksum bytes
  b += b % 5 ? 5 - b % 5 : 0; //round up to next 5 bytes as thats the size of cmds70/6e including chksum
  pgmBuffer.len = b;
  pgmBuffer.partition=partition;
  pending6E=true;
  pgmBuffer.dataPending=false;
  byte key = 0xa5;
  writeCharsToQueue( & key,partition);
}

void dscKeybusInterface::setLCDSend(byte partition,bool sendhash) {
  if (!partition) partition=currentDefaultPartition;
  int dataSum;
  pgmBuffer.idx = 0;
  pending70 = true;
  byte key = 0xAA;
  pgmBuffer.sendhash=sendhash;
  pgmBuffer.dataPending=false;
  
  for (int y=0;y<pgmBuffer.len;y=y+5) { //calculate checksums for all groups of 4 bytes
    dataSum = 0;        
    for (int x = 0; x < 4; x++) {
        dataSum += pgmBuffer.data[x+y];
    }
    pgmBuffer.data[y+4]=dataSum%256;
  }

  writeCharsToQueue( & key, partition);

}



