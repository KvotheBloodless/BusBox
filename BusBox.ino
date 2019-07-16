//=====[ INCLUDES ]=====
#include <XBee.h>

//=====[ XBee ]=====
XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle 
Rx16Response rx16 = Rx16Response();
Rx64Response rx64 = Rx64Response();

//=====[ CONSTANTS ]=====
const int bSize = 64;
const char comfortLight = 'a'; 
const char engineeringLight = 'b';
const char hydraulic = 'c';
const char outlet = 'd';
const char fan = 'e';
const char ventilation = 'f';
const char freshWaterPump = 'g';
const char wasteWaterPump = 'h';
const char washDownPump = 'i';
const char internet = 'j';
const char entertainment = 'k';
const char refrigeration = 'l';
const char anchorLight = 'm';
const char steamingLight = 'n';
const char redOverGreenLight = 'o';
const char deckLight = 'p';
const char navigationLight = 'q';
const char autopilot = 'r';
const char instrument = 's';
const char radio = 't';
const char unused = 'z';

//=====[ PIN CONSTANTS ]=====
// Double check that circuitPins and circuitTypes are same length

// const char circuitTypes[] = { comfortLight, engineeringLight, outlet, fan, ventilation, entertainment, refrigeration, redOverGreenLight, navigationLight, instrument, deckLight, unused }; // aft
// const char circuitTypes[] = { ... }; // mid ships
const char circuitTypes[] = { comfortLight, engineeringLight, outlet, fan, ventilation, entertainment, hydraulic, internet, anchorLight, steamingLight, deckLight, radio }; // forward

// const int circuitPins[] = { 21, 22, 23, 13, 12, 11, 20, 19, 18, 10, 9, 8 }; // aft
// const int circuitPins[] =   { ... }; // mid ships
const int circuitPins[] = { 8, 9, 10, 11, 12, 13, 18, 19, 20, 21, 22, 23 }; // forward

const int numberOfCircuits = 12;

//=====[ VARIABLES ]=====

char command[25];
bool validCommand = false;
uint8_t option = 0;

//=====[ SETUP ]=====

void setup() {

  // Configure cirucuit pins as output.
  for (int i = 0; i < numberOfCircuits; i++)
    pinMode(circuitPins[i], OUTPUT);

  Serial.print("Booting");

  Serial.begin(9600);

  Serial1.begin(9600);
  xbee.setSerial(Serial1);
}

//=====[ PROGRAM ]=====

void loop() {

  while (fetchCommand() == true)
    if (validCommand == true)
      // parse command and act on it
      processCommand();
}

//=====[ SUBROUTINES ]=====
bool fetchCommand(void) {

  validCommand = false;

  xbee.readPacket();
    
  if (xbee.getResponse().isAvailable()) {
      
    if (xbee.getResponse().getApiId() == RX_16_RESPONSE || xbee.getResponse().getApiId() == RX_64_RESPONSE) {
        
      if (xbee.getResponse().getApiId() == RX_16_RESPONSE) {
                
        xbee.getResponse().getRx16Response(rx16);
        option = rx16.getOption();
        strcpy(command, rx16.getData());
      } else {
                
        xbee.getResponse().getRx64Response(rx64);
        option = rx64.getOption();
        strcpy(command, rx64.getData());
      }

      if (false) { // Do some validation here

        Serial.print("Invalid command: ");
        Serial.println(command);
      } else
        validCommand = true;

      return true;
    } else {

      Serial.print("Not what we were expecting. ApiId: ");  
      Serial.println(xbee.getResponse().getApiId());
    }
  } else if (xbee.getResponse().isError()) {

    Serial.print("Error reading packet.  Error code: ");  
    Serial.println(xbee.getResponse().getErrorCode());
  } 

  return false;
}

void processCommand(void) {

  // Format is <circuit_name> <requested status>
  // For example: c 0 - turn circuit c off

  Serial.print("Received command ");
  Serial.println(command);

  // Process it
  // Split the command in two values
  char circuit;
  int requestedState;
  sscanf(command, "%c %d", &circuit, &requestedState);


  for(int i = 0; i < numberOfCircuits; i++) {

    if(circuit == circuitTypes[i]) {

      // Toggle pin status
      Serial.print("Setting pin ");
      Serial.print(circuitPins[i]);
      Serial.print(" to state ");
      Serial.println(requestedState == 0 ? "off" : "on");
      
      digitalWrite(circuitPins[i], requestedState == 0 ? LOW : HIGH);
    }
  }
}
