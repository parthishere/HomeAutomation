#include "Fligree.h"
#include <ArduinoJson.h>
const size_t MAX_CONTENT_SIZE = 512; 

#define max_timeout 50 // Second

#define url "http://192.168.0.103:8000/"
#define ENDPOINT_GET "api/get/"
#define ENDPOINT_POST "api/post/"

//Variables
int i = 0, pD0 = 0, pD1 = 1, pD2 = 2, pD3 = 3, pD4 = 4, pA0 = 34;
String pin_D0, pin_D1, pin_D2, pin_D3, pin_D4, pin_A0;
int statusCode;
const char* ssid = "text";
const char* passphrase = "text";
String st;
String content;
String payload, code;

char q;

String uqid = "";
String username = "";
String password = "";

//Function Decalration
bool testWifi(String username, String password, String uqid);
void launchWeb(void);
void setupAP(void);
void func(String payload);

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
  while (Serial.available() == 0 and millis() < max_timeout * 100) {
    Serial.print("!");
    delay(300);
  }
  Serial.println("Debuging time over... ");

  int IC = Serial.parseInt();
  if (IC == 1) {
    Serial.println("Clearing EEPROM");
    for (int i = 0; i < 256; ++i) {
      EEPROM.write(i, 0);
    }
    delay(10);
    Serial.println("Continue");
  }
  else {
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

  for (int i = 192; i < 224; ++i) {
    uqid += char(EEPROM.read(i));
  }

  Serial.print("UQID: ");
  Serial.println(uqid);
  Serial.println("");



  WiFi.begin(esid.c_str(), epass.c_str());
  if (testWifi(uqid, username, password))
  {
    Serial.println("Succesfully Connected!!!");
    esp.begin(url, username, password, uqid);

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

    while (Serial.available() == 0);
    
    int a = Serial.parseInt();
    
    if (a == 1){
      payload, code = esp.GETData(ENDPOINT_GET);
      Serial.println(payload);
      func(payload);
      Serial.println("Func done");
      
      
    }
    else if (a == 2){
      payload, code = esp.POSTData(ENDPOINT_POST, pin_D0, pin_D1, pin_D2, pin_D3, pin_D4, pin_A0);
      
    }

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




void func(String payload){
    const size_t capacity = JSON_ARRAY_SIZE(5) + 10*JSON_OBJECT_SIZE(3) + 490;
    DynamicJsonBuffer jsonBuffer(capacity);
    
//    const char* json = "[{\"esp\":{\"id\":1,\"unique_id\":\"hsgusbbwi162bs\",\"timestamp\":\"2022-07-08T11:00:42.773325Z\"},\"name\":\"A0\",\"value\":0},{\"esp\":{\"id\":1,\"unique_id\":\"hsgusbbwi162bs\",\"timestamp\":\"2022-07-08T11:00:42.773325Z\"},\"name\":\"D3\",\"value\":0},{\"esp\":{\"id\":1,\"unique_id\":\"hsgusbbwi162bs\",\"timestamp\":\"2022-07-08T11:00:42.773325Z\"},\"name\":\"D4\",\"value\":0},{\"esp\":{\"id\":1,\"unique_id\":\"hsgusbbwi162bs\",\"timestamp\":\"2022-07-08T11:00:42.773325Z\"},\"name\":\"D2\",\"value\":0},{\"esp\":{\"id\":1,\"unique_id\":\"hsgusbbwi162bs\",\"timestamp\":\"2022-07-08T11:00:42.773325Z\"},\"name\":\"D0\",\"value\":0}]";
    
    JsonObject& root = jsonBuffer.parseObject(payload);

    Serial.println("Root");
    Serial.println(root.size());

    for (JsonPair& kv : root) {
    Serial.println(kv.key);
    Serial.println(kv.value.as<char*>());
    }
    
//    JsonObject& root_0 = root[0];
//    JsonObject& root_0_esp = root_0["esp"];
//    int root_0_esp_id = root_0_esp["id"]; // 1
//    const char* root_0_esp_unique_id = root_0_esp["unique_id"]; // "hsgusbbwi162bs"
//    const char* root_0_esp_timestamp = root_0_esp["timestamp"]; // "2022-07-08T11:00:42.773325Z"
//    
//    const char* root_0_name = root_0["name"]; // "A0"
//    int root_0_value = root_0["value"]; // 0
//    
//    JsonObject& root_1 = root[1];
//    
//    JsonObject& root_1_esp = root_1["esp"];
//    int root_1_esp_id = root_1_esp["id"]; // 1
//    const char* root_1_esp_unique_id = root_1_esp["unique_id"]; // "hsgusbbwi162bs"
//    const char* root_1_esp_timestamp = root_1_esp["timestamp"]; // "2022-07-08T11:00:42.773325Z"
//    
//    const char* root_1_name = root_1["name"]; // "D3"
//    int root_1_value = root_1["value"]; // 0
//    
//    JsonObject& root_2 = root[2];
//    
//    JsonObject& root_2_esp = root_2["esp"];
//    int root_2_esp_id = root_2_esp["id"]; // 1
//    const char* root_2_esp_unique_id = root_2_esp["unique_id"]; // "hsgusbbwi162bs"
//    const char* root_2_esp_timestamp = root_2_esp["timestamp"]; // "2022-07-08T11:00:42.773325Z"
//    
//    const char* root_2_name = root_2["name"]; // "D4"
//    int root_2_value = root_2["value"]; // 0
//    
//    JsonObject& root_3 = root[3];
//
//    JsonObject& root_3_esp = root_3["esp"];
//    int root_3_esp_id = root_3_esp["id"]; // 1
//    const char* root_3_esp_unique_id = root_3_esp["unique_id"]; // "hsgusbbwi162bs"
//    const char* root_3_esp_timestamp = root_3_esp["timestamp"]; // "2022-07-08T11:00:42.773325Z"
//    
//    const char* root_3_name = root_3["name"]; // "D2"
//    int root_3_value = root_3["value"]; // 0
//    
//    JsonObject& root_4 = root[4];
//    
//    JsonObject& root_4_esp = root_4["esp"];
//    int root_4_esp_id = root_4_esp["id"]; // 1
//    const char* root_4_esp_unique_id = root_4_esp["unique_id"]; // "hsgusbbwi162bs"
//    const char* root_4_esp_timestamp = root_4_esp["timestamp"]; // "2022-07-08T11:00:42.773325Z"
//    
//    const char* root_4_name = root_4["name"]; // "D0"
//    int root_4_value = root_4["value"]; // 0
}
