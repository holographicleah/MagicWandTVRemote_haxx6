/* Magic Wand TV Remote
   Updated December 2014
   By Allie Fauer
   Based on Arduino User JohnChi's MPU-6050 Short Example sketch and Chris Young's IRsend Demo from the IR Lib
   Public Domain
   http://www.instructables.com/id/Magic-Wand-TV-Remote/
*/
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 6
Adafruit_NeoPixel strip = Adafruit_NeoPixel(7, PIN, NEO_GRB + NEO_KHZ800);

#include<Wire.h>
const int MPU = 0x68; // I2C address of the MPU-6050
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;


uint8_t myColors[][3] = {{200,0,50},   // green
                         {200, 200, 200},   // yellow 
                         {30, 200, 255},   // blue
                          };
                          
#define FAVCOLORS sizeof(myColors) / 3

void setup()
{
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(9600);

  strip.begin();
  strip.show();
}

void loop() {

  strip.setBrightness(40);


  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 14, true); // request a total of 14 registers
  AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)


   
    for (int i = 0; i <= 7; i++) {
      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      strip.setPixelColor(i, strip.Color(0, 155, 50)); 
      strip.show(); // This sends the updated pixel color to the hardware.
    }



if (AcY <3501 && AcY > -7000) {

   Serial.println("point");

   flashRandom(6, 1);  // first number is 'wait' delay, shorter num == shorter twinkle
   flashRandom(5, 3);  // second number is how many neopixels to simultaneously light up
   flashRandom(5, 2);
   
}





  if (AcY < -5500 && AcZ < 9000) { // tap up
    for (int i = 0; i <= 3; i++) {

      //My_Sender.send(NEC, 0x20DF40BF, 32); // Volume Up
      delay(100);
    }
  }



  if (AcY <8000 && AcY > 3500) { // tap down
    
    Serial.println("tap");


    for (int i = 0; i <= 7; i++) {
      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      strip.setPixelColor(i, strip.Color(255, 255, 255)); 
      strip.show(); // This sends the updated pixel color to the hardware.
    }

    delay(300);
    
    for (int i = 0; i <= 7; i++) {
      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      strip.setPixelColor(i, strip.Color(0, 0, 0)); // Moderately bright green color.
      strip.show(); // This sends the updated pixel color to the hardware.
    }


   

  }
}



//////////////////////////defines different neopixel things///////////////////////////////////////



void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}



//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j = 0; j < 1; j++) { //do 1 cycle of chasing
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, c);  //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

void flashRandom(int wait, uint8_t howmany) {
 
  for(uint16_t i=0; i<howmany; i++) {
    // pick a random favorite color!
    int c = random(FAVCOLORS);
    int red = myColors[c][0];
    int green = myColors[c][1];
    int blue = myColors[c][2]; 
 
    // get a random pixel from the list
    int j = random(strip.numPixels());
    
    // now we will 'fade' it in 5 steps
    for (int x=0; x < 5; x++) {
      int r = red * (x+1); r /= 5;
      int g = green * (x+1); g /= 5;
      int b = blue * (x+1); b /= 5;
      
      strip.setPixelColor(j, strip.Color(r, g, b));
      strip.show();
      delay(wait);
    }
    // & fade out in 5 steps
    for (int x=5; x >= 0; x--) {
      int r = red * x; r /= 5;
      int g = green * x; g /= 5;
      int b = blue * x; b /= 5;
      
      strip.setPixelColor(j, strip.Color(r, g, b));
      strip.show();
      delay(wait);
    }
  }
  // LEDs will be off when done (they are faded to 0)
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


