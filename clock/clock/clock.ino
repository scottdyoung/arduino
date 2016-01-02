#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <RTClib.h>
#ifdef __AVR__
    #include <avr/power.h>
#endif

#define PIN            6
#define NUMPIXELS      60
#define MINUTE_BUTTON  3

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define UNUSED_COLOR   pixels.Color(0,60,0)
#define HOUR_COLOR     pixels.Color(60,0,0)
#define MINUTE_COLOR   pixels.Color(0,0,60)
#define SECOND_COLOR   pixels.Color(50,50,50)

RTC_Millis rtc;

void setup() {
    #if defined (__AVR_ATtiny85__)
        if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
    #endif
    pixels.begin();
    resetPixels();
    rtc.begin(DateTime(1970, 1, 1, 0, 0, 0));
    Serial.begin(9600);
}
  
void loop() {
    int hour = rtc.now().hour();
    int minute = rtc.now().minute();
    int second = rtc.now().second();

    int isPressed = digitalRead(MINUTE_BUTTON);
    if (isPressed == LOW) {
        rtc.adjust(rtc.now().operator+(TimeSpan(60)));
        delay(150);
    }
    
    setTime(hour, minute, second);
    pixels.show(); 
}

void resetPixels() {
    for(int i=0; i<NUMPIXELS; i++) {
        pixels.setPixelColor(i, UNUSED_COLOR);
    }
}

void setTime(int hour, int minute, int second) {
    resetPixels();
    int hourPixel = findHourPixel(hour);
    pixels.setPixelColor(hourPixel, HOUR_COLOR);
    pixels.setPixelColor(minute, MINUTE_COLOR);
    pixels.setPixelColor(second, SECOND_COLOR);
}

int findHourPixel(int hour) {
    int normalizedHour = hour % 12;
    return normalizedHour * 5;     
}



