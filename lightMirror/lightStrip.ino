#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
  #include <avr/power.h>
#endif

#define OUTER_LIGHTS_PIN      6
#define INNER_LIGHTS_PIN      4
#define OUTER_LIGHTS          60
#define INNER_LIGHTS          150
#define SECONDS_UNTIL_EVENT   2

Adafruit_NeoPixel innerLights = Adafruit_NeoPixel(INNER_LIGHTS, INNER_LIGHTS_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel outerLights = Adafruit_NeoPixel(OUTER_LIGHTS, OUTER_LIGHTS_PIN, NEO_GRB + NEO_KHZ800);


const int relayPin = 2;
const int buttonPin = 13;
const int sensorPin = A4;

long pressureStartTime = 0;

boolean isRelayOpen = false;
boolean isActive = false;
void setup() {
    #if defined (__AVR_ATtiny85__)
      if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
    #endif

    Serial.begin(9600);

    pinMode(sensorPin, INPUT);
    pinMode(relayPin, OUTPUT); 
    pinMode(buttonPin, INPUT);
    initializeLights(innerLights, innerLights.Color(0, 0, 150));
    initializeLights(outerLights, outerLights.Color(0, 150, 0));
}

void initializeLights(Adafruit_NeoPixel& lights, uint32_t color) {
    lights.begin();
    changeColor(lights, color);
}

void loop(){
    int pressureValue = analogRead(sensorPin);
    long now = millis() / 1000;
    
    if (isPressureSensorActive(pressureValue)) {
        if (!isRelayOpen) {
            pressureStartTime = now;
            openRelay();
            delay(50);
        } else if (isPressedLongEnough(now)) {
            changeColor(innerLights, innerLights.Color(150, 0 , 0));  
        }
        innerLights.show();
    } else {
        if (isRelayOpen) {
            closeRelay();  
        }
        pressureStartTime = now;
        outerLights.show();
    }
} 

boolean isPressedLongEnough(long now) {
  return (now - pressureStartTime) >= SECONDS_UNTIL_EVENT;
}

boolean isPressureSensorActive(int pressureValue) {
    return pressureValue < 512;
}

void openRelay() {
    isRelayOpen = true;
    digitalWrite(relayPin, HIGH);
}

void closeRelay() {
    isRelayOpen = false;
    digitalWrite(relayPin, LOW);
}

void changeColor(Adafruit_NeoPixel& lights, uint32_t color) {
    for(int i=0; i<lights.numPixels(); i++){
        lights.setPixelColor(i, color);
    }
    lights.show();
    delay(10);
}

