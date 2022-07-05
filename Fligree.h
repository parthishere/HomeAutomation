#define URL "http://192.168.0.102:8000/"
String unique_id, uname, passwd;
int httpCode;

class Fligree{
 
  public:
      
    Fligree(){
      
      Serial.println("Constructur called");
      
    };
    
    void Begin(String uqid, String username, String password) {
      Serial.println(uqid);
      Serial.println(username);
      Serial.println(password);
      unique_id=uqid;
      uname=username;
      passwd=password;
    };


    String GETData(String endpoint){
   /* endpoint shoud be like api/home */      
      WiFiClient client;
  
      HTTPClient http;
  
      Serial.print("[HTTP] begin...\n");
      // configure traged server and url
      Serial.print("usernmae");
      Serial.println(uname);
      Serial.print("uqid");
      Serial.println(unique_id);
      URI = URL+endpoint+"?uqid="+unique_id+"&username="+uname;
      http.begin(client, URI);

      Serial.println(URI);
      
      /*
        // or
        http.begin(client, "http://jigsaw.w3.org/HTTP/Basic/");
        http.setAuthorization("guest", "guest");

        // or
        http.begin(client, "http://jigsaw.w3.org/HTTP/Basic/");
        http.setAuthorization("Z3Vlc3Q6Z3Vlc3Q=");
      */
  
  
      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      httpCode = http.GET();
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
  
        // file found at server
        if (httpCode == HTTP_CODE_OK) {
          String payload = http.getString();
          code = "200";
          Serial.println(payload);
          http.end();
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        code = "400";
        http.end();
      } 
      return payload, code;
    };



    String POSTData(String endpoint, String D0, String D1, String D2, String D3, String D4, String A0){

        WiFiClient client;
    
        HTTPClient http;
    
        Serial.print("[HTTP] begin...\n");
    
    
        http.begin(client, (URL+endpoint));
        
        Serial.print("[HTTP] POST...\n");
        
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        data_to_send = "username="+uname+"&password="+passwd+"&uqid="+unique_id+"&D0="+ D0 +"&D1="+ D1 +"&D2="+ D2 +"&D3="+ D3 +"&D4="+ D4 +"&A0="+A0;
        Serial.println(data_to_send);
      
        int httpCode = http.POST(data_to_send);
        
        // httpCode will be negative on error
        if (httpCode > 0) {
          
          Serial.printf("[HTTP] POST... code: %d\n", httpCode);
    
 
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

    

  private:
    String payload, code, url, D0, D1, D2, D3, D4, A0, data_to_send, URI;
};
