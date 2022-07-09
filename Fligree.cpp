#include "Fligree.h"
#include <string>
#include <cctype>

Fligree::Fligree(void){
  Serial.println("Constructor Called");
}

void Fligree::begin(String url, String uname, String passwd, String uid){
  URL = String(url.c_str());
  username = String(uname.c_str());
  password = String(passwd.c_str());
  uqid = uid;

}



String Fligree::GETData(String endpoint){

  WiFiClient client;

  HTTPClient http;

  URI = URL+endpoint+"?uqid="+uqid+"&username="+username;
 
  http.begin(client, URI);

  Serial.println("GET, "+URI);

  httpCode = http.GET();
  if (httpCode > 0) {


    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      code = "200";
      Serial.print("GET DATA: ");
      Serial.println(payload);
      http.end();
    }
  } else {
    Serial.print("[HTTP] GET... failed, error: "+String(http.errorToString(httpCode))+"\n,to URI: %s"+ URI);
    code = "400";
    http.end();
  } 
  Serial.println("");
  return payload, code;
};



String Fligree::POSTData(String endpoint, String D0, String D1, String D2, String D3, String D4, String A0){

  
  WiFiClient client;

  HTTPClient http;


  URI = URL+endpoint+"?uqid="+uqid+"&username="+username+"\0";

  http.begin(client, URI);


  Serial.println("POST, "+URI);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  httpRequestData += "&D0="+ D0 +"&D1="+ D1 +"&D2="+ D2 +"&D3="+ D3 +"&D4="+ D4 +"&A0="+ A0 +"&password="+password+"&username="+username+"&uqid="+String(uqid.c_str())+"\0";
  
  int httpCode = http.POST(httpRequestData);

  if (httpCode > 0) {

    if (httpCode == HTTP_CODE_OK) {
      payload = http.getString();
      code = "200";
      Serial.print("POST DATA: ");
      Serial.println(payload);
      http.end();
    }
  } else {
    Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str(),", to URI: "+URI+"\nwith Data: %s"+httpRequestData);
    code = "400";
    http.end();
  } 
  Serial.println("");
  return payload, code;
};
