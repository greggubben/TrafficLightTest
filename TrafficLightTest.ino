/*
 * This will simulate an Intersection comprised of 2 sets of Traffic Lights.
 * One Traffic Light controls the North/South traffic and the other
 * Traffic Light controls the East/West traffic.
 * 
 * The Traffic Light will be in control for it's GO and CAUTION states and then
 * transfer control to the other Traffic Light while it is in a STOP state.
 */


enum TrafficLightStates {GO, CAUTION, STOP, READY, TRAFFICLIGHTSTATES_MAX};

typedef struct Light {
  unsigned int  ledPin;       // Pin the Light is attached.
  unsigned long onDuration;   // Time light should be on.
} Light;

typedef struct TrafficLight {
  Light* red;
  Light* yellow;
  Light* green;
  TrafficLightStates currentState;
  char* name;
} TrafficLight;

typedef struct Intersection {
  TrafficLight* activeTrafficLight;
  TrafficLight* passiveTrafficLight;
} Intersection;


// North/South Traffic light
#define NS_RED_PIN    12
#define NS_YELLOW_PIN 11
#define NS_GREEN_PIN  10

Light ns_red    = {NS_RED_PIN,    1000};
Light ns_yellow = {NS_YELLOW_PIN, 5000};
Light ns_green  = {NS_GREEN_PIN, 10000};

TrafficLight ns_trafficLight = {&ns_red, &ns_yellow, &ns_green, STOP, "NS"};

// East/West Traffic light
#define EW_RED_PIN    7
#define EW_YELLOW_PIN 6
#define EW_GREEN_PIN  5

Light ew_red    = {EW_RED_PIN,    1000};
Light ew_yellow = {EW_YELLOW_PIN, 5000};
Light ew_green  = {EW_GREEN_PIN, 10000};

TrafficLight ew_trafficLight = {&ew_red, &ew_yellow, &ew_green, GO, "EW"};

// Intersection
Intersection nsew_intersection = {&ew_trafficLight, &ns_trafficLight};

unsigned long lastStateChange;
unsigned long delayStateChange;
TrafficLightStates nextTrafficLightState;

/************************************
 * 
 ************************************/


void setup() {
  Serial.begin(115200);
  intersectionInit(&nsew_intersection);
  lastStateChange = millis();

  printAllValues();
  
  Serial.println(F("End Setup"));
  
}

void loop() {
  if (trafficLightSwitchMachine(nsew_intersection.activeTrafficLight)) {

    if (nsew_intersection.activeTrafficLight->currentState == STOP) {
      Serial.print(F("Swap active "));
      Serial.print(nsew_intersection.activeTrafficLight->name);
      Serial.print(F(" with passive "));
      Serial.print(nsew_intersection.passiveTrafficLight->name);
      Serial.print(F(" with state "));
      TrafficLight* hold = nsew_intersection.activeTrafficLight;
      nsew_intersection.activeTrafficLight = nsew_intersection.passiveTrafficLight;
      nsew_intersection.passiveTrafficLight = hold;
      trafficAllOff(nsew_intersection.activeTrafficLight);
      trafficSetState(nsew_intersection.activeTrafficLight, nsew_intersection.activeTrafficLight->currentState);
      //delayStateChange = 0;   // Start next cycle
    }
  }
  delay(50);
}

void printAllValues () {
  printIntersectionValues();
  printTrafficLightValues();
  printLightValues();
}

/************************************
 * Intersection Light Functions
 ************************************/

// Initialize the Traffic Lights involved in an Intersection
void intersectionInit(Intersection* intersection) {
  trafficLightInit(intersection->activeTrafficLight);
  trafficLightInit(intersection->passiveTrafficLight);
}


void printIntersectionValues() {
  char ptr_buffer[30];
  Serial.print(F("NSEW Intersection addr:       "));
  sprintf(ptr_buffer,"%p",&nsew_intersection);
  Serial.print(ptr_buffer);
  Serial.print(F(" - "));
  Serial.println(sizeof(nsew_intersection));
  Serial.print(F("  Active  Traffic Light addr: "));
  sprintf(ptr_buffer,"%p",nsew_intersection.activeTrafficLight);
  Serial.println(ptr_buffer);
  Serial.print(F("  Passive Traffic Light addr: "));
  sprintf(ptr_buffer,"%p",nsew_intersection.passiveTrafficLight);
  Serial.println(ptr_buffer);
  Serial.println();
}

/************************************
 * Traffic Light Functions
 ************************************/

// Initialize a Traffic Light
void trafficLightInit(TrafficLight* trafficLight) {
  Serial.print(F("Init "));
  Serial.print(trafficLight->name);
  Serial.print(F(": "));
  lightInit(trafficLight->red);
  lightInit(trafficLight->yellow);
  lightInit(trafficLight->green);
  trafficAllOff(trafficLight);
  trafficSetState(trafficLight, trafficLight->currentState);
}

// Check to see if it is time to switch the state of the Traffic Light
bool trafficLightSwitchMachine(TrafficLight* trafficLight) {
  bool switchedState = false;
  if (millis() - lastStateChange >= delayStateChange) {
    Serial.print(F("State Change "));
    Serial.print(trafficLight->name);
    Serial.print(F(": "));
    trafficAllOff(trafficLight);
    trafficSetState(trafficLight, nextTrafficLightState);
    lastStateChange = millis();
    switchedState = true;
  }
  return switchedState;
}

void trafficAllOff(TrafficLight* trafficLight) {
  lightOff(trafficLight->red);
  lightOff(trafficLight->yellow);
  lightOff(trafficLight->green);
}

void trafficSetState(TrafficLight* trafficLight, TrafficLightStates nextState) {
  char ptr_buffer[30];

  trafficLight->currentState = nextState;

  switch (trafficLight->currentState) {
    case GO: {
      Serial.println(F("GO"));
      lightOn(trafficLight->green);
      delayStateChange = trafficLight->green->onDuration;
      nextTrafficLightState = CAUTION;
    }
    break;
    case CAUTION: {
      Serial.println(F("CAUTION"));
      lightOn(trafficLight->yellow);
      delayStateChange = trafficLight->yellow->onDuration;
      nextTrafficLightState = STOP;
    }
    break;
    case STOP: {
      Serial.println(F("STOP"));
      lightOn(trafficLight->red);
      delayStateChange = trafficLight->red->onDuration;
      nextTrafficLightState = GO;
    }
    break;
  }
}

void printTrafficLightValues () {
  char ptr_buffer[30];
  Serial.print(F("NS Traffic Light addr: "));
  sprintf(ptr_buffer,"%p",&ns_trafficLight);
  Serial.print(ptr_buffer);
  Serial.print(F(" - "));
  Serial.println(sizeof(ns_trafficLight));
  Serial.print(F("  Name:                "));
  Serial.println(ns_trafficLight.name);
  Serial.print(F("  currentState:        "));
  Serial.println(ns_trafficLight.currentState);
  Serial.print(F("  Green  Light addr:   "));
  sprintf(ptr_buffer,"%p",ns_trafficLight.green);
  Serial.println(ptr_buffer);
  Serial.print(F("  Yellow Light addr:   "));
  sprintf(ptr_buffer,"%p",ns_trafficLight.yellow);
  Serial.println(ptr_buffer);
  Serial.print(F("  Red    Light addr:   "));
  sprintf(ptr_buffer,"%p",ns_trafficLight.red);
  Serial.println(ptr_buffer);
  Serial.println();

  Serial.print(F("EW Traffic Light addr: "));
  sprintf(ptr_buffer,"%p",&ew_trafficLight);
  Serial.print(ptr_buffer);
  Serial.print(F(" - "));
  Serial.println(sizeof(ew_trafficLight));
  Serial.print(F("  Name:                "));
  Serial.println(ew_trafficLight.name);
  Serial.print(F("  currentState:        "));
  Serial.println(ew_trafficLight.currentState);
  Serial.print(F("  Green  Light addr:   "));
  sprintf(ptr_buffer,"%p",ew_trafficLight.green);
  Serial.println(ptr_buffer);
  Serial.print(F("  Yellow Light addr:   "));
  sprintf(ptr_buffer,"%p",ew_trafficLight.yellow);
  Serial.println(ptr_buffer);
  Serial.print(F("  Red    Light addr:   "));
  sprintf(ptr_buffer,"%p",ew_trafficLight.red);
  Serial.println(ptr_buffer);
  Serial.println();
  
}

/************************************
 * Light Functions
 ************************************/

void lightInit(Light* light) {
  pinMode(light->ledPin, OUTPUT);
  lightOff(light);
}

void lightOn(Light* light) {
  digitalWrite(light->ledPin, HIGH);
}

void lightOff(Light* light) {
  digitalWrite(light->ledPin, LOW);
}

void printLightValues () {
  char ptr_buffer[30];
  Serial.print(F("NS Green  Light addr: "));
  sprintf(ptr_buffer,"%p",&ns_green);
  Serial.print(ptr_buffer);
  Serial.print(F(" - "));
  Serial.println(sizeof(ns_green));
  Serial.print(F("  Led Pin:            "));
  Serial.println(ns_green.ledPin);
  Serial.print(F("  On Duration:        "));
  Serial.println(ns_green.onDuration);
  Serial.println();

  Serial.print(F("NS Yellow Light addr: "));
  sprintf(ptr_buffer,"%p",&ns_yellow);
  Serial.print(ptr_buffer);
  Serial.print(F(" - "));
  Serial.println(sizeof(ns_yellow));
  Serial.print(F("  Led Pin:            "));
  Serial.println(ns_yellow.ledPin);
  Serial.print(F("  On Duration:        "));
  Serial.println(ns_yellow.onDuration);
  Serial.println();

  Serial.print(F("NS Red    Light addr: "));
  sprintf(ptr_buffer,"%p",&ns_red);
  Serial.print(ptr_buffer);
  Serial.print(F(" - "));
  Serial.println(sizeof(ns_red));
  Serial.print(F("  Led Pin:            "));
  Serial.println(ns_red.ledPin);
  Serial.print(F("  On Duration:        "));
  Serial.println(ns_red.onDuration);
  Serial.println();

  Serial.print(F("EW Green  Light addr: "));
  sprintf(ptr_buffer,"%p",&ew_green);
  Serial.print(ptr_buffer);
  Serial.print(F(" - "));
  Serial.println(sizeof(ew_green));
  Serial.print(F("  Led Pin:            "));
  Serial.println(ew_green.ledPin);
  Serial.print(F("  On Duration:        "));
  Serial.println(ew_green.onDuration);
  Serial.println();

  Serial.print(F("EW Yellow Light addr: "));
  sprintf(ptr_buffer,"%p",&ew_yellow);
  Serial.print(ptr_buffer);
  Serial.print(F(" - "));
  Serial.println(sizeof(ew_yellow));
  Serial.print(F("  Led Pin:            "));
  Serial.println(ew_yellow.ledPin);
  Serial.print(F("  On Duration:        "));
  Serial.println(ew_yellow.onDuration);
  Serial.println();

  Serial.print(F("EW Red    Light addr: "));
  sprintf(ptr_buffer,"%p",&ew_red);
  Serial.print(ptr_buffer);
  Serial.print(F(" - "));
  Serial.println(sizeof(ew_red));
  Serial.print(F("  Led Pin:            "));
  Serial.println(ew_red.ledPin);
  Serial.print(F("  On Duration:        "));
  Serial.println(ew_red.onDuration);
  Serial.println();

}
