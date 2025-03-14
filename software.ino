
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <U8g2lib.h>
//https://github.com/PowerBroker2/ELMduino
#include <SoftwareSerial.h>
#include "ELMduino.h"
#include <OneWire.h>
#include <DallasTemperature.h>

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);

#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define upkey 6
#define downkey 8
#define menukey 7
#define triggerkey 9

OneWire oneWire(15);
DallasTemperature sensors(&oneWire);
int maxA = 35;
float curA;
int curW;
int R1 = 1004;
int R2 = 983;
int screen = 0; //switch between main screen and menu screen
int menu = 0; //menu position
int pwm = 253;
int maxpwm;
int btemp = 0;
int mtemp = 0;
float bvoltage;
float cvoltage;
int bcharge;
float analog1;
float analog2 = 20000;
float coilr;
float refcoilr = 0.25;
float alpha = 0.003;
int coilt = 250;
int calt;
int batp;
int tref = 20;
// 'bat3', 7x9px
const unsigned char bat3 [] PROGMEM = {
  0xc6, 0x10, 0x6c, 0x6c, 0x6c, 0x54, 0x6c, 0x7c, 0x00
};
// 'bat4', 7x9px
const unsigned char bat4 [] PROGMEM = {
  0xc6, 0x10, 0x7c, 0x7c, 0x7c, 0x00, 0x00, 0x00, 0x00
};
// 'coil', 7x9px
const unsigned char coil [] PROGMEM = {
  0xbe, 0xc6, 0xfa, 0xc6, 0xbe, 0xc6, 0xfa, 0xc6, 0xbe
};
// 'mosfet', 7x9px
const unsigned char mosfet [] PROGMEM = {
  0x82, 0xba, 0xba, 0x82, 0x82, 0x82, 0x82, 0xaa, 0xaa
};

// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 128)
const int epd_bitmap_allArray_LEN = 4;
const unsigned char* epd_bitmap_allArray[4] = {
  bat3,
  bat4,
  coil,
  mosfet
};

void setup() {
  // put your setup code here, to run once:
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);

  u8g2.begin();
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_NokiaSmallPlain_tr);
    u8g2.setFontMode(0);
    u8g2.setDrawColor(1);
    u8g2.setCursor(0, 8);
    u8g2.print("Epecior OS v1.0B");
    u8g2.setCursor(0, 16);
    u8g2.print("starting temperature sensors");
  } while ( u8g2.nextPage() );
  sensors.begin();
  delay(100);
  sensors.requestTemperatures();
  btemp = sensors.getTempCByIndex(1);
  mtemp = sensors.getTempCByIndex(0);


  u8g2.firstPage();
  do {
    u8g2.setFontMode(0);
    u8g2.setDrawColor(1);
    u8g2.setCursor(0, 8);
    u8g2.print("Epecior OS v1.0B");
    u8g2.setCursor(0, 16);
    u8g2.print("starting temperature sensors");
    u8g2.setCursor(0, 24);
    u8g2.print("sensors started T1="); u8g2.print(btemp); u8g2.print(" T2="); u8g2.print(mtemp);
    u8g2.setCursor(0, 32);
    u8g2.print("starting ADC sensor");
  } while ( u8g2.nextPage() );

  //ads1115.begin();
  analog1 = analogRead(A0);
  bvoltage = (analog1 * 10) / 1024;
  //analog2 = ads1015.readADC_SingleEnded(1);
  //cvoltage = (analog2 * 6.144)/32768;
  coilr = ((R2 * bvoltage) / cvoltage) - R2 - R1;
  calt = sensors.getTempCByIndex(0);


  u8g2.firstPage();
  do {
    u8g2.setFontMode(0);
    u8g2.setDrawColor(1);
    u8g2.setCursor(0, 8);
    u8g2.print("Epecior OS v1.0B");
    u8g2.setCursor(0, 16);
    u8g2.print("sensors started T1="); u8g2.print(btemp); u8g2.print(" T2="); u8g2.print(mtemp);
    u8g2.setCursor(0, 24);
    u8g2.print("starting ADC sensor");
    u8g2.setCursor(0, 32);
    u8g2.print("ADC started"); u8g2.print(bvoltage); u8g2.print(" vcoil="); u8g2.print(cvoltage);
    u8g2.setCursor(0, 40);
    )                   u8g2.print("vbat="); u8g2.print (bvoltage ) ;
                   u8g2.print("calibrating coil resistance...");
                   u8g2.setCursor(0, 48);
                   u8g2.print("coil calibrated R="); u8g2.print(coilr); u8g2.print("@"); u8g2.print(calt);
  } while ( u8g2.nextPage() );

  delay(5000);
}

void loop() {

  //read values
  analog1 = analogRead(A0);

  sensors.requestTemperatures();
  btemp = sensors.getTempCByIndex(1);
  mtemp = sensors.getTempCByIndex(0);


  u8g2.firstPage();
  do {
    switch (screen) {
      case 0:
        u8g2.setFont(u8g2_font_helvR08_tr);
        u8g2.setFontMode(0);
        u8g2.setDrawColor(1);
        u8g2.setCursor(0, 0);
        u8g2.drawRFrame(0, 0, 128, 12, 3);
        u8g2.drawXBMP(3, 2, 8, 9, bat4);
        u8g2.setCursor(13, 10); u8g2.print(batp); u8g2.print("%");
        u8g2.drawXBMP(44, 2, 8, 9, bat3);
        u8g2.setCursor(53, 10); u8g2.print(btemp); u8g2.print("C");
        u8g2.drawXBMP(84, 2, 8, 9, mosfet);
        u8g2.setCursor(93, 10); u8g2.print(mtemp); u8g2.print("C");
        u8g2.drawRFrame(0, 14, 36, 50, 3);
        u8g2.setCursor(3, 62); u8g2.print(pwm);
        u8g2.setCursor(3, 52); u8g2.print(coilr); u8g2.print("R");
        u8g2.setCursor(3, 42); u8g2.print(coilt); u8g2.print("C");
        u8g2.setCursor(3, 32); u8g2.print((int)curA); u8g2.print("A");
        if (pwm == maxpwm) {
          u8g2.drawRBox(73, 50, 54, 14, 3);
          u8g2.setDrawColor(0);
          u8g2.setFontMode(1);
          u8g2.setCursor(75, 60); u8g2.print("max power");
          u8g2.setDrawColor(1);
          u8g2.setFontMode(0);
        }
        u8g2.setFont(u8g2_font_bubble_tr);
        u8g2.setCursor(37, 47); u8g2.print(curW); u8g2.print("W");
        break;

      case 1:
        break;

      case 2:

        break;
    }
  } while ( u8g2.nextPage() );
  calculate();
  if (pwm > maxpwm) {
    pwm = maxpwm;
  }
}

void calculate() {
  //battery
  bvoltage = (analog1 * 10) / 1024;
  batp = ((bvoltage - 6) / 2.4) * 100;
  if (batp > 100) {
    batp = 100;
  } else if (batp < 0) {
    batp = 0;
  }
  //coil
  cvoltage = (R2 / (R1 + R2 + 0.255)) * bvoltage; //test
  //cvoltage = (analog2 * 6.144)/32768;
  coilr = ((R2 * bvoltage) / cvoltage) - R2 - R1;


  //high power and safety stuff
  curA = (bvoltage / coilr) * ((float)pwm / 255);
  maxpwm = (maxA * (coilr * 255)) / bvoltage;
  curW = curA * bvoltage;
  coilt = ((coilr / refcoilr) + alpha * tref - 1) / alpha;
}
