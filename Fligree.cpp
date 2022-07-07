#include "Fligree.h"


Fligree::Fligree(String URL){
  Serial.println("Constructor Called");
  URL = URL;
}


String Fligree::GETData(String username, String uqid, String endpoint){
     
  WiFiClient client;

  HTTPClient http;

  Serial.print("[HTTP] begin...\n");

  URI = URL+endpoint+"?uqid="+uqid+"&username="+username  ;
 
  http.begin(client, URI);

  Serial.println(URI);
  
  Serial.print("[HTTP] GET...\n, "+ URI);

  httpCode = http.GET();
  if (httpCode > 0) {

    Serial.printf("[HTTP] GET... code: %d\n", httpCode);

    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      code = "200";
      Serial.println(payload);
      http.end();
    }
  } else {
    Serial.print("[HTTP] GET... failed, error: "+String(http.errorToString(httpCode))+"\n,to URI: %s"+ URI);
    code = "400";
    http.end();
  } 
  return payload, code;
};



String Fligree::POSTData(String username, String password, String uqid, String endpoint, String D0, String D1, String D2, String D3, String D4, String A0){

  WiFiClient client;

  HTTPClient http;

  Serial.print("[HTTP] begin...\n");

  URI = URL+endpoint+"?uqid="+uqid+"&username="+username;

  http.begin(client, URI);

  Serial.print("[HTTP] POST...\n, "+ URI);
  
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

//  String httpRequestData = "&password="+ password+"&D0="+ D0 +"&D1="+ D1 +"&D2="+ D2 +"&D3="+ D3 +"&D4="+ D4 +"&A0="+ A0;
  String httpRequestData = "password=123&D0=0&D1=1&D2=2&D3=3&D4=4&A0=255";
  Serial.println(httpRequestData);

  int httpCode = http.POST(httpRequestData);

  if (httpCode > 0) {
    
    Serial.print("[HTTP] POST... code: "+String(http.errorToString(httpCode))+"\n, to URI: "+URI+"\n with Data: %s"+httpRequestData);

    if (httpCode == HTTP_CODE_OK) {
      payload = http.getString();
      Serial.println(payload);
      code = "200";
      http.end();
    }
  } else {
    Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    code = "400";
    http.end();
  } 
  
  return payload, code;
};
