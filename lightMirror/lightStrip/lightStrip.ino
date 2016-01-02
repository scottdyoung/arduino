#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
  #include <avr/power.h>
#endif

#define OUTER_LIGHTS_PIN      4
#define INNER_LIGHTS_PIN      6
#define OUTER_LIGHTS          97
#define INNER_LIGHTS          60
#define SECONDS_UNTIL_EVENT   2
#define COLOR_MAX_ONE_THIRD   85
#define COLOR_MAX_TWO_THIRD   170
#define COLOR_MAX             255

Adafruit_NeoPixel innerLights = Adafruit_NeoPixel(INNER_LIGHTS, INNER_LIGHTS_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel outerLights = Adafruit_NeoPixel(OUTER_LIGHTS, OUTER_LIGHTS_PIN, NEO_GRB + NEO_KHZ800);


const int relayPin = 2;
const int buttonPin = 13;
const int sensorPin = A4;
const int innerLightGrowthRate = 2;

int currentOuterColor = 0;
int currentLed = 0;
int currentInnerLightStrength = 0;

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
    initializeLights(innerLights, innerLights.Color(0, 0, 0));
    initializeLights(outerLights, outerLights.Color(0, 0, COLOR_MAX));
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
        } else {
            currentInnerLightStrength = currentInnerLightStrength + innerLightGrowthRate;
            int innerIntensity = (currentInnerLightStrength > COLOR_MAX) ? COLOR_MAX : currentInnerLightStrength;
            if (innerIntensity == COLOR_MAX) {
                changeColor(innerLights, innerLights.Color(COLOR_MAX, 0, 0));  
            } else {
                changeColor(innerLights, innerLights.Color(innerIntensity, innerIntensity, innerIntensity));  
            }
            delay((((float)(COLOR_MAX - innerIntensity) / (float)COLOR_MAX)) * 100);
        }
        innerLights.show();
    } else {
        if (isRelayOpen) {
            closeRelay();  
        }
        pressureStartTime = now;
        outerLights.show();
        currentInnerLightStrength = 0;
        initializeLights(innerLights, innerLights.Color(0, 0, 0));
    }


    
    for (int i=0; i < outerLights.numPixels(); i=i+1) {
        outerLights.setPixelColor(i, getPixelColor(outerLights, (i+currentOuterColor) % COLOR_MAX)); 
    }
    outerLights.show();
    currentOuterColor = (currentOuterColor + 1)  % COLOR_MAX;
    delay(15);
} 

uint32_t getPixelColor(Adafruit_NeoPixel& lights, byte color) {
    color = COLOR_MAX - color;
    if(color < COLOR_MAX_ONE_THIRD) {
        return lights.Color(getMappingColor(51, COLOR_MAX, color), 0, 0);
    }
    if(color < COLOR_MAX_TWO_THIRD) {
        color = color - COLOR_MAX_ONE_THIRD;
        return lights.Color(getMappingColor(102, COLOR_MAX, color), getMappingColor(25, 50, color), 0);
    }
    color = color - COLOR_MAX_TWO_THIRD;
    return lights.Color(getMappingColor(0, 25, color), getMappingColor(0, 25, color), getMappingColor(0, 25, color));
}

int getMappingColor(int low, int high, int current) {
    int range = high - low;
    double percent = (float) current/ (float) COLOR_MAX_ONE_THIRD;
    return percent * range + low;
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

