// A basic everyday NeoPixel strip test program.

// NEOPIXEL BEST PRACTICES for most reliable operation:
// - Add 1000 uF CAPACITOR between NeoPixel strip's + and - connections.
// - MINIMIZE WIRING LENGTH between microcontroller board and first pixel.
// - NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR.
// - AVOID connecting NeoPixels on a LIVE CIRCUIT. If you must, ALWAYS
//   connect GROUND (-) first, then +, then data.
// - When using a 3.3V microcontroller with a 5V-powered NeoPixel strip,
//   a LOGIC-LEVEL CONVERTER on the data line is STRONGLY RECOMMENDED.
// (Skipping these may work OK on your workbench but can fail in the field)

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    3

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 16

#define ACCEL_INPUT_VOL_PIN 0
#define BREAK_INPUT_PIN 7
#define CLUTCH_INPUT_PIN 8

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_BRG + NEO_KHZ800);

unsigned long lastShowTime;

void setup() {
  lastShowTime = millis();

  pinMode(ACCEL_INPUT_VOL_PIN, INPUT);
  pinMode(CLUTCH_INPUT_PIN, INPUT);
  pinMode(BREAK_INPUT_PIN, INPUT);

  Serial.begin(9600);

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
}

float accelVoltage = 0;
bool clutchPressed = false;
bool brakePressed = false;

void updateCarState()
{
  accelVoltage = analogRead(ACCEL_INPUT_VOL_PIN) * (5.0f / 1023);
  clutchPressed = digitalRead(CLUTCH_INPUT_PIN) == HIGH;
  brakePressed = digitalRead(BREAK_INPUT_PIN) == HIGH;
}

void loop() {

  unsigned long currentTime = millis();
  if((currentTime - lastShowTime) >= 10) // every 10 milliseconds
  {
    updateCarState();
    
    // set all pixels to black
    for(int x = 0; x < LED_COUNT; x++)
    {
      strip.setPixelColor(x, strip.Color(0, 0, 0));
    }

    // set the brake color, if it's pressed
    if(brakePressed)
    {
      for(int x = 0; x < LED_COUNT; x++)
      {
        strip.setPixelColor(x, strip.Color(255, 0, 0));
      }
    }

    int howManyLedsToLightUp = (accelVoltage * LED_COUNT) / 5;

    for(int x = 0; x < howManyLedsToLightUp; x++)
    {
      if(clutchPressed)
      {
        // yellow
        strip.setPixelColor(x, strip.Color(255, 255, 0));
      }
      else
      {
        // green
        strip.setPixelColor(x, strip.Color(0, 255, 0));
      }
      
    }

    if(accelVoltage < 0.3f /*volts*/ && !brakePressed && !clutchPressed)
    {
      int ledsToLightup = 5;
      for(int x = LED_COUNT - ledsToLightup; x < LED_COUNT; x++)
      {
        strip.setPixelColor(x, strip.Color(0, 0, 255));
      }
    }

    strip.show();
    
  }
}
