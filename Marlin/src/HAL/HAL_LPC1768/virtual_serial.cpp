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

#ifdef TARGET_LPC1768
#include "../../inc/MarlinConfigPre.h"

#include "virtual_serial.h"
#include "MarlinSerial.h"

virtual_serial VirtualSerial;

virtual_serial::virtual_serial(void):Stream(){ }

void virtual_serial::begin(int32_t baud) { }

int16_t virtual_serial::peek(void) {
  uint8_t value;
  return receive_buffer.peek(&value) ? value : -1;
}

int16_t virtual_serial::peek_tx(void) {
  uint8_t value;
  uint32_t ret = transmit_buffer.read(&value);
  return (ret ? value : -1);
}

int16_t virtual_serial::read(void) {
  uint8_t value;
  uint32_t ret = receive_buffer.read(&value);
  return (ret ? value : -1);
}

int16_t virtual_serial::read_tx(void) {
  uint8_t value;
  uint32_t ret = transmit_buffer.read(&value);
  return (ret ? value : -1);
}

size_t virtual_serial::write(const uint8_t c) {
  uint32_t current_time = millis();
  while (transmit_buffer.write(c) == 0) {
    if(millis() - current_time > VIRTUAL_SERIAL_TIMEOUT) {
      return	0;
    }
  }
  return 1;
}

size_t virtual_serial::available(void) {
  return (size_t)receive_buffer.available();
}

size_t virtual_serial::available_tx(void) {
  return (size_t)transmit_buffer.available();
}

void virtual_serial::flush(void) {
  receive_buffer.clear();
}

void virtual_serial::flush_tx(void) {
  transmit_buffer.clear();
}

uint8_t virtual_serial::availableForWrite(void) {
  return transmit_buffer.free() > 255 ? 255 : (uint8_t)transmit_buffer.free();
}

size_t virtual_serial::printf(const char *format, ...) {
  static char buffer[512];
  va_list vArgs;
  va_start(vArgs, format);
  int length = vsnprintf((char *) buffer, 256, (char const *) format, vArgs);
  va_end(vArgs);
  size_t i = 0;
  if (length > 0 && length < 512) {
    while (i < (size_t)length) {
      i += transmit_buffer.write(buffer[i]);
    }
  }
  return i;
}

size_t virtual_serial::printf_rx(const char *format, ...) {
  static char buffer[256];
  va_list vArgs;
  va_start(vArgs, format);
  int length = vsnprintf((char *) buffer, 256, (char const *) format, vArgs);
  va_end(vArgs);
  size_t i = 0;
  if (length > 0 && length < 256) {
    while (i < (size_t)length) {
      i += receive_buffer.write(buffer[i]);
   }
  }
  return i;
}
#endif // TARGET_LPC1768
