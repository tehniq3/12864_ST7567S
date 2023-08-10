/* NTP clock on 2.2" monochrome i2c display with ST7567S driver (128x64 i2c)
 * original design by niq_ro: https://github.com/tehniq3/ 
 * v.1 - base NTP clock (using info from https://github.com/tehniq3/eink_NTPclock )
 * v.1b - changed initialization of display
 * v.1c - changed for 1.3" OLED display SH1107 driver (64x128 i2c)
 * v.1c2 - set to 12-hour format
 * v.1d - changed fr 0.96"OLED display SSD1306 driver (64x128 i2c)
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

//U8G2_ST7567_ENH_DG128064I_F_SW_I2C u8g2(U8G2_R0, SCL, SDA, U8X8_PIN_NONE);
//U8G2_ST7567_ENH_DG128064I_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
//U8G2_SH1107_64X128_F_HW_I2C u8g2(U8G2_R1, /* reset=*/ U8X8_PIN_NONE);  // https://github.com/olikraus/u8g2/wiki/u8g2setupcpp#sh1107-64x128
//U8G2_SSD1306_128X64_NONAME_Fc_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);  // as at https://github.com/olikraus/u8g2/wiki/u8g2setupcpp#ssd1306-128x64_noname
//U8G2_ST7567_ENH_DG128064I_F_SW_I2C u8g2(U8G2_R0, SCL, SDA, U8X8_PIN_NONE);
U8G2_ST7567_ENH_DG128064I_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

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
byte pm = 0;  // AM -> pm = 0, PM -> pm = 1;

void setup()
{
    u8g2.setI2CAddress (0x7E);  //(0x3F * 2);
    u8g2.begin();
    u8g2.setContrast(255);
    u8g2.clearBuffer(); // clear the internal memory
     u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_profont12_mr); 
  //  u8g2.setFont(u8g2_font_ncenB08_tr); 
    u8g2.drawStr(0, 20, "NTP clock by niq_ro."); // write something to the internal memory  
    } while (u8g2.nextPage() );  
  //  u8g2.sendBuffer(); // transfer internal memory to the display
    delay(3000);
   
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
if (ora >= 12)
  pm = 1;
else
  pm = 0;
 ora = ora % 12;
 if (ora == 0) ora = 12;
minut = timeClient.getMinutes();
secunda = timeClient.getSeconds();

ceas();
delay(500);
}  // end main loop


void ceas()
{
  char m_str[3];
  strcpy(m_str, u8x8_u8toa(minut, 2));   /* convert m to a string with two digits */
  char h_str[3];
  strcpy(h_str, u8x8_u8toa(ora, 2));
  
  u8g2.firstPage();
  do {
//    u8g2.setFont(u8g2_font_ncenB08_tr); 
    u8g2.setFont(u8g2_font_profont12_mr); 
 //   u8g2.drawStr(0, 10, "22.07.2023 - ");
    u8g2.setCursor(0,10);
    if (zi < 10) u8g2.print("0");
    u8g2.print(zi);
    u8g2.print("-");
    if (luna < 10) u8g2.print("0");
    u8g2.print(luna);
    u8g2.print("-");
    u8g2.print(an);
    u8g2.print(", ");
     
    u8g2.setCursor(70,10);
    if (minut%2 == 0)
    u8g2.print(weekDays1[zi2]);
    else
    u8g2.print(weekDays2[zi2]);
    u8g2.drawStr(10, 63, "arduinotehniq.com"); // write something to the internal memory
    u8g2.setFont(u8g2_font_7Segments_26x42_mn);
  //  u8g2.drawStr(0,54,h_str);
    if (ora > 9 ) 
    {
    u8g2.setCursor(0,54);
    }
    else
    {
    u8g2.setCursor(32,54);
    }    
    u8g2.print(ora);    
    if (secunda%2 == 0)
        u8g2.drawStr(58,54,":"); 
    u8g2.drawStr(69,54,m_str);

   u8g2.setFont(u8g2_font_freedoomr10_mu); // https://github.com/olikraus/u8g2/wiki/fntgrpu8gw
   if (pm == 1)
   u8g2.drawStr(0,30,"P"); 
   else
   u8g2.drawStr(0,30,"A"); 
   u8g2.drawStr(0,43,"M");
    
  } while (u8g2.nextPage() );
}

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
