/**
   BasicHTTPSClient.ino

    Created on: 20.08.2018

*/

#include <Arduino.h>
#include <Arduino_JSON.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClientSecureBearSSL.h>
// Fingerprint for demo URL, expires on June 2, 2021, needs to be updated well before this date


//1D 72 D0 EE 02 94 45 F9 B2 BF 26 ED 08 F0 C9 7B 9C D4 CB 01
const uint8_t fingerprint[20] = {0x5A, 0x12, 0xCA, 0xB5, 0x35, 0x69, 0x04, 0x81, 0xE6, 0x1F, 0x8A, 0x3D, 0xBA, 0xF1, 0x87, 0x1A, 0x24, 0xA5, 0x40, 0x64};                                 
//const uint8_t fingerprint[20] = {0x1D, 0x72, 0xD0, 0xEE, 0x02, 0x94, 0x45, 0xF9, 0xB2, 0xBF, 0x26, 0xED, 0x08, 0xF0, 0xC9, 0x7B, 0x9C, 0xD4, 0xCB, 0x01};

//VARIABLES
int buttonState = 0;
int button2State = 0;
int button3State = 0;
int button4State = 0;
int mode = 0; //seda muudan pärast veebist režiimi vahetades...
int cyclelength=2000; //Seda muudan pärast veebist aega muutes...
int ongoingCycle = 0;
int ongoingDelay = 0;  // nulli jooksev rohelise laine delay 
int delaylength = 0; //seadistatud rohelise laine delay pikkus
int greenWave = 0;
String payloadin; 
int myState;
int foor2State;
int previousfoor2State = 99;
int ongoingfoor2State;
int changed= 1;
int globalButtonState = 0;
String teade;


                   
ESP8266WiFiMulti WiFiMulti;

void setup() {
  pinMode(D0, INPUT); //nupp
  //foor1 autod
  pinMode(D1, OUTPUT);//punane
  pinMode(D2, OUTPUT);//kollane
  pinMode(D3, OUTPUT);//roheline
  //foor2 jalakäija
  pinMode(D5, OUTPUT);//punane
  pinMode(D6, OUTPUT);//roheline
 
  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
 //WiFiMulti.addAP("SSID", "PASSWORD");
 WiFiMulti.addAP("TLU", "");
 

      

}

void loop()
{
  buttonState = digitalRead(D0);
   
  //Serial.println("LOOP ALGUS");
  //Serial.println("buttonState");
  //Serial.println(buttonState);
  

  //Serial.println("D6");
  //Serial.println(digitalRead(D6));
  //get DATA
  
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    
        std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    
        client->setFingerprint(fingerprint);
        // Or, if you happy to ignore the SSL certificate, then use the following line instead:
        // client->setInsecure();
        
        HTTPClient https;
    
        Serial.print("[HTTPS] begin...\n");
        if (https.begin(*client, "https://kenpikasjad-default-rtdb.europe-west1.firebasedatabase.app/foor.json")) {  // HTTPS
    
          https.addHeader("Content-Type", "application/json");
          Serial.print("[HTTPS] GET...\n");
          // start connection and send HTTPS header
    
          int httpCode = https.GET();
    
          // httpCode will be negative on error
         if (httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
    
            // file found at server
            if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
              payloadin = https.getString();
              //Serial.println(payloadin);
             
              
              JSONVar myObject = JSON.parse(payloadin);

              // JSON.typeof(jsonVar) can be used to get the type of the var
              if (JSON.typeof(myObject) == "undefined") {
                Serial.println("Parsing input failed!");
                return;
              }
              //Serial.print("JSON object = ");
              //Serial.println(myObject);
              
              // myObject.keys() can be used to get an array of all the keys in the object
              JSONVar keys = myObject.keys();
              

              cyclelength = int(myObject["duration"]);
              mode = int(myObject["mode"]);
              
            }
          } else {
            Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
            mode=0;
          }
    
          https.end();
        } else {
          Serial.printf("[HTTPS] Unable to connect\n");
          mode = 0;
        }
      } 
      
//ROHELISE LAINE SEADISTUS SISSE
if ((WiFiMulti.run() == WL_CONNECTED)) {
        Serial.println("Küsin rohelise laine seadistust sisse");
        std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    
        client->setFingerprint(fingerprint);
        // Or, if you happy to ignore the SSL certificate, then use the following line instead:
        // client->setInsecure();
        
        HTTPClient https;
    
        Serial.print("[HTTPS] begin...\n");
        if (https.begin(*client, "https://kenpikasjad-default-rtdb.europe-west1.firebasedatabase.app/wave.json")) {  // HTTPS
    
          https.addHeader("Content-Type", "application/json");
          Serial.print("[HTTPS] GET...\n");
          // start connection and send HTTPS header
    
          int httpCode = https.GET();
    
          // httpCode will be negative on error
         if (httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
    
            // file found at server
            if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
              payloadin = https.getString();
              //Serial.println(payloadin);
             
              
              JSONVar myObject = JSON.parse(payloadin);

              // JSON.typeof(jsonVar) can be used to get the type of the var
              if (JSON.typeof(myObject) == "undefined") {
                Serial.println("Parsing input failed!");
                return;
              }
              //Serial.print("JSON object = ");
              //Serial.println(myObject);
              
              // myObject.keys() can be used to get an array of all the keys in the object
              JSONVar keys = myObject.keys();
              
//              for (int i = 0; i < keys.length(); i++) {
//                JSONVar value = myObject[keys[i]];
//                Serial.print(keys[i]);
//                Serial.print(" = ");
//                Serial.println(value);
//              }
              delaylength = int(myObject["delay"]);
              greenWave = int(myObject["state"]);
              //button1State = int(myObject["button1state"]);
              button2State = int(myObject["button2state"]);
              button3State = int(myObject["button3state"]);
              button4State = int(myObject["button4state"]);
              Serial.println("Sain rohelise laine seadistuse kätte! ");
              Serial.println("delaylength: ");
              Serial.println(delaylength);
              Serial.println("greenWave: ");
              Serial.println(greenWave);
              Serial.println("button1State: ");
              Serial.println(buttonState);
              Serial.println("button2State: ");
              Serial.println(button2State);
              Serial.println("button3State: ");
              Serial.println(button3State);
              Serial.println("button4State: ");
              Serial.println(button4State);
              
              
            }
          } else {
            Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
            greenWave=0;
          }
    
          https.end();
        } else {
          Serial.printf("[HTTPS] Unable to connect\n");
          greenWave = 0;
        }
      } 
    


  //kui greenwave on sisse lülitatud
  if(greenWave==1){
    //FOOR1 seega
    //kui delay väiksem kui null on foor 2 juhi rollis
      if(delaylength >= 0){
        //foor 1 alustab ise käitub nagu ajastatud foor ja saadab oma state serveri mida foor 2 loeb
        previousfoor2State = 99;
        //tavaline ajastatud režiim
        //Kui tsükli aeg pole 0
        Serial.println("Greenwave algus");
        Serial.println("ongoingcycle:");
        Serial.println(ongoingCycle);
        if(ongoingCycle > 0){
          Serial.println("kui ongoingCycle suurem kui null vähendan tsüklit 100 võrra");
          ongoingCycle-=100;
          Serial.println("ongoingcycle:");
          Serial.println(ongoingCycle);
          
        }
        //kui tsükli aeg on 0
        else if(ongoingCycle == 0){
          //Serial.println("kui ongoingCycle =  null");
          if(digitalRead(D3)== 1 && digitalRead(D5)== 1){ //kui autole põleb roheline ja jalak punane
            
            //Serial.println("kui autole põleb roheline ja jalak punane");
            //SAADAN SERVERI ET Jalakäijale roheliseks
              teade = "{\"state\":\ 0 }";
            // wait for WiFi connection
            if ((WiFiMulti.run() == WL_CONNECTED)) {
          
              std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
          
              client->setFingerprint(fingerprint);
              // Or, if you happy to ignore the SSL certificate, then use the following line instead:
              // client->setInsecure();
              
              HTTPClient https;
          
              Serial.print("[HTTPS] begin...\n");
              if (https.begin(*client, "https://kenpikasjad-default-rtdb.europe-west1.firebasedatabase.app/foor.json")) {  // HTTPS
          
                https.addHeader("Content-Type", "application/json");
    
                Serial.print("[HTTPS] PATCH...\n");
                // start connection and send HTTP header
                //int httpCode = https.POST(teade);
          
                int httpCode = https.PATCH(teade);
          
                // httpCode will be negative on error
                if (httpCode > 0) {
                  // HTTP header has been send and Server response header has been handled
                  Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
          
                  // file found at server
                  if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                    String payload = https.getString();
                    Serial.println(payload);

             
                  }
                } else {
                  Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
                }
          
                https.end();
              } else {
                Serial.printf("[HTTPS] Unable to connect\n");
              }
              
            }
              
              
              //muudan foorid jalakäijale roheliseks
              carToPed();
              //taaskäivitan tsükli
              ongoingCycle= cyclelength;
          }
          else if(digitalRead(D1)== 1 && digitalRead(D6)== 1){ //kui autole põleb punane ja jalak roheline
              //Serial.println("kui autole põleb punane ja jalak roheline");
              //muudan foori autole roheliseks

              //SAADAN SERVERI ET autole läheb roheliseks
              teade = "{\"state\":\ 1 }";
            // wait for WiFi connection
            if ((WiFiMulti.run() == WL_CONNECTED)) {
          
              std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
          
              client->setFingerprint(fingerprint);
              // Or, if you happy to ignore the SSL certificate, then use the following line instead:
              // client->setInsecure();
              
              HTTPClient https;
          
              Serial.print("[HTTPS] begin...\n");
              if (https.begin(*client, "https://kenpikasjad-default-rtdb.europe-west1.firebasedatabase.app/foor.json")) {  // HTTPS
          
                https.addHeader("Content-Type", "application/json");
    
                Serial.print("[HTTPS] PATCH...\n");
                // start connection and send HTTP header
          
                int httpCode = https.PATCH(teade);
          
                // httpCode will be negative on error
                if (httpCode > 0) {
                  // HTTP header has been send and Server response header has been handled
                  Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
          
                  // file found at server
                  if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                    String payload = https.getString();
                    Serial.println(payload);
                  
                  }
                } else {
                  Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
                }
          
                https.end();
              } else {
                Serial.printf("[HTTPS] Unable to connect\n");
              }
              
            }
         
              pedToCar();
              //taaskäivitan tsükli
              ongoingCycle= cyclelength;
          }
          else if(digitalRead(D1)== 0 && digitalRead(D6)== 0 && digitalRead(D3)== 0 && digitalRead(D5)== 0){
            //Serial.println("Kui foor ei põle");

            //SAADAN SERVERI ET autole läheb roheliseks
            teade = "{\"state\":\ 1 }";
            // wait for WiFi connection
            if ((WiFiMulti.run() == WL_CONNECTED)) {
          
              std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
          
              client->setFingerprint(fingerprint);
              // Or, if you happy to ignore the SSL certificate, then use the following line instead:
              // client->setInsecure();
              
              HTTPClient https;
          
              Serial.print("[HTTPS] begin...\n");
              if (https.begin(*client, "https://kenpikasjad-default-rtdb.europe-west1.firebasedatabase.app/foor.json")) {  // HTTPS
          
                https.addHeader("Content-Type", "application/json");
    
                Serial.print("[HTTPS] PATCH...\n");
                // start connection and send HTTP header
                //int httpCode = https.POST(teade);
          
                int httpCode = https.PATCH(teade);
          
                // httpCode will be negative on error
                if (httpCode > 0) {
                  // HTTP header has been send and Server response header has been handled
                  Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
          
                  // file found at server
                  if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                    String payload = https.getString();
                    Serial.println(payload);
              
                  }
                } else {
                  Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
                }
          
                https.end();
              } else {
                Serial.printf("[HTTPS] Unable to connect\n");
              }
              
            }
              
            pedToCar();
            ongoingCycle= cyclelength;
            
          }
        }  
      }
      //kui delay on positiivne siis foor2 kontrollib foor1 state
      else if(delaylength < 0){
        Serial.println("küsin foor2 staatust!");
        if ((WiFiMulti.run() == WL_CONNECTED)) {
    
          std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
      
          client->setFingerprint(fingerprint);
          // Or, if you happy to ignore the SSL certificate, then use the following line instead:
          // client->setInsecure();
          
          HTTPClient https;
      
          Serial.print("[HTTPS] begin...\n");
          if (https.begin(*client, "https://kenpikasjad-default-rtdb.europe-west1.firebasedatabase.app/foor2.json")) {  // HTTPS
      
            https.addHeader("Content-Type", "application/json");
            Serial.print("[HTTPS] GET...\n");
            // start connection and send HTTPS header
      
            int httpCode = https.GET();
      
            // httpCode will be negative on error
           if (httpCode > 0) {
              // HTTP header has been send and Server response header has been handled
              Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
      
              // file found at server
              if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                payloadin = https.getString();
                //Serial.println(payloadin);
               
                
                JSONVar myObject = JSON.parse(payloadin);
  
                // JSON.typeof(jsonVar) can be used to get the type of the var
                if (JSON.typeof(myObject) == "undefined") {
                  Serial.println("Parsing input failed!");
                  return;
                }
                
                // myObject.keys() can be used to get an array of all the keys in the object
                JSONVar keys = myObject.keys();
                
                foor2State = int(myObject["state"]);
                
                Serial.println("foor2 andmed käes! ");
                Serial.println("foor2 state: ");
                Serial.println(foor2State);

                Serial.println("previous: ");
                Serial.println(previousfoor2State);
                
                
         
                
                
                
                
              }
            } else {
              Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
              mode=0;
            }
      
            https.end();
          } else {
            Serial.printf("[HTTPS] Unable to connect\n");
            mode = 0;
          }
        } 

        // andmed foor 1 state on muutunud
        //salvestan käsil oleva foor1 staatuse mille järgi muudan peale delayd foori värve,
        //panen käima ongoing delay
        //ja sätin foori muutmise lubatuks.
        if(foor2State != previousfoor2State){
          Serial.println("Kui foo1 state on muutunud");
          ongoingfoor2State= foor2State; 
          Serial.println("ongoigfoor2State: ");
          Serial.println(ongoingfoor2State);
          previousfoor2State=foor2State;
          changed = 0; //
          //kui delay on negatiivne
          if(delaylength < 0){
            //muudan positiivseks
            ongoingDelay = delaylength * -1;
              
          }
          else{
            //delaylength positiivne ja muutma ei pea
            ongoingDelay = delaylength;
          }
        }
           
           
          
        }

        //Kui delay aeg pole 0
        
        Serial.println("ongoingdelay:");
        Serial.println(ongoingDelay);
        Serial.println("changed:");
        Serial.println(changed);
        Serial.println("ongoingfoorstate:");
        Serial.println(ongoingfoor2State);
        if(ongoingDelay > 0){
          Serial.println("kui ongoingDelay suurem kui null vähendan tsüklit 100 võrra");
          ongoingDelay-=100;
          Serial.println("ongoingDelay:");
          Serial.println(ongoingDelay);
          
        }
        //kui delay aeg on 0
        else if(changed == 0){
          
          if(ongoingfoor2State == 0){
            Serial.println("Changing from car to ped...");
            carToPed();
            changed = 1;
            
            
          }
          else if (ongoingfoor2State== 1){
            Serial.println("Changing from ped to car:");
            pedToCar();
            changed = 1;
            
          }
        

        
        
      }

      


    
    
  }

  //KUI ON GREENWAVE JALAKÄIJA NUPUGA
  else if(greenWave==2){
    //nupu state andmebaasi
    if(buttonState == HIGH){
          teade = "{\"button1state\":\ 1 }";
          // wait for WiFi connection
          if ((WiFiMulti.run() == WL_CONNECTED)) {
        
            std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
        
            client->setFingerprint(fingerprint);
            // Or, if you happy to ignore the SSL certificate, then use the following line instead:
            // client->setInsecure();
            
            HTTPClient https;
        
            Serial.print("[HTTPS] begin...\n");
            if (https.begin(*client, "https://kenpikasjad-default-rtdb.europe-west1.firebasedatabase.app/wave.json")) {  // HTTPS
        
              https.addHeader("Content-Type", "application/json");
      
              Serial.print("[HTTPS] PATCH...\n");
              // start connection and send HTTP header
              //int httpCode = https.POST(teade);
        
              int httpCode = https.PATCH(teade);
        
              // httpCode will be negative on error
              if (httpCode > 0) {
                // HTTP header has been send and Server response header has been handled
                Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
        
                // file found at server
                if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                  String payload = https.getString();
                  Serial.println(payload);
            
                }
              } else {
                Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
              }
        
              https.end();
            } else {
              Serial.printf("[HTTPS] Unable to connect\n");
            }
          }  
         }

        
        
    
  
        //NUPU LÕPP







    

     //kui delay suurem kui null on foor 1 juhi rollis
      if(delaylength >= 0){
        //kui mõni nupp on sisse vajutatud...
        if(buttonState==1 || button2State==1 || button3State==1 || button4State==1){
          if((digitalRead(D3)== 1 && digitalRead(D5)== 1)|| (digitalRead(D1)== 0 && digitalRead(D6)== 0 && digitalRead(D3)== 0 && digitalRead(D5)== 0)){ //kui autole põleb roheline ja jalak punane või foor ei põle!            
            //Serial.println("kui autole põleb roheline ja jalak punane");
            //SAADAN SERVERI ET Jalakäijale roheliseks
              teade = "{\"state\":\ 0 }";
            // wait for WiFi connection
            if ((WiFiMulti.run() == WL_CONNECTED)) {
          
              std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
          
              client->setFingerprint(fingerprint);
              // Or, if you happy to ignore the SSL certificate, then use the following line instead:
              // client->setInsecure();
              
              HTTPClient https;
          
              Serial.print("[HTTPS] begin...\n");
              if (https.begin(*client, "https://kenpikasjad-default-rtdb.europe-west1.firebasedatabase.app/foor.json")) {  // HTTPS
          
                https.addHeader("Content-Type", "application/json");
    
                Serial.print("[HTTPS] PATCH...\n");
                // start connection and send HTTP header
                //int httpCode = https.POST(teade);
          
                int httpCode = https.PATCH(teade);
          
                // httpCode will be negative on error
                if (httpCode > 0) {
                  // HTTP header has been send and Server response header has been handled
                  Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
          
                  // file found at server
                  if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                    String payload = https.getString();
                    Serial.println(payload);

             
                  }
                } else {
                  Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
                }
          
                https.end();
              } else {
                Serial.printf("[HTTPS] Unable to connect\n");
              }
              
            }
             //Seadistan serveris nuppude states 0-ks
              //teade = "{\"button1state\":\ 0,\"button2state\": \0,\"button3state\": \0,\"button4state\": \0}";
              teade = "{\"button1state\":0,\"button2state\":0,\"button3state\":0,\"button4state\":0}";
              button2State = 0;
              button3State = 0;
              button4State = 0;
             
            // wait for WiFi connection
            if ((WiFiMulti.run() == WL_CONNECTED)) {
          
              std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
          
              client->setFingerprint(fingerprint);
              // Or, if you happy to ignore the SSL certificate, then use the following line instead:
              // client->setInsecure();
              
              HTTPClient https;
          
              Serial.print("[HTTPS] begin...\n");
              if (https.begin(*client, "https://kenpikasjad-default-rtdb.europe-west1.firebasedatabase.app/wave.json")) {  // HTTPS
          
                https.addHeader("Content-Type", "application/json");
    
                Serial.print("[HTTPS] PATCH...\n");
                // start connection and send HTTP header
                //int httpCode = https.POST(teade);
          
                int httpCode = https.PATCH(teade);
          
                // httpCode will be negative on error
                if (httpCode > 0) {
                  // HTTP header has been send and Server response header has been handled
                  Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
          
                  // file found at server
                  if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                    String payload = https.getString();
                    Serial.println(payload);

             
                  }
                } else {
                  Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
                }
          
                https.end();
              } else {
                Serial.printf("[HTTPS] Unable to connect\n");
              }
              
            }




              
              
              //muudan foorid jalakäijale roheliseks
              carToPed();
              //taaskäivitan tsükli
              ongoingCycle= cyclelength;
          }
        }
      //kui pole nupule vajutatud
      else{  
        //foor 1 alustab ise käitub nagu ajastatud foor ja saadab oma state serveri mida foor 2 loeb
        previousfoor2State = 99;
        //tavaline ajastatud režiim
        //Kui tsükli aeg pole 0
        Serial.println("Greenwave algus");
        Serial.println("ongoingcycle:");
        Serial.println(ongoingCycle);
        if(ongoingCycle > 0){
          Serial.println("kui ongoingCycle suurem kui null vähendan tsüklit 100 võrra");
          ongoingCycle-=100;
          Serial.println("ongoingcycle:");
          Serial.println(ongoingCycle);
          
        }
        //kui tsükli aeg on 0
        else if(ongoingCycle == 0){
          //Serial.println("kui ongoingCycle =  null");
          if(digitalRead(D3)== 1 && digitalRead(D5)== 1){ //kui autole põleb roheline ja jalak punane
            
            //Serial.println("kui autole põleb roheline ja jalak punane");
            //SAADAN SERVERI ET Jalakäijale roheliseks
              teade = "{\"state\":\ 0 }";
            // wait for WiFi connection
            if ((WiFiMulti.run() == WL_CONNECTED)) {
          
              std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
          
              client->setFingerprint(fingerprint);
              // Or, if you happy to ignore the SSL certificate, then use the following line instead:
              // client->setInsecure();
              
              HTTPClient https;
          
              Serial.print("[HTTPS] begin...\n");
              if (https.begin(*client, "https://kenpikasjad-default-rtdb.europe-west1.firebasedatabase.app/foor.json")) {  // HTTPS
          
                https.addHeader("Content-Type", "application/json");
    
                Serial.print("[HTTPS] PATCH...\n");
                // start connection and send HTTP header
                //int httpCode = https.POST(teade);
          
                int httpCode = https.PATCH(teade);
          
                // httpCode will be negative on error
                if (httpCode > 0) {
                  // HTTP header has been send and Server response header has been handled
                  Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
          
                  // file found at server
                  if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                    String payload = https.getString();
                    Serial.println(payload);

             
                  }
                } else {
                  Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
                }
          
                https.end();
              } else {
                Serial.printf("[HTTPS] Unable to connect\n");
              }
              
            }
              
              
              //muudan foorid jalakäijale roheliseks
              carToPed();
              //taaskäivitan tsükli
              ongoingCycle= cyclelength;
          }
          else if(digitalRead(D1)== 1 && digitalRead(D6)== 1){ //kui autole põleb punane ja jalak roheline
              //Serial.println("kui autole põleb punane ja jalak roheline");
              //muudan foori autole roheliseks

              //SAADAN SERVERI ET autole läheb roheliseks
              teade = "{\"state\":\ 1 }";
            // wait for WiFi connection
            if ((WiFiMulti.run() == WL_CONNECTED)) {
          
              std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
          
              client->setFingerprint(fingerprint);
              // Or, if you happy to ignore the SSL certificate, then use the following line instead:
              // client->setInsecure();
              
              HTTPClient https;
          
              Serial.print("[HTTPS] begin...\n");
              if (https.begin(*client, "https://kenpikasjad-default-rtdb.europe-west1.firebasedatabase.app/foor.json")) {  // HTTPS
          
                https.addHeader("Content-Type", "application/json");
    
                Serial.print("[HTTPS] PATCH...\n");
                // start connection and send HTTP header
          
                int httpCode = https.PATCH(teade);
          
                // httpCode will be negative on error
                if (httpCode > 0) {
                  // HTTP header has been send and Server response header has been handled
                  Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
          
                  // file found at server
                  if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                    String payload = https.getString();
                    Serial.println(payload);
                  
                  }
                } else {
                  Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
                }
          
                https.end();
              } else {
                Serial.printf("[HTTPS] Unable to connect\n");
              }
              
            }
         
              pedToCar();
              //taaskäivitan tsükli
              ongoingCycle= cyclelength;
          }
          else if(digitalRead(D1)== 0 && digitalRead(D6)== 0 && digitalRead(D3)== 0 && digitalRead(D5)== 0){
            //Serial.println("Kui foor ei põle");

            //SAADAN SERVERI ET autole läheb roheliseks
            teade = "{\"state\":\ 1 }";
            // wait for WiFi connection
            if ((WiFiMulti.run() == WL_CONNECTED)) {
          
              std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
          
              client->setFingerprint(fingerprint);
              // Or, if you happy to ignore the SSL certificate, then use the following line instead:
              // client->setInsecure();
              
              HTTPClient https;
          
              Serial.print("[HTTPS] begin...\n");
              if (https.begin(*client, "https://kenpikasjad-default-rtdb.europe-west1.firebasedatabase.app/foor.json")) {  // HTTPS
          
                https.addHeader("Content-Type", "application/json");
    
                Serial.print("[HTTPS] PATCH...\n");
                // start connection and send HTTP header
                //int httpCode = https.POST(teade);
          
                int httpCode = https.PATCH(teade);
          
                // httpCode will be negative on error
                if (httpCode > 0) {
                  // HTTP header has been send and Server response header has been handled
                  Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
          
                  // file found at server
                  if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                    String payload = https.getString();
                    Serial.println(payload);
              
                  }
                } else {
                  Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
                }
          
                https.end();
              } else {
                Serial.printf("[HTTPS] Unable to connect\n");
              }
              
            }
              
            pedToCar();
            ongoingCycle= cyclelength;
            
          }
        }  
      }
      }
  
      

    
     //kui delay on positiivne siis foor2 kontrollib foor1 state
      else if(delaylength < 0){
        Serial.println("küsin foor2 staatust!");
        if ((WiFiMulti.run() == WL_CONNECTED)) {
    
          std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
      
          client->setFingerprint(fingerprint);
          // Or, if you happy to ignore the SSL certificate, then use the following line instead:
          // client->setInsecure();
          
          HTTPClient https;
      
          Serial.print("[HTTPS] begin...\n");
          if (https.begin(*client, "https://kenpikasjad-default-rtdb.europe-west1.firebasedatabase.app/foor2.json")) {  // HTTPS
      
            https.addHeader("Content-Type", "application/json");
            Serial.print("[HTTPS] GET...\n");
            // start connection and send HTTPS header
      
            int httpCode = https.GET();
      
            // httpCode will be negative on error
           if (httpCode > 0) {
              // HTTP header has been send and Server response header has been handled
              Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
      
              // file found at server
              if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                payloadin = https.getString();
                //Serial.println(payloadin);
               
                
                JSONVar myObject = JSON.parse(payloadin);
  
                // JSON.typeof(jsonVar) can be used to get the type of the var
                if (JSON.typeof(myObject) == "undefined") {
                  Serial.println("Parsing input failed!");
                  return;
                }
                
                // myObject.keys() can be used to get an array of all the keys in the object
                JSONVar keys = myObject.keys();
                
                foor2State = int(myObject["state"]);
                
                Serial.println("foor2 andmed käes! ");
                Serial.println("foor2 state: ");
                Serial.println(foor2State);

                Serial.println("previous: ");
                Serial.println(previousfoor2State);
                
                
         
                
                
                
                
              }
            } else {
              Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
              mode=0;
            }
      
            https.end();
          } else {
            Serial.printf("[HTTPS] Unable to connect\n");
            mode = 0;
          }
        } 

        // andmed foor 1 state on muutunud
        //salvestan käsil oleva foor1 staatuse mille järgi muudan peale delayd foori värve,
        //panen käima ongoing delay
        //ja sätin foori muutmise lubatuks.
        if(foor2State != previousfoor2State){
          Serial.println("Kui foo1 state on muutunud");
          ongoingfoor2State= foor2State; 
          Serial.println("ongoigfoor2State: ");
          Serial.println(ongoingfoor2State);
          previousfoor2State=foor2State;
          changed = 0; //
          //kui delay on negatiivne
          if(delaylength < 0){
            //muudan positiivseks
            ongoingDelay = delaylength * -1;
              
          }
          else{
            //delaylength positiivne ja muutma ei pea
            ongoingDelay = delaylength;
          }
        }
           
           
          
        }

        //Kui delay aeg pole 0
        
        Serial.println("ongoingdelay:");
        Serial.println(ongoingDelay);
        Serial.println("changed:");
        Serial.println(changed);
        Serial.println("ongoingfoorstate:");
        Serial.println(ongoingfoor2State);
        if(ongoingDelay > 0){
          Serial.println("kui ongoingDelay suurem kui null vähendan tsüklit 100 võrra");
          ongoingDelay-=100;
          Serial.println("ongoingDelay:");
          Serial.println(ongoingDelay);
          
        }
        //kui delay aeg on 0
        else if(changed == 0){
          
          if(ongoingfoor2State == 0){
            Serial.println("Changing from car to ped...");
            carToPed();
            changed = 1;
            
            
          }
          else if (ongoingfoor2State== 1){
            Serial.println("Changing from ped to car:");
            pedToCar();
            changed = 1;
            
          }
        

        
        
      }
    
  }
   
  //greenwave nupuga lõpp

  
  


  
  //kui greenwave pole sisse lülitatud teeb foor kõike nii nagu algselt oma seatud mode-i järgi.
  else{
    if(mode == 0){
      //tavaline ajastatud režiim
      //Kui tsükli aeg pole 0
      Serial.println("valitud mode 0");
      Serial.println("ongoingcycle:");
      Serial.println(ongoingCycle);
      if(ongoingCycle > 0){
          //Serial.println("kui ongoingCycle suurem kui null vähendan tsüklit 100 võrra");
        ongoingCycle-=100;
          //Serial.println("ongoingcycle:");
        //Serial.println(ongoingCycle);
        
      }
      //kui tsükli aeg on 0
      else if(ongoingCycle == 0){
        
        //Serial.println("kui ongoingCycle =  null");
        if(digitalRead(D3)== 1 && digitalRead(D5)== 1){ //kui autole põleb roheline ja jalak punane
            //Serial.println("kui autole põleb roheline ja jalak punane");
            //muudan foorid jalakäijale
            carToPed();
            //taaskäivitan tsükli
            ongoingCycle= cyclelength;
        }
        else if(digitalRead(D1)== 1 && digitalRead(D6)== 1){ //kui autole põleb punane ja jalak roheline
            //Serial.println("kui autole põleb punane ja jalak roheline");
            //muudan foori autole
            pedToCar();
            //taaskäivitan tsükli
            ongoingCycle= cyclelength;
        }
        else if(digitalRead(D1)== 0 && digitalRead(D6)== 0 && digitalRead(D3)== 0 && digitalRead(D5)== 0){
          //Serial.println("Kui foor ei põle");
          pedToCar();
          ongoingCycle= cyclelength;
          
        }
      }      
    }
    else if(mode == 1){
      //jalakäija nupuga režiim
      Serial.println("valitud mode 1");
      Serial.println("ongoingcycle:");
      Serial.println(ongoingCycle);
      if(ongoingCycle > 0){
        ongoingCycle-=100;
        
      }
      //kui tsükli aeg on 0
      else if(ongoingCycle == 0){
        //Kui nupp vajutatud ja autole roheline ning jalakäijale punane
        if(buttonState == HIGH && digitalRead(D3)== 1 && digitalRead(D5)== 1 ){
          carToPed();
          //taaskäivitan tsükli
          ongoingCycle= cyclelength;
  
        }
        //Kui nupp pole vajutatud ja autole punane ning jalakäijale roheline
        else if(digitalRead(D1)== 1 && digitalRead(D6)== 1){
          //Serial.println("kui autole punane ning jalakäijale roheline");
          pedToCar();
        }
        //kui fooris rohelised ega punased ei põle
        else if(digitalRead(D1)== 0 && digitalRead(D6)== 0 && digitalRead(D3)== 0 && digitalRead(D5)== 0){
          //Serial.println("Kui foor ei põle");
          
          
          digitalWrite(D1, LOW); //auto Punane
        digitalWrite(D2, LOW); // auto Kollane
          digitalWrite(D3, HIGH); // auto Roheline
  
          digitalWrite(D5, HIGH); // jalak Punane
          digitalWrite(D6, LOW);  // jalak Roheline 
          
          
        }
        //kui nupp pole vajutatud ja autole roheline siis ei tee midagi...
      } 
    }
    else if(mode == 2){
      //off mode- auto kollane vilgub
      Serial.println("Valitud mode 2");
      Serial.println("digitalRead(D1)");
      Serial.println(digitalRead(D1));
      Serial.println("digitalRead(D2)");
      Serial.println(digitalRead(D2));
      Serial.println("digitalRead(D3)");
      Serial.println(digitalRead(D3));
      Serial.println("digitalRead(D5)");
      Serial.println(digitalRead(D5));
      Serial.println("digitalRead(D6)");
      Serial.println(digitalRead(D6));
      
      blinkYellow();
      
    }
  }
   
  
  
  
  
  
delay(100);  
   
 
} 
void carToPed(){ 
  //auto roheline vilgub 5 ajaühikut
  for(int i=0; i<=5;i++){
    //Serial.println("isCarGreenOn?");
    //Serial.println(digitalRead(D3));
    if(digitalRead(D3)== 1){  
      digitalWrite(D1, LOW); // Auto Punane
      digitalWrite(D2, LOW); // Auto Kollane
      digitalWrite(D3, LOW); // Auto Roheline
    }else if(digitalRead(D3)== 0){    
      digitalWrite(D3, HIGH); // Auto Roheline
    }
    delay(500);

  }
  //Auto roheline kustub ja Süttib kollane 5ks ajaühikuks
  digitalWrite(D3, LOW); //Auto Roheline
  for(int i=0; i<=5;i++){
    digitalWrite(D2, HIGH); //Auto Kollane
   
    delay(500);
  }  
  
  //lõpp
  //Autole punane  ja jalakäijale roheline
  digitalWrite(D1, HIGH); //auto Punane
  digitalWrite(D2, LOW); // auto Kollane
  digitalWrite(D3, LOW); // auto Roheline


  digitalWrite(D5, LOW); // jalak Punane
  digitalWrite(D6, HIGH);  // jalak Roheline    
  

}
  
void pedToCar(){
  //jalakäija roheline põleb
  //Autol punane  koos kollasega -> roheline
  //Jalakäijal vilkuv roheline -> punane
  //autol Punane ja kollane koos põlema...
  //ja jalakäija  roheline vilkuma...
  digitalWrite(D1, HIGH); //auto Punane
  digitalWrite(D2, HIGH); // auto Kollane
  
  //ja jalakäija  roheline vilkuma...
  for(int i=0; i<=5;i++){

    if(digitalRead(D6)== 1 ){
      //jalak roheline kustu
      digitalWrite(D5, LOW); // jalak Punane
      digitalWrite(D6, LOW);  // jalak Roheline

    }else if(digitalRead(D6)== 0){
      //jalakäija roheline põlema
      digitalWrite(D5, LOW); // jalak Punane
      digitalWrite(D6, HIGH); //Roheline

    }  
    delay(500);
  }
  //lõppseis
  //Autol roheline
 
  digitalWrite(D1, LOW); //auto Punane
  digitalWrite(D2, LOW); // auto Kollane
  digitalWrite(D3, HIGH); // auto Roheline

  digitalWrite(D5, HIGH); // jalak Punane
  digitalWrite(D6, LOW);  // jalak Roheline

}

void sendColor(String color) {
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    String req = "{\"currentColor\": \"" + color + "\" }";
    Serial.printf("Sending color to foor...:");
    Serial.println(req);
    
          
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    client->setFingerprint(fingerprint);
    // Or, if you happy to ignore the SSL certificate, then use the following line instead:
    // client->setInsecure();
    
    HTTPClient https;

    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, "https://kenpikasjad-default-rtdb.europe-west1.firebasedatabase.app/foor.json")) {  // HTTPS

      https.addHeader("Content-Type", "application/json");

      Serial.print("[HTTPS] PATCH...\n");
      // start connection and send HTTP header
      //int httpCode = https.POST(teade);

      int httpCode = https.PATCH(req);

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] PATCH... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          Serial.println(payload);

   
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
    
  }
}

void blinkYellow(){
  digitalWrite(D1, LOW); //auto Punane
  //digitalWrite(D2, LOW); // auto Kollane
  digitalWrite(D3, LOW); // auto Roheline
  
  digitalWrite(D5, LOW); // jalak Punane
  digitalWrite(D6, LOW);  // jalak Roheline
  if(ongoingCycle > 0){
       Serial.println("kui ongoingCycle suurem kui null vähendan tsüklit 100 võrra");
      ongoingCycle-=100;
        Serial.println("ongoingcycle:");
      Serial.println(ongoingCycle);
      
  }
  //kui tsükli aeg on 0
  else if(ongoingCycle == 0){
    if(digitalRead(D2)== 1){
      
      digitalWrite(D2, LOW); // auto Kollane
      ongoingCycle= 100;
      
    }
    else if(digitalRead(D2)== 0){
      
      digitalWrite(D2, HIGH); // auto Kollane
      ongoingCycle= 100;
    }
  }     
}
