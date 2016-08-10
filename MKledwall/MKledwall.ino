#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>



#include <FastLED.h>



#include <NetworkProtocol.h>


#define UDP_PACKET_SIZE 1012
#define UDP_PORT 17200
#define LEDS_ATTACHED 250
#define ERROR_LED_PIN 2
#define ERROR_LED_WAIT 100
#define LED_PIN_1 4 
#define LED_PIN_2 5

#define FRAME_DELAY 10

#define POLL_DELAY 10



//cxonnect to test wifi AP
const char* ssid = "emfcamp-insecure";
const char* password = "";


LEDWall::NetworkProtocol* proto;


CRGB leds[LEDS_ATTACHED];

WiFiUDP udpclient;
//LEDWall::simplevec<uint16_t> substr_bounds(NUM_SUBSTRINGS-1);
//LEDWall::simplevec<uint8_t> network_data(LEDWall::UDP_PACKET_SIZE);

uint8_t network_data[UDP_PACKET_SIZE];


typedef void (*voidfp)();

void errorBlink(int numBlinks=5)
{
  for(int i=0; i < numBlinks; i++)
  {
    digitalWrite(ERROR_LED_PIN,HIGH);
    delay(ERROR_LED_WAIT);
    digitalWrite(ERROR_LED_PIN,LOW);
    delay(ERROR_LED_WAIT);
  }
  
}


void setup() {
  Serial.begin(115200);

  Serial.println("starting wifi");
  WiFi.begin(ssid,password);

  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    
  }
  // put your setup code here, to run once:

  //connect to test AP
  
  

  Serial.println("wifi connected");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());

  if(udpclient.begin(UDP_PORT) == 1)
  {
    Serial.println("UDP connection successful");
    Serial.println("UDP port: ");
    Serial.println(udpclient.localPort());
    
  }
  //setup LED substring structure
  
  

  
 //setup UDP receive


 //setup FastLED stuff
 FastLED.addLeds<WS2811, LED_PIN_1,  RGB>(leds,0, 150);
 FastLED.addLeds<WS2811, LED_PIN_2, RGB>(leds, 150, 100  );
 

//substr_bounds[0] = SUBSTR_ADDR_SPLIT;

//setup network protocol handler
  proto = new LEDWall::NetworkProtocol(LEDS_ATTACHED);
  proto->reset_recv();

//setup error blinky
 pinMode(ERROR_LED_PIN, OUTPUT);
 
}

void loop() {
  // put your main code here  , to run repeatedly:

  //errorBlink(2);
  //Serial.print("#");
  auto readstat = udpclient.parsePacket();

  if(readstat == 0)
  {
    delay(POLL_DELAY);
    return;
  }


  Serial.print("!");
  auto readbytes = udpclient.read(network_data, UDP_PACKET_SIZE);

  Serial.print("Read:");
  Serial.print(readbytes);
  Serial.print(" bytes");

 
 // Serial.print("packet recvd");

 if( (readbytes -12 ) != (LEDS_ATTACHED *3))
 {
  Serial.println("length mismatch error!");
  digitalWrite(ERROR_LED_PIN,HIGH);
  return;
 }

  //HACK
  for(uint16_t i=0 ;i < LEDS_ATTACHED ; i++)
  {
      leds[i].r = *(network_data +i*3 + 12);
      leds[i].g = *(network_data + i*3 + 1 + 12);
      leds[i].b = *(network_data + i*3 + 2 + 12);  
      
  }

  FastLED.show();

  //reply to host

  Serial.println("replying to " );
  Serial.println(udpclient.remoteIP());
  char reply[] = "acknowledged";
  udpclient.beginPacket(udpclient.remoteIP(),UDP_PORT);
  udpclient.write(reply);
  udpclient.endPacket();

  
  
  

}
