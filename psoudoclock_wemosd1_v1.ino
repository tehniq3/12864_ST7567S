// https://aliexpress.ru/item/1005004617618178.html?sku_id=12000029849830576
// 128X64 I2C ST7567S COG Graphic Display ARDUINO
//
// https://www.youtube.com/c/LeventeDaradici/videos
// the display was bought from here: https://www.aliexpress.com/item/1005004617618178.html
// short review: https://satelit-info.com/phpBB3/viewtopic.php?f=172&t=3338
//
#include <U8g2lib.h>
#include <Wire.h>

//#define SCL_PIN SCL  // SCL pin of OLED. Default: D1 (ESP8266) or D22 (ESP32)
//#define SDA_PIN SDA  // SDA pin of OLED. Default: D2 (ESP8266) or D21 (ESP32)
#define u8g2_PIN_NONE 100

U8G2_ST7567_ENH_DG128064I_F_SW_I2C u8g2(U8G2_R0, SCL, SDA, U8X8_PIN_NONE);

int ora = 19;
int minut = 35;
int secunda = 47;

int previoussec = 60;
int zo, uo;
int zm, um;
int zs, us;

 
void setup(void) 
     {
        u8g2.setI2CAddress (0x7E);  //(0x3F * 2);
        u8g2.begin();
        u8g2.clearBuffer(); // clear the internal memory
        u8g2.setFont(u8g2_font_ncenB08_tr); 
        //u8g2.setFont(u8g2_font_cu12_tr);
        u8g2.drawStr(0, 10, "NTP clock"); // write something to the internal memory
        u8g2.sendBuffer(); // transfer internal memory to the display
        delay(1000);
      }
void loop(void) 
     {
   u8g2.setFont(u8x8_font_px437wyse700a_2x2_r) ;
 // u8g2.drawString(2,0, "19.07.2023" );

  if (millis()/1000 != previoussec)
  {
    previoussec = millis()/1000;
    secunda++;
  } 
  if (secunda > 59)
  {
    secunda = 0;
    minut++;
  }
  if (minut > 59)
  {
    minut = 0;
    ora++;
  }
  if (ora > 23)
  {
    ora = 0;
  } 
zo = ora/10;
uo = ora%10;
zm = minut/10;
um = minut%10;
zs = secunda/10;
us = secunda%10;;

 u8g2.setFont (u8x8_font_inb33_3x6_n);
 u8g2.setCursor(0,2);
 u8g2.print(zo);
 u8g2.print(uo);
 u8g2.print(":");
 u8g2.print(zm);
 u8g2.print(um);
 u8g2.print(":");
 u8g2.print(zs);
 u8g2.print(us);

  u8g2.setFont(u8x8_font_chroma48medium8_r);
 // u8g2.drawString (2,7, "www.arduinotehniq.com");
  delay(1000);
  u8g2.clear();
     }  // end main loop
