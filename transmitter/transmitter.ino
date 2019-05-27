#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#define I2C_ADDRESS 0x3C

const uint64_t pipeOut = 0xE8E8F0F0E1LL;

SSD1306AsciiWire oled;

RF24 radio(7,8);
 
uint8_t col[2];
uint8_t rows;


struct MyData {
  byte throttle;
  byte yaw;
  byte pitch;
  byte roll;
  byte AUX1;
  byte AUX2;
};

MyData data;

void resetData() 
{
 
  data.throttle = 0;
  data.yaw = 127;
  data.pitch = 127;
  data.roll = 127;
  data.AUX1 = 0;
  data.AUX2 = 0;
}

void setup()
{
  
  radio.begin();
  Wire.begin();
  Wire.setClock(400000L);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(pipeOut);
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.setFont(System5x7);
  oled.clear();
  oled.set2X();
  oled.println("initializing...");
  delay(2000);
  oled.println("");
  oled.println("");
  oled.set1X();
  oled.println("\t\t @victor_gpz");
  resetData();
  oled.clear();


  oled.println("ADC0: 9999 ADC1: 9999");
  oled.println("ADC2: 9999 ADC3: 9999");
  oled.println("ADC4: 9999 ADC5: 9999");
  
    
  col[0] = oled.fieldWidth(strlen("ADC0: "));
  col[1] = oled.fieldWidth(strlen("ADC0: 9999 ADC1: "));
  rows = oled.fontRows();
  delay(3000);
  
}




int mapJoystickValues(int val, int lower, int middle, int upper, bool reverse)
{
  val = constrain(val, lower, upper);
  if ( val < middle )
    val = map(val, lower, middle, 0, 128);
  else
    val = map(val, middle, upper, 128, 255);
  return ( reverse ? 255 - val : val );
}

void loop()
{ 
 
  data.throttle = mapJoystickValues( analogRead(A0), 13, 524, 1015, true );
  data.yaw      = mapJoystickValues( analogRead(A1),  1, 505, 1020, true );
  data.pitch    = mapJoystickValues( analogRead(A2), 12, 544, 1021, true );
  data.roll     = mapJoystickValues( analogRead(A3), 34, 522, 1020, true );
  data.AUX1     = digitalRead(9);
  data.AUX2     = digitalRead(10);

  radio.write(&data, sizeof(MyData));

  
  for (uint8_t i = 0; i < 6; i++) {  
    oled.clearField(col[i%2], rows*(i/2), 4);    
    oled.print(analogRead(i));
  }
  
}
