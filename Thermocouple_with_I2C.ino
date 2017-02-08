/***************************************************
  This is an example for the Adafruit Thermocouple Sensor w/MAX31855K


  Designed specifically to work with the Adafruit Thermocouple Sensor
  ----> https://www.adafruit.com/products/269


  These displays use SPI to communicate, 3 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!


  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/


#include <SPI.h>
#include <Wire.h>
#include <SharpIR.h>
#include <Adafruit_MAX31855.h>


// Default connection is using software SPI, but comment and uncomment one of
// the two examples below to switch between software SPI and hardware SPI:
// Example creating a thermocouple instance with software SPI on any three
// digital IO pins.
#define MAXDO   8 // MAXDO on thermocouples all connect to pin 6
#define MAXCLK  7 // CLK on all thermocouples all connect to pin 5
#define NUM_THERMO  8 // Number of thermocouples
//
int MAXCS[] = {2,3,4,5,6,11,12,13}; // CS on thermocouples connect to these pins, respectively
byte temps[NUM_THERMO]; // Initializing array to hold temperature values in byte format

// Declaring the thermocouples
Adafruit_MAX31855 * thermocouples[NUM_THERMO];

// Example creating a thermocouple instance with hardware SPI (Uno/Mega only)
// on a given CS pin.
//#define MAXCS   10
//Adafruit_MAX31855 thermocouple(MAXCS);

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
#define Serial SerialUSB
#endif

// Declaring distance sensors
#define NUM_DIST  0
SharpIR sharp1(A0, 25, 93, 1080);
SharpIR sharp2(A1, 25, 93, 1080);
SharpIR dist_sensors[] = {sharp1};
int dist_pins[] = {A0};
int distance[NUM_DIST];

void setup() {
#ifndef ESP8266
  while (!Serial);     // will pause Zero, Leonardo, etc until serial console opens
#endif
  Wire.begin(0x15);
  Serial.begin(9600);
  Serial.println("MAX31855 test");
  // wait for MAX chip to stabilize
  delay(500);
  for (int i = 0; i < NUM_THERMO; i++) {
    thermocouples[i] = new Adafruit_MAX31855(MAXCLK, MAXCS[i], MAXDO); // Make all the new thermocouple objects
  }
  Wire.onRequest(beagleLoop); // Arduino writes data when interrupted by Beaglebone(runs beagleLoop code)
}


void beagleLoop() {
  Wire.write(temps, NUM_THERMO); // Writing the data to the arduino
  for (int i = 0; i < NUM_DIST; i++) {
    Serial.println(distance[i]);
    if (distance[i] > 255) {        // If value will be greater than 1 byte, break it up into two bytes as it will overflow otherwise
      Wire.write(highByte(distance[i]));
      Wire.write(lowByte(distance[i]));
    }
    else {
      Wire.write(distance[i]);
    }
  }
  
//  for (int i = 0; i < NUM_THERMO; i = i + 1) {
//    Serial.println(temps[i]);
//  }
//  Serial.println(byte(distance));
//  Serial.println(distance);
}

void loop() {
  for (int i = 0; i < NUM_THERMO; i++) {
    if (isnan(thermocouples[i]->readFarenheit())) {
      Serial.print("Something is wrong with thermocouple ");
      Serial.println(i);
    }
    else {
      temps[i] = byte(thermocouples[i]->readFarenheit()); // cast double output of method readFarenheit to bytes
      Serial.print(i);
      Serial.print(" F = ");
      Serial.println(thermocouples[i]->readFarenheit());
    }
    if (i < NUM_DIST) {
      //double dis = analogRead(dist_pins[i]);
      //Serial.println(dis);
      //distance[i] = 1.4898933*pow(10, -5)*pow(dis, 4) - 0.0013447369*pow(dis, 3) + 0.0460845433*pow(dis, 2) - 0.7347082944*dis + 5.15304 ;   // DISTANCE FROM DISTANCE SENSOR in cm??? Unsure if library works correctly, could also just do Analog read
      //distance[i] = 11.365*pow(dis, -0.885);
      //Serial.println(distance[i]);
      distance[i] = (dist_sensors[i].distance())/2;
    }
  }
  //Serial.print("Distance = ");
  //Serial.println(distance);
  delay(1000); 
}
