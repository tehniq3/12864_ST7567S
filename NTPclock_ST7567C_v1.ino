// https://www.hackster.io/alankrantas/esp8266-ntp-clock-on-ssd1306-oled-arduino-ide-35116e

#ifdef ESP8266
#include <ESP8266WiFi.h>
#endif

#ifdef ESP32
#include <WiFi.h>
#endif

#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <U8g2lib.h>

// ========== user settings ==========

const char          *ssid          = "bbk2";             // your wifi name
const char          *pw            = "internet2";             // your wifi password
const char          *ntpServer     = "pool.ntp.org"; // change it to local NTP server if needed
const unsigned long updateDelay    = 10800;            // NTP update interval (unit: seconds, default: 3 hours)
const long          timezoneOffset = 0;              // timezone offset (unit: hours)

//#define SCL_PIN SCL  // SCL pin of OLED. Default: D1 (ESP8266) or D22 (ESP32)
//#define SDA_PIN SDA  // SDA pin of OLED. Default: D2 (ESP8266) or D21 (ESP32)

// ====================================

const String  weekDays[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
unsigned long lastCheckTime = 0;
unsigned int  second_prev = 0;
bool          colon_switch = false;

// NTPClient: https://github.com/arduino-libraries/NTPClient
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer);

// u8g2 Display constructors: https://github.com/olikraus/u8g2/wiki/u8g2setupcpp
// u8g2 Fonts: https://github.com/olikraus/u8g2/wiki/u8g2reference
//u8g2_SSD1306_128X64_NONAME_SW_I2C u8g2(SCL_PIN, SDA_PIN, u8g2_PIN_NONE);

#define SCL_PIN SCL  // SCL pin of OLED. Default: D1 (ESP8266) or D22 (ESP32)
#define SDA_PIN SDA  // SDA pin of OLED. Default: D2 (ESP8266) or D21 (ESP32)
#define u8g2_PIN_NONE 100

 
U8G2_ST7567_ENH_DG128064I_F_SW_I2C u8g2(U8G2_R0, SCL, SDA, u8g2_PIN_NONE);
//u8g2_ST7567_64X32_HW_I2C u8g2(/* reset=*/ u8g2_PIN_NONE); 

void setup() {
  // serial baud rate: 115200
  Serial.begin(115200);

  // initialize OLED
  //u8g2.setI2CAddress(0x3F * 2);
  u8g2.begin();
  u8g2.setBusClock(400000);
  u8g2.setFont(u8x8_font_7x14B_1x2_f);
  u8g2.drawString(0, 0, "Connecting");
  u8g2.drawString(0, 3, "  to WiFi...");

  // connect to wifi
  Serial.println("Connecting to WiFi...");
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pw);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected. IP:");
  Serial.println(WiFi.localIP());

  // clear OLED
  u8g2.clear();

  // setup NTPClient
  timeClient.setUpdateInterval(updateDelay * 1000);
  timeClient.setTimeOffset(timezoneOffset * 60 * 60);
  timeClient.begin();
  while (!timeClient.update()) timeClient.forceUpdate();
  lastCheckTime = millis();

}

void loop() {

  // check wifi status and NTP updates
  if (millis() - lastCheckTime >= updateDelay * 1000) {
    // reconnect wifi if needed
    if (WiFi.status() != WL_CONNECTED) WiFi.reconnect();
    // update NTP (and force update if needed)
    if (!timeClient.update()) timeClient.forceUpdate();
    lastCheckTime = millis();
  }

  // extract second from board's internal clock
  unsigned int second = timeClient.getSeconds();

  // when the second number changes, extract the rest of values
  // then re-draw the clock texts
  if (second != second_prev) {

    colon_switch = !colon_switch;

    time_t rawtime = timeClient.getEpochTime();
    struct tm * ti;
    ti = localtime(&rawtime);
    unsigned int year = ti->tm_year + 1900;
    unsigned int month = ti->tm_mon + 1;
    unsigned int day = ti->tm_mday;
    unsigned int hour = timeClient.getHours();
    unsigned int minute = timeClient.getMinutes();

    String fYear = String(year);
    String fDate = (month < 10 ? "0" : "") + String(month) + "/" + (day < 10 ? "0" : "") + String(day);
    String fTime = (hour < 10 ? "0" : "") + String(hour) + (colon_switch ? ":" : " ") + (minute < 10 ? "0" : "") + String(minute);
    String weekDay = weekDays[timeClient.getDay()];

    Serial.println("Board time: " + fYear + "/" + fDate + " (" + weekDay + ") " + timeClient.getFormattedTime());

    u8g2.setFont(u8g2_font_lucasarts_scumm_subtitle_o_2x2_f);
    u8g2.drawString(1, 0, strcpy(new char[fDate.length() + 1], fDate.c_str()));
    u8g2.setFont(u8g2_font_pxplusibmcga_f);
    u8g2.drawString(12, 0, strcpy(new char[fYear.length() + 1], fYear.c_str()));
    u8g2.setFont(u8g2_font_victoriamedium8_r);
    u8g2.drawString(12, 1, strcpy(new char[weekDay.length() + 1], weekDay.c_str()));
    u8g2.setFont(u8g2_font_inb33_3x6_f);
    u8g2.drawString(1, 2, strcpy(new char[fTime.length() + 1], fTime.c_str()));

  }

  second_prev = second;

  delay(200);

}
