//=====[ INCLUDES ]=====
#include <XBee.h>

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
static const char deckLight = 'p';
static const char navigationLight = 'q';
static const char autopilot = 'r';
static const char instrument = 's';
static const char radio = 't';
static const char unused = 'z';

//=====[ PIN CONSTANTS ]=====
// Double check that circuitPins and circuitTypes are same length

// const char circuitTypes[] = { comfortLight, engineeringLight, outlet, fan, ventilation, entertainment, refrigeration, redOverGreenLight, navigationLight, instrument, deckLight, unused }; // aft
// const char circuitTypes[] = { ... }; // mid ships
static const char circuitTypes[] = { comfortLight, engineeringLight, outlet, fan, ventilation, entertainment, hydraulic, internet, anchorLight, steamingLight, deckLight, radio }; // forward

// const int circuitPins[] = { 21, 22, 23, 13, 12, 11, 20, 19, 18, 10, 9, 8 }; // aft
// const int circuitPins[] =   { ... }; // mid ships
static const int circuitPins[] = { 8, 9, 10, 11, 12, 13, 18, 19, 20, 21, 22, 23 }; // forward

const int numberOfCircuits = 12;

//=====[ VARIABLES ]=====
XBee xbee = XBee();
char command[256];
ZBRxResponse rx = ZBRxResponse();
char *ptr = NULL;
char circuit;
int requestedState;

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

    if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {

      xbee.getResponse().getZBRxResponse(rx);

      strncpy(command, rx.getData(), rx.getDataLength());

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

  // Format is <circuit_name> <requested status>;[<circuit_name> <requested status>;]...
  // For example: c 0;a 1; - turn circuit c off, turn circuit a on

  Serial.print("Received command ");
  Serial.println(command);

  // Process it
  ptr = strtok(command, ";");
  while (ptr != NULL) {

    // Split the command in two values
    sscanf(ptr, "%c %d", &circuit, &requestedState);

    for (int i = 0; i < numberOfCircuits; i++) {

      if (circuit == circuitTypes[i]) {

        // Toggle pin status
        Serial.print("Setting pin ");
        Serial.print(circuitPins[i]);
        Serial.print(" to state ");
        Serial.println(requestedState == 0 ? "off" : "on");

        digitalWrite(circuitPins[i], requestedState == 0 ? LOW : HIGH);
      }
    }

    ptr = strtok(NULL, ";");
  }
}
