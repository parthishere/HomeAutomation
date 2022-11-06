#include "Fligree.h"
// #include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <ThingSpeak.h>
#include <ESP8266HTTPClient.h>
#include <Servo.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16



const size_t MAX_CONTENT_SIZE = 512;

#define max_timeout 100

#define url "http://192.168.0.103:8000/"
#define ENDPOINT_GET "api/get/"
#define ENDPOINT_POST "api/post/"

unsigned long counterChannelNumber = 1922816;                // Channel ID
const char * myCounterReadAPIKey = "2MLZDDQIBA7S319S";      // Read API Key
const int FieldNumber1 = 1;

int sw = 5, IC = 0;
const int LED1 = 1;
//Variables

String pin_D0, pin_D1, pin_D2, pin_D3, pin_D4, pin_A0;
int statusCode;
unsigned int time_send_scanned_network;
const char* ssid = "text";
const char* passphrase = "text";
char* cString;
String st, another_st, new_ssid_from_bt, new_pass_from_bt;
String content, ssid_pass;
String payload, code;
char  B, buff[1000]; // character and character string buffer
int  buffCount = 0; // how many characters in the buffer
int  buffCharLimit = 1000; // maximum characters allowed


String uqid = "";
String username = "";
String password = "";


//Function Decalration
bool checkWifi_connection(String username, String password, String uqid);
void func(String payload);
void createWebServer();
void send_data_to_bt_and_setup_sta(void);
void end_line_and_print(void);
void manage_bt_data(void);


Fligree esp;
SoftwareSerial btSerial(D4, D3);  // RX,
SoftwareSerial ESPserial(2, 3);
WiFiClient client;
Servo myservo;  // create servo object to control a servo


void setup() {

  myservo.attach(16);
  pinMode(sw, INPUT);
  
  btSerial.begin(9600);
  Serial.begin(9600);
  delay(100);
  ESPserial.begin(115200);
  delay(10);
  ESPserial.println("AT+IPR=9600");
  delay(1000);
  ESPserial.end();
  // Start the software serial for communication with the ESP8266
  ESPserial.begin(9600);

  Serial.println("Ready");
  ESPserial.println("AT+GMR");
  delay(1000);

  Serial.println("Disconnecting previously connected WiFi");
  WiFi.disconnect();
  EEPROM.begin(512);  //Initialasing EEPROM
  delay(100);


  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  
  display.display();
  delay(1000); 

  display.clearDisplay();

  display.drawPixel(10, 10, SSD1306_WHITE);

  

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Startup"));
  display.display();


  Serial.println("Enter 1 for cleaing eeprom");
  while (millis() < max_timeout * 100) {
    if (Serial.available() == 0) {
      Serial.println("Reading");
      IC = Serial.parseInt();
    }
    Serial.print("!");
    delay(300);
  }
  Serial.println("Debuging time over... ");


  if (IC == 1) {
    Serial.println("Clearing EEPROM");
    for (int i = 0; i < 256; ++i) {
      EEPROM.write(i, 0);
    }
    delay(10);
    Serial.println("Continue");
  } else {
    Serial.println("EEPROM isn't cleared !");
  }

  // Read EEPROM for SSID and pass
  Serial.println("Reading EEPROM ssid");

  String esid;
  for (int i = 0; i < 32; ++i) {
    esid += char(EEPROM.read(i));
  }
  Serial.println();
  Serial.print("SSID: ");
  Serial.println(esid);
  Serial.println("");
  Serial.println("Reading EEPROM pass");

  String epass = "";
  for (int i = 32; i < 96; ++i) {
    epass += char(EEPROM.read(i));
  }
  Serial.print("PASS: ");
  Serial.println(epass);
  Serial.println("");
  Serial.println("Reading EEPROM username");


  for (int i = 96; i < 128; ++i) {
    username += char(EEPROM.read(i));
  }
  Serial.print("USERNAME: ");
  Serial.println(username);
  Serial.println("");
  Serial.println("Reading EEPROM PASSWORD");

  for (int i = 128; i < 192; ++i) {
    password += char(EEPROM.read(i));
  }
  Serial.print("PASSWORD: ");
  Serial.println(password);
  Serial.println("");
  Serial.println("Reading EEPROM UQID");

  for (int i = 196; i < 224; ++i) {
    uqid += char(EEPROM.read(i));
  }

  Serial.print("UQID: ");
  Serial.println(uqid);
  Serial.println("");


  
  
  
  WiFi.begin(esid.c_str(), epass.c_str());
  if (checkWifi_connection(uqid, username, password)) {
    Serial.println("Succesfully Connected!!!");
    esp.begin(url, username, password, uqid);
    ThingSpeak.begin(client);
    return;
  } else {
    Serial.println("Turning the Bluetooth On");
    // Setup Bluetooth
    Serial.print("State of switch");
    Serial.print(digitalRead(sw));
    if (digitalRead(sw) == HIGH) {
      send_data_to_bt_and_setup_sta();
    }
    else {
      Serial.println("Do nothing");
    }
  }

  Serial.println();
  Serial.println("Waiting...");

  while ((WiFi.status() != WL_CONNECTED)) {
    delay(1);
    send_data_to_bt_and_setup_sta();
  }

}



void loop() {

  int A = ThingSpeak.readLongField(counterChannelNumber, FieldNumber1, myCounterReadAPIKey);
  if (A == 1){
    myservo.write(90);
  }
  else{
    myservo.write(0);
  }
  Serial.println(A);

}



bool checkWifi_connection(String username, String password, String uqid) {
  int c = 0;
  Serial.println("Waiting for Wifi to connect");
  while (c < 20) {
    if (WiFi.status() == WL_CONNECTED) {
      if (username.length() == 0 && password.length() == 0 && uqid.length() == 0) {
        return false;
      }
      return true;
    }
    delay(500);
    Serial.print("*");
    c++;
  }
  Serial.println("");
  Serial.println("Connection timed out, password or username are incorrect");
  return false;
}



void send_data_to_bt_and_setup_sta(void) {

  while (digitalRead(sw) == HIGH)
  {

    while (ESPserial.available())
    {
      ssid_pass = ESPserial.readString();
      Serial.println(ssid_pass);

      for (int i = 0; i < ssid_pass.length(); i++)
      {
        if (ssid_pass.substring(i, i + 1) == ",")
        {
          new_ssid_from_bt = ssid_pass.substring(0, i);
          new_pass_from_bt = ssid_pass.substring(i + 1);
          Serial.print("SSID = "); Serial.println(new_ssid_from_bt);
          Serial.print("Password = "); Serial.println(new_pass_from_bt);
          delay(2000);

          int n1 = new_ssid_from_bt.length();
          char char_array1[n1 + 1];
          strcpy(char_array1, new_ssid_from_bt.c_str());

          int n2 = new_pass_from_bt.length();
          char char_array2[n2 + 1];
          strcpy(char_array2, new_pass_from_bt.c_str());


          Serial.println("Clearing EEPROM");
          for (int i = 0; i < 256; ++i) {
            EEPROM.write(i, 0);
          }
          delay(10);
          Serial.println("Continue");

          Serial.println("writing eeprom ssid:");
          for (int i = 0; i < new_ssid_from_bt.length(); ++i) {
            EEPROM.write(i, new_ssid_from_bt[i]);
            Serial.print("Wrote: ");
            Serial.println(new_ssid_from_bt[i]);
          }

          Serial.println("writing eeprom pass:");
          for (int i = 0; i < new_pass_from_bt.length(); ++i) {
            EEPROM.write(32 + i, new_pass_from_bt[i]);
            Serial.print("Wrote: ");
            Serial.println(new_pass_from_bt[i]);
          }

          //          Serial.println("writing eeprom username:");
          //          for (int i = 0; i < username.length(); ++i) {
          //            EEPROM.write(96 + i, username[i]);
          //            Serial.print("Wrote: ");
          //            Serial.println(username[i]);
          //          }
          //
          //          Serial.println("writing eeprom password:");
          //          for (int i = 0; i < password.length(); ++i) {
          //            EEPROM.write(128 + i, password[i]);
          //            Serial.print("Wrote: ");
          //            Serial.println(password[i]);
          //          }
          //
          //          Serial.println("writing eeprom uqid:");
          //          for (int i = 0; i < uqid.length(); ++i) {
          //            EEPROM.write(196 + i, uqid[i]);
          //            Serial.print("Wrote: ");
          //            Serial.println(uqid[i]);
          //          }

          delay(100);
          EEPROM.commit();
          delay(100);
          ESP.reset();
          WiFi.begin(char_array1, char_array2);
        }
        Serial.println("");
      }
    }
  }

}
