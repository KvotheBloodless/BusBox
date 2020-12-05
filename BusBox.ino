//=====[ INCLUDES ]=====
#include <XBee.h>

//=====[ XBee ]=====
XBee xbee = XBee();
XBeeResponse response = XBeeResponse();

Rx16Response rx16 = Rx16Response();
Rx64Response rx64 = Rx64Response();
ZBRxResponse rxZB = ZBRxResponse();


//=====[ CONSTANTS ]=====
static const int bSize = 64;
static const char comfortLight = 'a'; 
static const char engineeringLight = 'b';
static const char hydraulic = 'c';
static const char outlet = 'd';
static const char fan = 'e';
static const char ventilation = 'f';
static const char freshWaterPump = 'g';
static const char wasteWaterPump = 'h';
static const char washDownPump = 'i';
static const char internet = 'j';
static const char entertainment = 'k';
static const char refrigeration = 'l';
static const char anchorLight = 'm';
static const char steamingLight = 'n';
static const char redOverGreenLight = 'o';
static const char deckLightForward = 'p';
static const char navigationLight = 'q';
static const char autopilot = 'r';
static const char instrument = 's';
static const char radio = 't';
static const char deckLightCockpit = 'u';
static const char deckLightAft = 'v';
static const char unused = 'z';

//=====[ PIN CONSTANTS ]=====
// Double check that circuitPins and circuitTypes are same length

// static const char circuitTypes[] = { comfortLight, engineeringLight, outlet, fan, ventilation, entertainment, refrigeration, redOverGreenLight, navigationLight, internet, deckLightCockpit, deckLightAft }; // aft
// - Proposed - Unsure about engineering light and outle
static const char circuitTypes[] = { comfortLight, ventilation, navigationLight, deckLightAft, engineeringLight, deckLightCockpit, refrigeration, redOverGreenLight, outlet, internet, fan, entertainment }; // aft

// static const char circuitTypes[] = { comfortLight, engineeringLight, hydraulic, outlet, fan, freshWaterPump, wasteWaterPump, washDownPump, refrigeration, autopilot, instrument, radio }; // mid ships
// static const char circuitTypes[] = { comfortLight, engineeringLight, outlet, fan, ventilation, entertainment, hydraulic, internet, anchorLight, steamingLight, deckLightForward, radio }; // forward

static const int circuitPins[] = { 21, 22, 23, 13, 12, 11, 20, 19, 18, 10, 9, 8 }; // aft
// static const int circuitPins[] =   { 8, 9, 10, 11, 12, 13, 18, 19, 20, 21, 22, 23 }; // mid ships
// static const int circuitPins[] = { 8, 9, 10, 11, 12, 13, 18, 19, 20, 21, 22, 23 }; // forward

const int numberOfCircuits = 12;

//=====[ VARIABLES ]=====

char command[255];
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

  if (fetchCommand() == true)
    // parse command and act on it
    processCommand();
}

//=====[ SUBROUTINES ]=====
bool fetchCommand(void) {

  xbee.readPacket();
    
  if (xbee.getResponse().isAvailable()) {
      
    if (xbee.getResponse().getApiId() == RX_16_RESPONSE || xbee.getResponse().getApiId() == RX_64_RESPONSE || xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
        
      if (xbee.getResponse().getApiId() == RX_16_RESPONSE) {
                
        xbee.getResponse().getRx16Response(rx16);
        option = rx16.getOption();
        strcpy(command, rx16.getData());
      } else if (xbee.getResponse().getApiId() == RX_64_RESPONSE) {
                
        xbee.getResponse().getRx64Response(rx64);
        option = rx64.getOption();
        strcpy(command, rx64.getData());
      } else {
                
        xbee.getResponse().getZBRxResponse(rxZB);
        option = rxZB.getOption();
        strcpy(command, rxZB.getData());
      }

      if (false) { // Do some validation here

        Serial.print("Invalid command: ");
        Serial.println(command);
      } else
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
