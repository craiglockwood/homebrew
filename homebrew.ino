#include <TimeLib.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>


/**
   Monitors temp of beer in fermentation stage and whilst kegged

*/



// NAME THE BEER
char myBEER[] = "American Pale Ale";


int target;
int targetPot = A0;



#define PIN 5  //pin for neopixels
int running = 1;

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);


const unsigned long threeSecs = 3000; // 3 second delay between temp readings
static unsigned long lastSampleTime = 0 - threeSecs;  // initialize such that a reading is due the first time through loop()


// Pins for LEDs
int chillLed = 11;
int heatLed = 12;





// Data wire is plugged into pin 2 on the Arduino for thermometre
#define ONE_WIRE_BUS 2
/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);




int chillPin = 8;  // Pin of Relay Module (chill)
int heatPin = 9; // Pin of Relay Module (heat)


// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}







// a bit for mode button, acting as a switch
int inPin = 3;         // the number of the input pin
int outPin = 13;       // the number of the output pin

int state = HIGH;      // the current state of the output pin
int reading;           // the current reading from the input pin
int previous = LOW;    // the previous reading from the input pin

// the follow variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long time = 0;         // the last time the output pin was toggled
long debounce = 400;   // the debounce time, increase if the output flickers




void setup()

{

  pinMode(targetPot, INPUT);

  pinMode(inPin, INPUT_PULLUP);
  pinMode(outPin, OUTPUT);






  pinMode(4, INPUT_PULLUP); //reset buttton


  lcd.begin();
  lcd.backlight();



  // Set Pin connected to chillerand heater as an OUTPUT
  pinMode(chillPin, OUTPUT);
  pinMode(heatPin, OUTPUT);

  // Set Pin to LOW to turn chiler & heater OFF
  digitalWrite(chillPin, LOW);
  digitalWrite(heatPin, LOW);


  pinMode(chillLed, OUTPUT);
  pinMode(heatLed, OUTPUT);




  // start serial port
  Serial.begin(9600);


  Serial.println("Craigs beer machine setup");

  lcd.setCursor(0, 0);
  lcd.print("Craigs beer machine");

  lcd.setCursor(0, 2);
  lcd.print("Testing sensors...");


  // start neopixels animation on startup
  strip = Adafruit_NeoPixel(60, PIN, NEO_RGB + NEO_KHZ800);

  strip.begin();

  colorWipe(strip.Color(0, 255, 0), 50); // Red
  colorWipe(strip.Color(0, 0, 255), 50); // Blue
  colorWipe(strip.Color(255, 0, 0), 50); // Green



  strip.setPixelColor(0, 0, 0, 0);
  strip.setPixelColor(1, 0, 0, 0);
  strip.setPixelColor(2, 0, 0, 0);
  strip.setPixelColor(3, 0, 0, 0);
  strip.setPixelColor(4, 0, 0, 0);
  strip.setPixelColor(5, 0, 0, 0);
  strip.setPixelColor(6, 0, 0, 0);
  strip.setPixelColor(7, 0, 0, 0);
  strip.show();





  // clear the LCD
  lcd.clear();

  // Start up the library for the thermometre
  sensors.begin();




}





void loop()
{
  strip.show();





  unsigned long now = millis();

  if (now - lastSampleTime >= threeSecs)
  {
    lastSampleTime += threeSecs;
    sensors.requestTemperatures(); // Send the command to get temperature readings
  }



  if (digitalRead(4) == LOW) {
    //reset button is pressed!
    Serial.println ("Reset button is pressed");
    setTime(00, 00, 00, 01, 01, 70); // reset time
    strip.setPixelColor(7, 0, 0, 0); // turn off all neopixels
    strip.setPixelColor(6, 0, 0, 0);
    strip.setPixelColor(5, 0, 0, 0);
    strip.setPixelColor(4, 0, 0, 0);
    strip.setPixelColor(3, 0, 0, 0);
    strip.setPixelColor(2, 0, 0, 0);
    strip.setPixelColor(1, 0, 0, 0);
    strip.setPixelColor(0, 0, 0, 0);
    strip.show();

  }

  reading = digitalRead(inPin);

  // if the input just went from LOW and HIGH and we've waited long enough
  // to ignore any noise on the circuit, toggle the output pin and remember
  // the time
  if (reading == HIGH && previous == LOW && millis() - time > debounce) {
    if (state == HIGH)


      state = LOW;  // selects ferment mode


    else  // selects pour mode

      state = HIGH;

    time = millis();
  }



  digitalWrite(outPin, state);

  previous = reading;


  if (state == LOW)  //i.e ferment mode

  {







    lcd.setCursor(0, 0);
    // print the beer name set from variable
    lcd.print(myBEER);
    lcd.setCursor(0, 1);
    lcd.print("Tank temp:   ");

    lcd.print(sensors.getTempCByIndex(0));

    lcd.print((char)223);
    lcd.print("c  ");



    lcd.setCursor(0, 2);
    lcd.print("Target temp: ");

    target = analogRead(targetPot) / 30;


    lcd.print(target);
    lcd.print((char)223);
    lcd.print("c   ");





    // print the day in cycle - upto day 7

    if (day() > 6)
    {
      lcd.setCursor(0, 3);
      lcd.print("Fermenting:  ");
      lcd.print("DONE");
    }

    else
    {
      lcd.setCursor(0, 3);
      lcd.print("Fermenting:  ");
      lcd.print("day ");
      lcd.print(day());


      if (day() < 10)
      {
        lcd.print("  ");
      }

      else

      {
        lcd.print(" ");
      }
    }







    // the logic

    if ((sensors.getTempCByIndex(0)) > target)
    {
      digitalWrite(chillPin, HIGH);  // Chiller is  ON
      digitalWrite(chillLed, HIGH);  // Chill LED is ON
      digitalWrite(heatLed, LOW);    // Heat LED is OFF
      digitalWrite(heatPin, LOW);    // Heater is OFF

    }



    else

    {
      digitalWrite(chillPin, LOW);  // Chiller is  OFF
      digitalWrite(chillLed, LOW);  // Chill LED is OFF
      digitalWrite(heatLed, HIGH);  // Heat LED is ON
      digitalWrite(heatPin, HIGH);    // Heater is ON
    }



    // start adding an led light for each day in cycle
    if (day() >= 1)
    {
      strip.setPixelColor(7, 255, 0, 0);

      strip.show();
    }
    if (day() >= 2)
    {
      strip.setPixelColor(7, 255, 0, 0);
      strip.setPixelColor(6, 255, 0, 0);

      strip.show();
    }
    if (day() >= 3)
    {
      strip.setPixelColor(7, 255, 0, 0);
      strip.setPixelColor(6, 255, 0, 0);
      strip.setPixelColor(5, 255, 0, 0);

      strip.show();
    }
    if (day() >= 4)
    {
      strip.setPixelColor(7, 255, 0, 0);
      strip.setPixelColor(6, 255, 0, 0);
      strip.setPixelColor(5, 255, 0, 0);
      strip.setPixelColor(4, 255, 0, 0);

      strip.show();
    }
    if (day() >= 5)
    {
      strip.setPixelColor(7, 255, 0, 0);
      strip.setPixelColor(6, 255, 0, 0);
      strip.setPixelColor(5, 255, 0, 0);
      strip.setPixelColor(4, 255, 0, 0);
      strip.setPixelColor(3, 255, 0, 0);

      strip.show();
    }
    if (day() >= 6)
    {
      strip.setPixelColor(7, 255, 0, 0);
      strip.setPixelColor(6, 255, 0, 0);
      strip.setPixelColor(5, 255, 0, 0);
      strip.setPixelColor(4, 255, 0, 0);
      strip.setPixelColor(3, 255, 0, 0);
      strip.setPixelColor(2, 255, 0, 0);

      strip.show();
    }
    if (day() >= 7)
    {
      strip.setPixelColor(7, 255, 0, 0);
      strip.setPixelColor(6, 255, 0, 0);
      strip.setPixelColor(5, 255, 0, 0);
      strip.setPixelColor(4, 255, 0, 0);
      strip.setPixelColor(3, 255, 0, 0);
      strip.setPixelColor(2, 255, 0, 0);
      strip.setPixelColor(1, 255, 0, 0);

      strip.show();
    }
    if (day() >= 8)
    {
      strip.setPixelColor(7, 255, 0, 0);
      strip.setPixelColor(6, 255, 0, 0);
      strip.setPixelColor(5, 255, 0, 0);
      strip.setPixelColor(4, 255, 0, 0);
      strip.setPixelColor(3, 255, 0, 0);
      strip.setPixelColor(2, 255, 0, 0);
      strip.setPixelColor(1, 255, 0, 0);
      strip.setPixelColor(0, 255, 0, 0);

      strip.show();
    }






  }



  if (state == HIGH)  //i.e pour mode

  {


    lcd.setCursor(0, 0);
    // print the beer name set from variable
    lcd.print(myBEER);




    lcd.setCursor(0, 1);
    lcd.print("Beer left: ");
    lcd.print("14.4 ltrs");



    lcd.setCursor(0, 2);
    lcd.print("Pressure:  ");
    lcd.print("12 psi ");

    lcd.setCursor(0, 3);
    lcd.print("Tap: ");

    lcd.print(sensors.getTempCByIndex(0), 1);

    lcd.print((char)223);
    lcd.print("c ");

    lcd.print("Goal: ");



    target = analogRead(targetPot) / 30;


    lcd.print(target);
    lcd.print((char)223);




    if (target < 10)
    {
      lcd.print("c");
    }

    else

    {

    }




    // the logic

    if ((sensors.getTempCByIndex(0)) > target)
    {
      digitalWrite(chillPin, HIGH);  // Chiller is  ON
      digitalWrite(chillLed, HIGH);  // Chill LED is ON
      digitalWrite(heatLed, LOW);    // Heat LED is OFF
      digitalWrite(heatPin, LOW);    // Heater is OFF

    }



    else

    {
      digitalWrite(chillPin, LOW);  // Chiller is  OFF
      digitalWrite(chillLed, LOW);  // Chill LED is OFF
      digitalWrite(heatLed, HIGH);  // Heat LED is ON
      digitalWrite(heatPin, HIGH);    // Heater is ON
    }




  }



}



