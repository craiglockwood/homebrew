
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
int heatPin = 9; // Pin of Relay Module (heat  - not used yet but hard wired ready to go)


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



// A bit to create timers
unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long tempdelay = 6000;  //the value is a number of milliseconds


void setup()

{

  //initial start time
  startMillis = millis();


  pinMode(3, INPUT_PULLUP); //mode button
  pinMode(4, INPUT_PULLUP); //reset buttton


  lcd.begin();
  lcd.backlight();



  // Set Pin connected to chiller as an OUTPUT
  pinMode(chillPin, OUTPUT);
  // Set Pin to LOW to turn chiler OFF
  digitalWrite(chillPin, LOW);




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

  // stop for 1 sec
  delay(300);

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



int rawValue;
int oldValue;
int potPercentage;
int oldPercentage;
int xPercentage;


void loop()


{


  if (digitalRead(3) == LOW) {
    //mode button is pressed!
    Serial.println ("Mode button is pressed");

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
  // read input twice
  rawValue = analogRead(A0);
  rawValue = analogRead(A0); // double read
  // ignore bad hop-on region of a pot by removing 8 values at both extremes
  rawValue = constrain(rawValue, 8, 1015);
  // add some deadband
  if (rawValue < (oldValue - 4) || rawValue > (oldValue + 4)) {
    oldValue = rawValue;
    // convert to percentage
    potPercentage = map(oldValue, 8, 1008, 0, 50);
    // Only print if %value changes
    if (oldPercentage != potPercentage) {


      // print target temperature
      lcd.print(  potPercentage);
      lcd.print((char)223);
      lcd.print("c      ");

      oldPercentage = potPercentage;
    }
  }











  // print the day in cycle




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

  }







  // timer - check temp & process every 4 seconds
  currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)
  if (currentMillis - startMillis >= tempdelay)  //test whether the period has elapsed
  {

    startMillis += tempdelay;


    // the logic

    if ((sensors.getTempCByIndex(0)) > potPercentage)
    {
      digitalWrite(chillPin, HIGH);  // Chiller is  ON


    }

    else

    {
      digitalWrite(chillPin, LOW);  // Chiller is  OFF


    }



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



  if (day() >= 9)
  {
    strip.setPixelColor(7, 0, 255, 0);
    strip.setPixelColor(6, 0, 255, 0);
    strip.setPixelColor(5, 0, 255, 0);
    strip.setPixelColor(4, 0, 255, 0);
    strip.setPixelColor(3, 0, 255, 0);
    strip.setPixelColor(2, 0, 255, 0);
    strip.setPixelColor(1, 0, 255, 0);
    strip.setPixelColor(0, 0, 255, 0);

    strip.show();
  }



  /********************************************************************/

  sensors.requestTemperatures(); // Send the command to get temperature readings

  /********************************************************************/





}
