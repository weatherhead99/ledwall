#pragma once

#include <stdint.h>

//forward declare from FastLED
struct CRGB;


namespace LEDWall
{
  
  //12 byte header
  //2 bytes LED string length (i.e. number in total broadcast)
  //must always match that in device
  // 1 byte sequence length
  // 1 byte sequence number
  // 2 bytes start address -- CURRENTLY NOT USED
  // 2 bytes end address -- CURRENTLY NOT USED
  // 4 byte checksum (of data) -- CURRENTLY NOT USED
  
  
  //byte aligned (upto 500 bytes)
  
  const uint16_t UDP_PACKET_SIZE = 512;
  const uint16_t UDP_HEADER_SIZE = 12;
  const uint16_t UDP_PAYLOAD_SIZE = 500;
  
#pragma pack(push, 1)
  struct LEDHeader
  {
    uint16_t led_string_length;
    uint8_t sequence_length;
    uint8_t sequence_number;
    
    uint16_t start_addr;
    
    uint16_t flags;
    
    //TODO: CRC impl
    uint32_t checksum;
  };
#pragma pack(pop)
  
#pragma pack(push,1)
 struct LEDStatus
 {
   uint16_t num_leds_attached;
   uint8_t status;
 };
  
#pragma pack(pop)
  
  template <typename T> class simplevec
  {
  public:
    simplevec(uint16_t size);
    ~simplevec();
    T& operator[] (uint16_t idx);
    uint16_t size() const;
    
    T* data() const;
    
    class iterator
    {
    public:
      iterator();
      ~iterator();
      iterator& operator++();
      bool operator==(const iterator& it) const;
      bool operator!=(const iterator& it) const;
      
      
    private:
      uint16_t _idx;
      simplevec<T>& _simplevec;
      
    };
    
    iterator begin();
    iterator end();
    
  private:
    T* _data;
    uint16_t _size;
    
    
    
  };
  
  enum class NetworkProtocolError
  {
    NO_ERROR,
    MISSING_PACKET,
    PACKET_LENGTH_MISMATCH,
    LED_STRING_LENGTH_INCORRECT,
    SEQUENCE_LENGTH_MISMATCH,
    
    
  };
  
  
  class NetworkProtocol
  {
  public:
    
      NetworkProtocol(const uint16_t leds_attached, simplevec<uint16_t>* substr_bounds = nullptr );
      
      void reset_recv();
      void receive_network_packet(const simplevec<uint8_t>& data);
      bool ready() const;
      uint8_t substr() const;
      NetworkProtocolError errc() const;
      void transfer_to_fastLED(CRGB** fastled_arr) const;
      
  private:
      void clear_errc(); 
      uint16_t _leds_attached;
      
      simplevec<uint16_t>* _substr_bounds = nullptr;
      simplevec<uint8_t> _led_data;
      NetworkProtocolError _errc;
      bool _ready;
      uint8_t _packets_to_recv = -1;
      uint8_t _total_sequence_size = 0;
      
      
  };
  
  
};


namespace LEDWall
{
  
  template <typename T> simplevec<T>::simplevec(uint16_t size) : _size(size)
  {
    
    _data = new T(_size);
    
  }

  
  template <typename T> simplevec<T>::~simplevec() 
  {
    delete _data;
    
  };
  
  
  template <typename T> T& simplevec<T>::operator[] (uint16_t idx)
  {
    
    return *(_data + idx);
    
  };
  
  template <typename T> uint16_t simplevec<T>::size() const
  {
    return _size;
    
  };

  template <typename T> T* simplevec<T>::data() const
  {
    return _data;
  };
  
}//namespace

