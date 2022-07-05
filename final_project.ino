#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include "Fligree.h"

#define max_timeout 100 // Second
 
//Variables
int i = 0, pD0=0, pD1=1, pD2=2, pD3=3, pD4=4, pA0=34;
String pin_D0, pin_D1, pin_D2, pin_D3, pin_D4, pin_A0;
int statusCode;
const char* ssid = "text";
const char* passphrase = "text";
String st;
String content;


String url = "http://192.168.0.102:8000/";

String uqid = "";
String username = "";
String password = "";
 
//Function Decalration
bool testWifi(String username, String password, String uqid);
void launchWeb(void);
void setupAP(void);
 
//Establishing Local server at port 80 whenever required
ESP8266WebServer server(80);
Fligree esp;

void setup()
{
 
  Serial.begin(115200); //Initialising if(DEBUG)Serial Monitor
  Serial.println();
  Serial.println("Disconnecting previously connected WiFi");
  WiFi.disconnect();
  EEPROM.begin(512); //Initialasing EEPROM
  delay(10);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println();
  Serial.println();
  Serial.println("Startup");

  Serial.println("Enter 1 for cleaing eeprom");
  while (Serial.available() == 0 and millis() < max_timeout*100){
    Serial.print("!"); 
    delay(300);
   }
  Serial.println("Debuging time over... ");
   
  int IC = Serial.parseInt();
  if (IC == 1){
    Serial.println("Clearing EEPROM");
    for (int i = 0; i < 256; ++i) {
          EEPROM.write(i, 0);
        }
    delay(10);
    Serial.println("Continue");
  }
  else{
    Serial.println("EEPROM isn't cleared !");
  }
 
  //---------------------------------------- Read EEPROM for SSID and pass
    Serial.println("Reading EEPROM ssid");
 
    String esid;
    for (int i = 0; i < 32; ++i)
    {
      esid += char(EEPROM.read(i));
    }
    Serial.println();
    Serial.print("SSID: ");
    Serial.println(esid);
    Serial.println("");
    Serial.println("Reading EEPROM pass");
 
    String epass = "";
    for (int i = 32; i < 96; ++i)
    {
      epass += char(EEPROM.read(i));
    }
    Serial.print("PASS: ");
    Serial.println(epass);
    Serial.println("");
    Serial.println("Reading EEPROM USERNAME");


    for (int i = 96; i < 128; ++i){
      username += char(EEPROM.read(i));
    }
    Serial.println();
    Serial.print("USERNAME: ");
    Serial.println(username);
    Serial.println("");
    Serial.println("Reading EEPROM PASSWORD");

    for (int i = 128; i < 192; ++i){
      password += char(EEPROM.read(i));
    }
    Serial.println();
    Serial.print("PASSWORD: ");
    Serial.println(password);
    Serial.println("");
    Serial.println("Reading EEPROM UQID");

    for (int i = 192; i < 256; ++i){
      uqid += char(EEPROM.read(i));
    }
    Serial.println();
    Serial.print("UQID: ");
    Serial.println(uqid);
    Serial.println("");

 
 
  WiFi.begin(esid.c_str(), epass.c_str());
  if (testWifi(uqid, username, password))
  {
    Serial.println("Succesfully Connected!!!");
    esp.Begin(uqid, username, password);
    
    return;
  }
  else
  {
    Serial.println("Turning the HotSpot On");
    launchWeb();
    setupAP();// Setup HotSpot
  }
 
  Serial.println();
  Serial.println("Waiting.");
  
  while ((WiFi.status() != WL_CONNECTED))
  {
    Serial.print(".");
    delay(100);
    server.handleClient();
  }
 
}
void loop() {
  if ((WiFi.status() == WL_CONNECTED))
  {
 
    pin_D0 = String(digitalRead(D0));
    pin_D1 = String(digitalRead(D1));
    pin_D2 = String(digitalRead(D2));
    pin_D3 = String(digitalRead(D3));
    pin_D4 = String(digitalRead(D4));
  
    pin_A0 = String(analogRead(A0));
  
    esp.GETData("home/");
    delay(5000);
    esp.POSTData("home/", pin_D0, pin_D1, pin_D2, pin_D3, pin_D4, pin_A0);
    delay(5000);
 
  }
  else
  {
  }
 
}
 
 
//-------- Fuctions used for WiFi credentials saving and connecting to it which you do not need to change 
bool testWifi(String username, String password, String uqid)
{
  int c = 0;
  Serial.println("Waiting for Wifi to connect");
  while ( c < 20 ) {
    if (WiFi.status() == WL_CONNECTED)
    {
      if (username.length() == 0 && password.length() == 0 && uqid.length() == 0){
        return false;
      }
      return true;
    }
    delay(500);
    Serial.print("*");
    c++;
  }
  Serial.println("");
  Serial.println("Connect timed out, opening AP");
  return false;
}
 
void launchWeb()
{
  Serial.println("");
  if (WiFi.status() == WL_CONNECTED)
    Serial.println("WiFi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  createWebServer();
  // Start the server
  server.begin();
  Serial.println("Server started");
}
 
void setupAP(void)
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");
  st = "<ol>";
  for (int i = 0; i < n; ++i)
  {
    // Print SSID and RSSI for each network found
    st += "<li>";
    st += WiFi.SSID(i);
    st += " (";
    st += WiFi.RSSI(i);
 
    st += ")";
    st += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
    st += "</li>";
  }
  st += "</ol>";
  delay(100);
  WiFi.softAP("Fligree", "");
  Serial.println("softap");
  launchWeb();
  Serial.println("over");
}
 
void createWebServer()
{
 {
    server.on("/", []() {
 
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      content = "<!DOCTYPE HTML>\r\n<html>Hello from ESP8266 at ";
      content += "<form action=\"/scan\" method=\"POST\"><input type=\"submit\" value=\"scan\"></form>";
      content += ipStr;
      content += "<p>";
      content += st;
      content += "</p><form method='get' action='setting'><label>SSID: </label><input name='ssid' length=32><label>PASSWORD: </label><input name='pass' length=64><br><hr><h4> Enter credential for website </h4><br><label>USERNAME: </label><input name='username' length=32><label>PASSWORD: </label><input name='password' length=64><label>UQID: </label><input name='uqid' length=64><input type='submit'></form>";
      content += "</html>";
      server.send(200, "text/html", content);
    });
    server.on("/scan", []() {
      //setupAP();
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
 
      content = "<!DOCTYPE HTML>\r\n<html>go back";
      server.send(200, "text/html", content);
    });
 
    server.on("/setting", []() {
      String qsid = server.arg("ssid");
      String qpass = server.arg("pass");
      if (qsid.length() > 0 && qpass.length() > 0) {
        Serial.println("clearing eeprom");
        for (int i = 0; i < 256; ++i) {
          EEPROM.write(i, 0);
        }
        Serial.println(qsid);
        Serial.println("");
        Serial.println(qpass);
        Serial.println("");
        Serial.println(username);
        Serial.println("");
        Serial.println(password);
        Serial.println("");
        Serial.println(uqid);
        Serial.println("");
 
        Serial.println("writing eeprom ssid:");
        for (int i = 0; i < qsid.length(); ++i)
        {
          EEPROM.write(i, qsid[i]);
          Serial.print("Wrote: ");
          Serial.println(qsid[i]);
        }
        Serial.println("writing eeprom pass:");
        for (int i = 0; i < qpass.length(); ++i)
        {
          EEPROM.write(32 + i, qpass[i]);
          Serial.print("Wrote: ");
          Serial.println(qpass[i]);
        }

        Serial.println("writing eeprom username:");
        for (int i = 0; i < username.length(); ++i)
        {
          EEPROM.write(96 + i, username[i]);
          Serial.print("Wrote: ");
          Serial.println(username[i]);
        }

        Serial.println("writing eeprom password:");
        for (int i = 0; i < password.length(); ++i)
        {
          EEPROM.write(128 + i, password[i]);
          Serial.print("Wrote: ");
          Serial.println(password[i]);
        }

        Serial.println("writing eeprom uqid:");
        for (int i = 0; i < uqid.length(); ++i)
        {
          EEPROM.write(196 + i, uqid[i]);
          Serial.print("Wrote: ");
          Serial.println(uqid[i]);
        }

        EEPROM.commit();
        
 
        content = "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}";
        statusCode = 200;
        ESP.reset();
      } else {
        content = "{\"Error\":\"404 not found\"}";
        statusCode = 404;
        Serial.println("Sending 404");
      }
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(statusCode, "application/json", content);
 
    });
  } 
}
