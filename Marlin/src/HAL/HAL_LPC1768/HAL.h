/**
 * Marlin 3D Printer Firmware
 *
 * Copyright (C) 2019 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 * Copyright (c) 2016 Bob Cousins bobcousins42@googlemail.com
 * Copyright (c) 2015-2016 Nico Tonnhofer wurstnase.reprap@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#pragma once

/**
 * HAL_LPC1768/HAL.h
 * Hardware Abstraction Layer for NXP LPC1768
 */

#define CPU_32_BIT
#define HAL_INIT

void HAL_init();

#include <stdint.h>
#include <stdarg.h>
#include <algorithm>

extern "C" volatile millis_t _millis;

#include <Arduino.h>
#include <pinmapping.h>

#include "../shared/math_32bit.h"
#include "../shared/HAL_SPI.h"
#include "fastio.h"
#include <adc.h>
#include "watchdog.h"
#include "HAL_timers.h"
#include "MarlinSerial.h"

//
// Default graphical display delays
//
#ifndef ST7920_DELAY_1
  #define ST7920_DELAY_1 DELAY_NS(600)
#endif
#ifndef ST7920_DELAY_2
  #define ST7920_DELAY_2 DELAY_NS(750)
#endif
#ifndef ST7920_DELAY_3
  #define ST7920_DELAY_3 DELAY_NS(750)
#endif

#if !WITHIN(SERIAL_PORT, -1, 3)
  #error "SERIAL_PORT must be from -1 to 3"
#endif

#if SERIAL_PORT == -1
  #define MYSERIAL0 UsbSerial
#elif SERIAL_PORT == 0
  #define MYSERIAL0 MSerial
#elif SERIAL_PORT == 1
  #define MYSERIAL0 MSerial1
#elif SERIAL_PORT == 2
  #define MYSERIAL0 MSerial2
#elif SERIAL_PORT == 3
  #define MYSERIAL0 MSerial3
#endif

#ifdef SERIAL_PORT_2
  #if !WITHIN(SERIAL_PORT_2, -1, 3)
    #error "SERIAL_PORT_2 must be from -1 to 3"
  #elif SERIAL_PORT_2 == SERIAL_PORT
    #error "SERIAL_PORT_2 must be different than SERIAL_PORT"
  #endif
  #define NUM_SERIAL 2
  #if SERIAL_PORT_2 == -1
    #define MYSERIAL1 UsbSerial
  #elif SERIAL_PORT_2 == 0
    #define MYSERIAL1 MSerial
  #elif SERIAL_PORT_2 == 1
    #define MYSERIAL1 MSerial1
  #elif SERIAL_PORT_2 == 2
    #define MYSERIAL1 MSerial2
  #elif SERIAL_PORT_2 == 3
    #define MYSERIAL1 MSerial3
  #endif
#else
  #define NUM_SERIAL 1
#endif

//
// serial_port_3 for lcd communication
//
#ifdef SERIAL_PORT_3
  #if !WITHIN(SERIAL_PORT_3, -1, 3)
    #error "SERIAL_PORT_3 must be from -1 to 3"
  #elif SERIAL_PORT_3 == SERIAL_PORT
    #error "SERIAL_PORT_3 must be different than SERIAL_PORT"
  #endif
  #if SERIAL_PORT_3 == -1
    #define MYSERIAL2 UsbSerial
  #elif SERIAL_PORT_3 == 0
    #define MYSERIAL2 MSerial
  #elif SERIAL_PORT_3 == 1
    #define MYSERIAL2 MSerial1
  #elif SERIAL_PORT_3 == 2
    #define MYSERIAL2 MSerial2
  #elif SERIAL_PORT_3 == 3
    #define MYSERIAL2 MSerial3
  #elif SERIAL_PORT_2 == 4
    #define MYSERIAL2 VirtualSerial
  #endif
#endif

//
// Interrupts
//
#define CRITICAL_SECTION_START  uint32_t primask = __get_PRIMASK(); __disable_irq()
#define CRITICAL_SECTION_END    if (!primask) __enable_irq()
#define ISRS_ENABLED() (!__get_PRIMASK())
#define ENABLE_ISRS()  __enable_irq()
#define DISABLE_ISRS() __disable_irq()

//
// Utility functions
//
int freeMemory(void);

//
// SPI: Extended functions taking a channel number (Hardware SPI only)
//

// Write single byte to specified SPI channel
void spiSend(uint32_t chan, byte b);
// Write buffer to specified SPI channel
void spiSend(uint32_t chan, const uint8_t* buf, size_t n);
// Read single byte from specified SPI channel
uint8_t spiRec(uint32_t chan);

// EEPROM
void eeprom_write_byte(uint8_t *pos, unsigned char value);
uint8_t eeprom_read_byte(uint8_t *pos);
void eeprom_read_block (void *__dst, const void *__src, size_t __n);
void eeprom_update_block (const void *__src, void *__dst, size_t __n);

//
// ADC API
//

#define ADC_MEDIAN_FILTER_SIZE (23) // Higher values increase step delay (phase shift),
                                    // (ADC_MEDIAN_FILTER_SIZE + 1) / 2 sample step delay (12 samples @ 500Hz: 24ms phase shift)
                                    // Memory usage per ADC channel (bytes): (6 * ADC_MEDIAN_FILTER_SIZE) + 16
                                    // 8 * ((6 * 23) + 16 ) = 1232 Bytes for 8 channels

#define ADC_LOWPASS_K_VALUE    (2)  // Higher values increase rise time
                                    // Rise time sample delays for 100% signal convergence on full range step
                                    // (1 : 13, 2 : 32, 3 : 67, 4 : 139, 5 : 281, 6 : 565, 7 : 1135, 8 : 2273)
                                    // K = 6, 565 samples, 500Hz sample rate, 1.13s convergence on full range step
                                    // Memory usage per ADC channel (bytes): 4 (32 Bytes for 8 channels)

using FilteredADC = LPC176x::ADC<ADC_LOWPASS_K_VALUE, ADC_MEDIAN_FILTER_SIZE>;
#define HAL_adc_init()         FilteredADC::init()
#define HAL_ANALOG_SELECT(pin) FilteredADC::enable_channel(pin)
#define HAL_START_ADC(pin)     FilteredADC::start_conversion(pin)
#define HAL_READ_ADC()         FilteredADC::get_result()
#define HAL_ADC_READY()        FilteredADC::finished_conversion()

// Parse a G-code word into a pin index
int16_t PARSED_PIN_INDEX(const char code, const int16_t dval);
// P0.6 thru P0.9 are for the onboard SD card
#define HAL_SENSITIVE_PINS P0_06, P0_07, P0_08, P0_09

#define HAL_IDLETASK 1
void HAL_idletask(void);

#define PLATFORM_M997_SUPPORT
void flashFirmware(int16_t value);
