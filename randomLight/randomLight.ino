
class ButtonControl {    
    public:
        ButtonControl(){};
        ButtonControl(int led, int button);
        int getLedPin();
        int getButtonPin();
    private:
        int ledPin, buttonPin;
};  

//Constructor member function
ButtonControl::ButtonControl(int led, int button) {
    ledPin = led;
    buttonPin = button;
    pinMode(led, OUTPUT);
    pinMode(button, INPUT);
}

int ButtonControl::getLedPin() {
    return ledPin;
}

int ButtonControl::getButtonPin() {
    return buttonPin;
}

const int NUM_LEDS = 2;
const int MIN_DELAY = 1000;
const int MAX_DELAY = 2000;
ButtonControl buttonControls[NUM_LEDS] = { ButtonControl(12, 3), ButtonControl(13, 2) };
ButtonControl _currentControl;
ButtonControl *currentControl = &_currentControl;

void setup() {
    Serial.begin(9600);
    lightRandom();
}

void loop() {
    int ledPin = currentControl->getLedPin();
    if (ledPin != NULL && digitalRead(currentControl->getButtonPin()) == LOW) {
        digitalWrite(ledPin, HIGH); 
    } else {
        digitalWrite(ledPin, LOW);
        lightRandom();        
    }
}

void lightRandom() {
    delay(random(MIN_DELAY, MAX_DELAY));
    currentControl = &buttonControls[random(0, NUM_LEDS)];
}
