#include <TimeLib.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <OneWire.h> 
#include <DallasTemperature.h>


/**
 * Ferment Mode monitors temp of beer in fermentation stage and tracks the amount of days in fermentation.
 * Pour Mode monitors pouring temperature and the amount of liquid poured since last reset
 */



// NAME THE BEER
char myBEER[] = "American Pale Ale     ";


// A bit to create timers
unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long tempdelay = 4000;  //the value is a number of milliseconds


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




int relayPin = 8;  // Pin of Relay Module


// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}






void setup()

{
  
	lcd.begin();
	lcd.backlight();

 

// Set Pin connected to Relay as an OUTPUT
pinMode(relayPin, OUTPUT); 
// Set Pin to LOW to turn Relay OFF 
digitalWrite(relayPin, LOW);  


   

 // start serial port 
 Serial.begin(9600); 

 //initial start time
 startMillis = millis();  
  
Serial.println("Fermento setup");  

lcd.setCursor(0,0);
lcd.print("Craigs beer machine");

lcd.setCursor(0,2);
lcd.print("Testing sensors...");


// start neopixels animation on startup
  strip = Adafruit_NeoPixel(60, PIN, NEO_RGB + NEO_KHZ800);

  strip.begin();
 
 colorWipe(strip.Color(0, 255, 0), 50); // Red
 colorWipe(strip.Color(0, 0, 255), 50); // Blue
 colorWipe(strip.Color(255, 0, 0), 50); // Green

// stop for 1 sec
delay(1000);

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

  lcd.setCursor(0,0);
// print the beer name set from variable
lcd.print(myBEER);

lcd.setCursor(0,1);
lcd.print("Tank temp:   ");
// timer - print the temp every 4 seconds
  currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)
  if (currentMillis - startMillis >= tempdelay)  //test whether the period has elapsed
  {
startMillis += tempdelay; 
lcd.print(sensors.getTempCByIndex(0));

lcd.print((char)223);
lcd.print("c      ");
  }

 
 




   
lcd.setCursor(0,2);
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







  
  
lcd.setCursor(0,3);
lcd.print("Fermenting:  ");
           
// print the day in cycle
lcd.print(day());

if (day() < 2)

{lcd.print(" day");
}

else

{lcd.print(" days");
}









if ((sensors.getTempCByIndex(0))>potPercentage)
{
  digitalWrite(relayPin, HIGH);  // Turn Relay ON
       Serial.println(" Chiller is ON");
       
}

else

{
digitalWrite(relayPin, LOW);  // Turn Relay OFF
       Serial.println(" Chiller is OFF");
       
}




// start adding an led light for each day in cycle
if (day() == 1)
{
strip.setPixelColor(7, 255, 0, 0);

 strip.show();
}


if (day() == 2)
{
strip.setPixelColor(7, 255, 0, 0);
strip.setPixelColor(6, 255, 0, 0);

 strip.show();
}

if (day() == 3)
{
strip.setPixelColor(7, 255, 0, 0);
strip.setPixelColor(6, 255, 0, 0);
strip.setPixelColor(5, 255, 0, 0);

 strip.show();
}

if (day() == 4)
{
strip.setPixelColor(7, 255, 0, 0);
strip.setPixelColor(6, 255, 0, 0);
strip.setPixelColor(5, 255, 0, 0);
strip.setPixelColor(4, 255, 0, 0);

 strip.show();
}

if (day() == 5)
{
strip.setPixelColor(7, 255, 0, 0);
strip.setPixelColor(6, 255, 0, 0);
strip.setPixelColor(5, 255, 0, 0);
strip.setPixelColor(4, 255, 0, 0);
strip.setPixelColor(3, 255, 0, 0);

 strip.show();
}

if (day() == 6)
{
strip.setPixelColor(7, 255, 0, 0);
strip.setPixelColor(6, 255, 0, 0);
strip.setPixelColor(5, 255, 0, 0);
strip.setPixelColor(4, 255, 0, 0);
strip.setPixelColor(3, 255, 0, 0);
strip.setPixelColor(2, 255, 0, 0);

 strip.show();
}

if (day() == 7)
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

if (day() == 8)
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

if (day() == 9)
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
 Serial.print("Current temp: "); 
 Serial.print(sensors.getTempCByIndex(0)); // Why "byIndex"?  
   // You can have more than one DS18B20 on the same bus.  
   // 0 refers to the first IC on the wire 
    



}
