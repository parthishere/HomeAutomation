#include "Fligree.h"
// #include <ArduinoJson.h>
#include <SoftwareSerial.h>
const size_t MAX_CONTENT_SIZE = 512;

#define max_timeout 100

#define url "http://192.168.0.103:8000/"
#define ENDPOINT_GET "api/get/"
#define ENDPOINT_POST "api/post/"

int sw = 5, led, IC = 0;
//Variables
int i = 0, pD0 = 0, pD1 = 1, pD2 = 2, pD3 = 3, pD4 = 4, pA0 = 34;
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

void setup() {


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



  Serial.println("Startup");

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

  //---------------------------------------- Read EEPROM for SSID and pass
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


  pinMode(sw, INPUT);
  WiFi.begin(esid.c_str(), epass.c_str());
  if (checkWifi_connection(uqid, username, password)) {
    Serial.println("Succesfully Connected!!!");
    esp.begin(url, username, password, uqid);
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
  // if (btSerial.available > 0){
  //   Serial.println(btSerial.read());
  //   Serial.println("reading form bluetooth");
  // }



  if (false) { /*((WiFi.status() == WL_CONNECTED))*/

    pin_D0 = String(digitalRead(D0));
    pin_D1 = String(digitalRead(D1));
    pin_D2 = String(digitalRead(D2));
    pin_D3 = String(digitalRead(D3));
    pin_D4 = String(digitalRead(D4));

    pin_A0 = String(analogRead(A0));

    while (Serial.available() == 0);

    int a = Serial.parseInt();

    if (a == 1) {
      payload, code = esp.GETData(ENDPOINT_GET);
      Serial.println(payload);


    } else if (a == 2) {
      payload, code = esp.POSTData(ENDPOINT_POST, pin_D0, pin_D1, pin_D2, pin_D3, pin_D4, pin_A0);
    }

  }
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


void manage_bt_data(void) {
  time_send_scanned_network += 1;

  while (btSerial.available() > 0) {
    Serial.println("reading recieved");
    char data = btSerial.read();
    if ((data == '\r') || (data == '\n'))  { // EOL detected
      buff[buffCount++] = 0; // C strings end with a terminating zero.
      Serial.println();
      Serial.println(buff);
      buffCount = 0; // start the next line
      memset(buff, 0, sizeof(buff));
      // buf has the data
    }
    else {
      buff[buffCount++] = data; // packing a byte into the MUMBO JUMBO buffer
      if (buffCount >= buffCharLimit) { // prevent buffer overflow
        buff[buffCount++] = 0; // C strings end with a terminating zero.
        Serial.println();
        Serial.println(buff);
        buffCount = 0; // start the next line
        memset(buff, 0, sizeof(buff));
      }
    }
  }

  if (time_send_scanned_network > 1000) {
    Serial.println("again send scanned networks");
    btSerial.write(cString);
    time_send_scanned_network = 0;
  }

  /*
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
    for (int i = 0; i < qsid.length(); ++i) {
    EEPROM.write(i, qsid[i]);
    Serial.print("Wrote: ");
    Serial.println(qsid[i]);
    }
    Serial.println("writing eeprom pass:");
    for (int i = 0; i < qpass.length(); ++i) {
    EEPROM.write(32 + i, qpass[i]);
    Serial.print("Wrote: ");
    Serial.println(qpass[i]);
    }

    Serial.println("writing eeprom username:");
    for (int i = 0; i < username.length(); ++i) {
    EEPROM.write(96 + i, username[i]);
    Serial.print("Wrote: ");
    Serial.println(username[i]);
    }

    Serial.println("writing eeprom password:");
    for (int i = 0; i < password.length(); ++i) {
    EEPROM.write(128 + i, password[i]);
    Serial.print("Wrote: ");
    Serial.println(password[i]);
    }

    Serial.println("writing eeprom uqid:");
    for (int i = 0; i < uqid.length(); ++i) {
    EEPROM.write(196 + i, uqid[i]);
    Serial.print("Wrote: ");
    Serial.println(uqid[i]);
    }


    EEPROM.commit();
    delay(100);
    ESP.reset();
  */
}








// void func(String payload){
//     const size_t capacity = JSON_ARRAY_SIZE(5) + 10*JSON_OBJECT_SIZE(3) + 490;
//     DynamicJsonBuffer jsonBuffer(capacity);


//     JsonObject& root = jsonBuffer.parseObject(payload);

//     Serial.println("Root");
//     Serial.println(root.size());

//     for (JsonPair& kv : root) {
//     Serial.println(kv.key);
//     Serial.println(kv.value.as<char*>());
//     }

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
// }
