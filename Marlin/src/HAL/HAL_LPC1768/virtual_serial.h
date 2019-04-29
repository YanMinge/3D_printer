/**   
 * \par Copyright (C), 2018-2019, MakeBlock
 * \class   virtual_serial
 * \brief   Virtual serial port driver.
 * @file    virtual_serial.h
 * @author  MakeBlock
 * @version V1.0.0
 * @date    2019/04/29
 * @brief   Header file for Virtual serial port driver.
 *
 * \par Copyright
 * This software is Copyright (C), 2018-2019, MakeBlock. Use is subject to license \n
 * conditions. The main licensing options available are GPL V2 or Commercial: \n
 *
 * \par Open Source Licensing GPL V2
 * This is the appropriate option if you want to share the source code of your \n
 * application with everyone you distribute it to, and you also want to give them \n
 * the right to share who uses it. If you wish to use this software under Open \n
 * Source Licensing, you must contribute all your source code to the open source \n
 * community in accordance with the GPL Version 2 when your application is \n
 * distributed. See http://www.gnu.org/copyleft/gpl.html
 *
 * \par Description
 * This file is a drive for Virtual serial port.
 *
 * \par Method List:
 *
 *    1.  void    virtual_serial::begin(int32_t baud);
 *    2.  int16_t virtual_serial::peek(void);
 *    3.  int16_t virtual_serial::peek_tx(void);
 *    4.  int16_t virtual_serial::read(void);
 *    5.  int16_t virtual_serial::read_tx(void);
 *    6.  size_t virtual_serial::write(const uint8_t c);
 *    7.  size_t virtual_serial::available(void);
 *    8.  size_t virtual_serial::available_tx(void);
 *    9.  void virtual_serial::flush(void);
 *    10.  void virtual_serial::flush_tx(void);
 *    11.  uint8_t virtual_serial::availableForWrite(void);
 *    12.  size_t virtual_serial::printf(const char *format, ...);
 *    13.  size_t virtual_serial::printf_rx(const char *format, ...);
 *
 * \par History:
 * <pre>
 * `<Author>`         `<Time>`        `<Version>`        `<Descr>`
 * Mark Yan         2019/04/29     1.0.0            Initial function design.
 * </pre>
 *
 */

#ifndef _VIRTUAL_SERIAL_H_
#define _VIRTUAL_SERIAL_H_

#ifdef TARGET_LPC1768
#include <stdarg.h>
#include <stdio.h>
#include <Stream.h>
#include <Arduino.h>

#ifndef VIRTUAL_SERIAL_TIMEOUT
  #define VIRTUAL_SERIAL_TIMEOUT 2500
#endif


/**
 * Generic ring_buffer
 * T type of the buffer array
 * S size of the buffer (must be power of 2)
 */
template <typename T, uint32_t S> class ring_buffer {
public:
  ring_buffer() {index_read = index_write = 0;}

  uint32_t available() const {return mask(index_write - index_read);}
  uint32_t free() const {return size() - available();}
  bool empty() const {return index_read == index_write;}
  bool full() const {return next(index_write) == index_read;}
  void clear() {index_read = index_write = 0;}

  bool peek(T *const value) const {
    if (value == nullptr || empty()) {
      return false;
    }
    *value = buffer[index_read];
    return true;
  }

  uint32_t read(T *const value) {
    if (value == nullptr || empty()) {
      return 0;
    }
    *value = buffer[index_read];
    index_read = next(index_read);
    return 1;
  }

  uint32_t write(const T value) {
    uint32_t next_head = next(index_write);
    if (next_head == index_read) {
      return 0;     // buffer full
    }
    buffer[index_write] = value;
    index_write = next_head;
    return 1;
  }

  constexpr uint32_t size() const {
    return buffer_size - 1;
  }

private:
  inline uint32_t mask(uint32_t val) const {
    return val & buffer_mask;
  }

  inline uint32_t next(uint32_t val) const {
    return mask(val + 1);
  }

  static const uint32_t buffer_size = S;
  static const uint32_t buffer_mask = buffer_size - 1;
  volatile T buffer[buffer_size];
  volatile uint32_t index_write;
  volatile uint32_t index_read;
};

/**
 *  Serial Interface Class
 *  Data is injected directly into, and consumed from, the fifo buffers
 */
//: public Stream 
class virtual_serial : public Stream{
public:

  virtual_serial(void);
  virtual ~virtual_serial(void) { }

  operator bool(void) { return true; }

  void begin(int32_t baud);

  int16_t peek(void);

  int16_t peek_tx(void);

  int16_t read(void);

  int16_t read_tx(void);

  size_t write(const uint8_t c);

  size_t available(void);

  size_t available_tx(void);

  void flush(void);

  void flush_tx(void);

  uint8_t availableForWrite(void);

  size_t printf(const char *format, ...);

  size_t printf_rx(const char *format, ...);

  ring_buffer<uint8_t, 128> receive_buffer;
  ring_buffer<uint8_t, 512> transmit_buffer;
  
};

extern virtual_serial VirtualSerial;
#endif // TARGET_LPC1768
#endif // _VIRTUAL_SERIAL_H_
