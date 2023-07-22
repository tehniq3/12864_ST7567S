/*
  Clock.ino
  
  Demonstrates a clock-like display with a very huge font.
  Also shows usage of u8x8_u8toa().

  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Copyright (c) 2016, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  

  Nicu FLORICA (niq_ro) used info from:
   https://aliexpress.ru/item/1005004617618178.html?sku_id=12000029849830576
   https://pastebin.com/cMgTQMQd
  to create a pseudo-clock
*/

#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

/*
  U8g2lib Example Overview:
    Frame Buffer Examples: clearBuffer/sendBuffer. Fast, but may not work with all Arduino boards because of RAM consumption
    Page Buffer Examples: firstPage/nextPage. Less RAM usage, should work with all Arduino boards.
    U8x8 Text Only Example: No RAM usage, direct communication with display controller. No graphics, 8x8 Text only. 
*/

//#define SCL_PIN SCL  // SCL pin of OLED. Default: D1 (ESP8266) or D22 (ESP32)
//#define SDA_PIN SDA  // SDA pin of OLED. Default: D2 (ESP8266) or D21 (ESP32)
#define u8g2_PIN_NONE 100

U8G2_ST7567_ENH_DG128064I_F_SW_I2C u8g2(U8G2_R0, SCL, SDA, U8X8_PIN_NONE);

String weekDays[7]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


void setup(void) {

  u8g2.setI2CAddress (0x7E);  //(0x3F * 2); 

  u8g2.begin();  
}

uint8_t h = 23;
uint8_t m = 24;
byte i = 0;

void loop(void) {
  
  char m_str[3];
  strcpy(m_str, u8x8_u8toa(m, 2));		/* convert m to a string with two digits */
  char h_str[3];
  strcpy(h_str, u8x8_u8toa(h, 2));
  
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr); 
    u8g2.drawStr(0, 10, "22.07.2023 - ");
    u8g2.setCursor(63,10);
    u8g2.print(weekDays[i]);
    u8g2.drawStr(10, 63, "arduinotehniq.com"); // write something to the internal memory
    u8g2.setFont(u8g2_font_7Segments_26x42_mn);
    u8g2.drawStr(0,54,h_str);
    if ((millis()/1000)%2 == 0)
        u8g2.drawStr(58,54,":"); 
    u8g2.drawStr(69,54,m_str);
  } while (u8g2.nextPage() );
  
  delay(500);
  m++;
  if (m == 60)
      m = 0;
  i++;
  if (i > 6)
      i = 0;
    
} // end main loop
