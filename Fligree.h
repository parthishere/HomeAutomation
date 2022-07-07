#ifndef fl
#define f1

#if (ARDUINO >100)
  #include "Arduino.h"
#else
  #include "Wrogram.h"
#endif

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

class Fligree{
 
  public:
      
    Fligree(String URL);

    String GETData(String username, String uqid, String endpoint);
    
    String POSTData(String username, String password, String uqid, String endpoint, String D0, String D1, String D2, String D3, String D4, String A0);

  private:
    String payload, code, url, D0, D1, D2, D3, D4, A0, data_to_send, URI, URL;
    String uqid, username, password;
    int httpCode;

};

#endif
