#include "NetworkProtocol.h"


LEDWall::NetworkProtocol::NetworkProtocol(const uint16_t leds_attached, LEDWall::simplevec< uint16_t >* substr_bounds)
: _leds_attached(leds_attached), _led_data(leds_attached * 3), _errc(NetworkProtocolError::NO_ERROR), _ready(false),
_substr_bounds(substr_bounds)
{
  
  

}


void LEDWall::NetworkProtocol::receive_network_packet(const LEDWall::simplevec< uint8_t >& data)
{
  //cast to a LEDHeader to avoid copy;
  LEDHeader* hdr = reinterpret_cast<LEDHeader*>(data.data());
  
  //check string length
  if(hdr->led_string_length != _leds_attached)
  {
    _errc = NetworkProtocolError::LED_STRING_LENGTH_INCORRECT;
    return;
  };
  
  if(_packets_to_recv <0 ) //this is a new sequence
  {
    _packets_to_recv = hdr->sequence_length;
    _total_sequence_size = hdr->sequence_length;
    
  }
  else if(_total_sequence_size != hdr->sequence_length)
  {
    _errc = NetworkProtocolError::SEQUENCE_LENGTH_MISMATCH;
  }
  else
  {
    _packets_to_recv--;
  };
  
  //find out correct address to put the packet in
  uint16_t start_addr = (hdr->sequence_number) * UDP_PAYLOAD_SIZE ;
  
  //copy the data
  unsigned count;
  
  //TODO: this needs a fix!
  //HACK: doesn't work for large LED strings
  count = (UDP_PAYLOAD_SIZE > _leds_attached) ? _leds_attached : UDP_PAYLOAD_SIZE;
   
  auto pt = _led_data.data();
  auto payload = reinterpret_cast<uint8_t*>( data.data() + UDP_HEADER_SIZE);
  for(unsigned i = start_addr ; i < ( count + start_addr) ; i++)
  {
    *(pt + i) = *payload;
    payload++;
  }
    
  
}


void LEDWall::NetworkProtocol::reset_recv()
{
  _packets_to_recv = -1;
  _ready = false;

}


LEDWall::NetworkProtocolError LEDWall::NetworkProtocol::errc() const
{
  
  return _errc;

}


bool LEDWall::NetworkProtocol::ready() const
{
  
  
  return _ready;
  
}


void LEDWall::NetworkProtocol::clear_errc()
{
  _errc = NetworkProtocolError::NO_ERROR;

}

void LEDWall::NetworkProtocol::transfer_to_fastLED(CRGB** fastled_arr) const
{
  
  auto changept = (_substr_bounds == nullptr) ? _leds_attached : _substr_bounds[0];
  unsigned changeptidx = 0;
  
  
  auto LED_substr = fastled_arr[changeptidx];
  
  for(unsigned i = 0; i < _leds_attached; i++)
  {
    if(i == changept)
    {
      changeptidx++;
      changept = _substr_bounds[changeptidx];
      LED_substr = fastled_arr[changeptidx];
      
    };
      
    auto led_base_ptr = _led_data.data() + (i*3);
    
    (LED_substr+i) -> R = *(led_base_ptr + 0);
    (LED_substr +i) -> G = *(led_base_ptr + 1);
    (LED_substr +i) ->B = *(led_base_ptr + 2);
    
  };

}




