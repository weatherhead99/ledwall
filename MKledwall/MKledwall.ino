

#include <FastLED.h>


#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

#include <NetworkProtocol.h>

#define UDP_PORT 17200
#define LEDS_ATTACHED 100
#define ERROR_LED_PIN 2
#define ERROR_LED_WAIT 100
#define LED_PIN_1 4 
#define LED_PIN_2 5

#define FRAME_DELAY 100

#define POLL_DELAY 100

#define NUM_SUBSTRINGS 2
#define SUBSTRING_1_LENGTH 50
#define SUBSTRING_2_LENGTH 50
#define SUBSTR_ADDR_SPLIT 50


//cxonnect to test wifi AP
char ssid[] = "ledwall_test";


LEDWall::NetworkProtocol* proto;
WiFiUDP udpclient;

CRGB** leds;


LEDWall::simplevec<uint16_t> substr_bounds(NUM_SUBSTRINGS-1);
LEDWall::simplevec<uint8_t> network_data(LEDWall::UDP_PACKET_SIZE);


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

  
  
  // put your setup code here, to run once:

  //connect to test AP
  WiFi.begin(ssid);

  
  //setup LED substring structure
  leds = new CRGB* [NUM_SUBSTRINGS];
  leds[0] = new CRGB[SUBSTRING_1_LENGTH];
  leds[1] = new CRGB[SUBSTRING_2_LENGTH];

  
 //setup UDP receive
 auto conn_status = udpclient.begin(UDP_PORT);

 //setup FastLED stuff
 FastLED.addLeds<WS2811, LED_PIN_1,  RGB>(leds[0], SUBSTRING_1_LENGTH);
 FastLED.addLeds<WS2811, LED_PIN_2, RGB>(leds[1], SUBSTRING_2_LENGTH);

substr_bounds[0] = SUBSTR_ADDR_SPLIT;

//setup network protocol handler
  proto = new LEDWall::NetworkProtocol(LEDS_ATTACHED, &substr_bounds);

//setup error blinky
 pinMode(ERROR_LED_PIN, OUTPUT);
 
}

void loop() {
  // put your main code here, to run repeatedly:

  errorBlink(2);
  auto readstat = udpclient.parsePacket();

  if(readstat == 0)
  {
    delay(POLL_DELAY);
  }
  
  udpclient.read(network_data.data(), LEDWall::UDP_PACKET_SIZE);

  proto->reset_recv();
  proto->receive_network_packet(network_data);

  if(proto->ready())
  {

    proto->transfer_to_FastLED(leds);
    FastLED.show();

    delay(FRAME_DELAY);
  }
  

}
