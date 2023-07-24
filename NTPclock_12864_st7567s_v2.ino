/* NTP clock on 2.2" monochrome i2c display with ST7567S driver
 * original design by niq_ro: https://github.com/tehniq3/ 
 * v.1 - base NTP clock (using info from https://github.com/tehniq3/eink_NTPclock )
 * v.2 - changed display mode (Full screen buffer mode)
*/

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <U8g2lib.h>
#include <Wire.h>

// https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/
//#define SCL_PIN SCL  // SCL pin of OLED. Default: D1 (ESP8266) or D22 (ESP32)
//#define SDA_PIN SDA  // SDA pin of OLED. Default: D2 (ESP8266) or D21 (ESP32)
#define u8g2_PIN_NONE 100

U8G2_ST7567_ENH_DG128064I_F_SW_I2C u8g2(U8G2_R0, SCL, SDA, U8X8_PIN_NONE);

const long timezoneOffset = 2; // ? hours
const char          *ntpServer  = "pool.ntp.org"; // change it to local NTP server if needed
const unsigned long updateDelay = 900000;         // update time every 15 min
const unsigned long retryDelay  = 5000;           // retry 5 sec later if time query failed

unsigned long tpafisare;
unsigned long tpinfo = 60000;  // 15000 for test in video

unsigned long lastUpdatedTime = updateDelay * -1;
unsigned int  second_prev = 0;
bool          colon_switch = false;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer);

byte DST = 1;
byte DST0;
bool updated;
byte a = 0;

int ora = 20;
int minut = 24;
int secunda = 0;
int zi, zi2, luna, an;
String weekDays1[7]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String weekDays2[7]={"Duminica", "Luni", "Marti", "Miercuri", "Joi", "Vineri", "Sambata"};


void setup()
{
    u8g2.setI2CAddress (0x7E);  //(0x3F * 2);
    u8g2.begin();
    u8g2.clearBuffer(); // clear the internal memory
    u8g2.setFont(u8g2_font_ncenB08_tr); 
    u8g2.drawStr(0, 10, "NTP clock by niq_ro."); // write something to the internal memory
    u8g2.sendBuffer(); // transfer internal memory to the display
    delay(1000);
    u8g2.clearBuffer();          // clear the internal memory
   
   //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    //reset saved settings
    //wifiManager.resetSettings();
    
    //set custom ip for portal
    //wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

    //fetches ssid and pass from eeprom and tries to connect
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration

    wifiManager.autoConnect("AutoConnectAP");
      
    //or use this for auto generated name ESP + ChipID
    //wifiManager.autoConnect();

    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");

    delay(500);

timeClient.setTimeOffset((timezoneOffset + DST)*3600);
timeClient.begin();
}


void loop() {
   if (WiFi.status() == WL_CONNECTED && millis() - lastUpdatedTime >= updateDelay) {
    updated = timeClient.update();
    if (updated) {
      Serial.println("NTP time updated.");
      getYear();
      getMonth();
      getDay();
      zi2 = timeClient.getDay();
      lastUpdatedTime = millis();
    } else {
      Serial.println("Failed to update time. Waiting for retry...");
      lastUpdatedTime = millis() - updateDelay + retryDelay;
    }
  } else {
    if (WiFi.status() != WL_CONNECTED) Serial.println("WiFi disconnected!");
  }
 
unsigned long t = millis();

ora = timeClient.getHours();
minut = timeClient.getMinutes();
secunda = timeClient.getSeconds();

  u8g2.setFont(u8g2_font_ncenB08_tr); 
    u8g2.setCursor(0,10);
    u8g2.print(zi/10);
    u8g2.print(zi%10);
    u8g2.print("-");
    u8g2.print(luna/10);
    u8g2.print(luna%10);
    u8g2.print("-");
    u8g2.print(an);
    u8g2.print(", ");
     
    u8g2.setCursor(63,10);
    if (minut%2 == 0)
    u8g2.print(weekDays1[zi2]);
    else
    u8g2.print(weekDays2[zi2]);
    u8g2.drawStr(10, 63, "arduinotehniq.com"); // write something to the internal memory
 u8g2.setFont(u8g2_font_7Segments_26x42_mn);
    u8g2.setCursor(0,54);
    u8g2.print(ora/10);
    u8g2.print(ora%10);
    if (secunda%2 == 0)
        u8g2.drawStr(58,54,":"); 
    u8g2.setCursor(69,54);
    u8g2.print(minut/10);
    u8g2.print(minut%10);   
    u8g2.sendBuffer();          // transfer internal memory to the displayj
delay(500);
    u8g2.clearBuffer();          // clear the internal memory
}  // end main loop


 void getYear() {
  time_t rawtime = timeClient.getEpochTime();
  struct tm * ti;
  ti = localtime (&rawtime);
  an = ti->tm_year + 1900;
  Serial.print("year = ");
  Serial.println(an);
}

void getMonth() {
  time_t rawtime = timeClient.getEpochTime();
  struct tm * ti;
  ti = localtime (&rawtime);
  luna = ti->tm_mon + 1;
  Serial.print("month = ");
  Serial.println(luna);
}

void getDay() {
  time_t rawtime = timeClient.getEpochTime();
  struct tm * ti;
  ti = localtime (&rawtime);
  zi = ti->tm_mday;
  Serial.print("day = ");
  Serial.println(zi);
}
