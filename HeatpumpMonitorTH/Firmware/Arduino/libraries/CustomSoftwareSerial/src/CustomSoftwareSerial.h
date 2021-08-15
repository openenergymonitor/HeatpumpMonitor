/*
CustomSoftwareSerial.h (formerly NewSoftSerial.h) -
Multi-instance software serial library for Arduino/Wiring
-- Interrupt-driven receive and other improvements by ladyada
   (http://ladyada.net)
-- Tuning, circular buffer, derivation from class Print/Stream,
   multi-instance support, porting to 8MHz processors,
   various optimizations, PROGMEM delay tables, inverse logic and
   direct port writing by Mikal Hart (http://www.arduiniana.org)
-- Pin change interrupt macros by Paul Stoffregen (http://www.pjrc.com)
-- 20MHz processor support by Garrett Mace (http://www.macetech.com)
-- ATmega1280/2560 support by Brett Hagman (http://www.roguerobotics.com/)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

The latest version of this library can always be found at
http://arduiniana.org.
*/

#ifndef CustomSoftwareSerial_h
#define CustomSoftwareSerial_h

#include <inttypes.h>
#include <Stream.h>

/******************************************************************************
* Definitions
******************************************************************************/

#define _SS_MAX_RX_BUFF 64 // RX buffer size
#ifndef GCC_VERSION
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif

#define CSERIAL_5N1 501
#define CSERIAL_6N1 601
#define CSERIAL_7N1 701
#define CSERIAL_8N1 801

#define CSERIAL_5N2 502
#define CSERIAL_6N2 602
#define CSERIAL_7N2 702
#define CSERIAL_8N2 802

#define CSERIAL_5O1 511
#define CSERIAL_6O1 611
#define CSERIAL_7O1 711
#define CSERIAL_8O1 811

#define CSERIAL_5O2 512
#define CSERIAL_6O2 612
#define CSERIAL_7O2 712
#define CSERIAL_8O2 812

#define CSERIAL_5E1 521
#define CSERIAL_6E1 621
#define CSERIAL_7E1 721
#define CSERIAL_8E1 821

#define CSERIAL_5E2 522
#define CSERIAL_6E2 622
#define CSERIAL_7E2 722
#define CSERIAL_8E2 822

enum Parity {
    NONE = 0,
    ODD = 1,
    EVEN = 2
};

class CustomSoftwareSerial : public Stream
{
private:
  // per object data
  uint8_t _receivePin;
  uint8_t _receiveBitMask;
  volatile uint8_t *_receivePortRegister;
  uint8_t _transmitBitMask;
  volatile uint8_t *_transmitPortRegister;

  uint16_t _rx_delay_centering;
  uint16_t _rx_delay_intrabit;
  uint16_t _rx_delay_stopbit;
  uint16_t _tx_delay;

  uint16_t _buffer_overflow:1;
  uint16_t _inverse_logic:1;

  uint8_t _numberOfDataBit;
  uint8_t _maxValueOfDataBit;
  Parity _parityBit;
  uint8_t _numberOfStopBit;


  // static data
  static char _receive_buffer[_SS_MAX_RX_BUFF];
  static volatile uint8_t _receive_buffer_tail;
  static volatile uint8_t _receive_buffer_head;
  static CustomSoftwareSerial *active_object;

  // private methods
  void recv();
  uint8_t rx_pin_read();
  void tx_pin_write(uint8_t pin_state);
  void setTX(uint8_t transmitPin);
  void setRX(uint8_t receivePin);

  void setPort(uint16_t configuration);

  // private static method for timing
  static inline void tunedDelay(uint16_t delay);

public:
  // public methods
  CustomSoftwareSerial(uint8_t receivePin, uint8_t transmitPin, bool inverse_logic = false);
  ~CustomSoftwareSerial();
  void begin(long speed);
  void begin(long speed, uint16_t configuration);
  bool listen();
  void end();
  bool isListening() { return this == active_object; }
  bool overflow() { bool ret = _buffer_overflow; _buffer_overflow = false; return ret; }
  int peek();

  uint8_t getNumberOfDataBit();
  Parity getParity();
  uint8_t getNumberOfStopBit();
  uint8_t calculateNumberOfBits1(uint8_t sentData);
  void writeStopBits();
  void writeParityBits(uint8_t numberOfBit1);

  virtual size_t write(uint8_t byte);
  virtual int read();
  virtual int available();
  virtual void flush();

  using Print::write;

  // public only for easy access by interrupt handlers
  static inline void handle_interrupt();
};

// Arduino 0012 workaround
#undef int
#undef char
#undef long
#undef byte
#undef float
#undef abs
#undef round

#endif
