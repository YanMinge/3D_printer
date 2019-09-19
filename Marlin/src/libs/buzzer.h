/**
 * Marlin 3D Printer Firmware
 * Copyright (C) 2019 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
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

#include "../inc/MarlinConfig.h"

#define VOICE_L1    262 // c
#define VOICE_L2    294 // d
#define VOICE_L3    330 // e
#define VOICE_L4    349 // f
#define VOICE_L5    392 // g
#define VOICE_L6    440 // a1
#define VOICE_L7    494 // b1

#define VOICE_M1    523 // c1
#define VOICE_M2    587 // d1
#define VOICE_M3    659 // e1
#define VOICE_M4    698 // f1
#define VOICE_M5    784 // g1
#define VOICE_M6    880 // a2
#define VOICE_M7    988 // b2

#define VOICE_H1    1047 // c2
#define VOICE_H2    1175 // d2
#define VOICE_H3    1319 // e2
#define VOICE_H4    1397 // f2
#define VOICE_H5    1568 // g2
#define VOICE_H6    1760 // a3
#define VOICE_H7    1976 // b3
#define VOICE_T     2000
#define VOICE_TT    1600

#if ENABLED(LCD_USE_I2C_BUZZER)

  #define BUZZ(d,f) ui.buzz(d,f)

#elif PIN_EXISTS(BEEPER)

  #include "circularqueue.h"

  #define TONE_QUEUE_LENGTH 4

  /**
   * @brief Tone structure
   * @details Simple abstraction of a tone based on a duration and a frequency.
   */
  struct tone_t {
    uint16_t duration;
    uint16_t frequency;
  };

  /**
   * @brief Buzzer class
   */
  class Buzzer {
    public:

      typedef struct {
        tone_t   tone;
        uint32_t endtime;
      } state_t;

    private:
      static state_t state;
#if ENABLED(TARGET_LPC1768)
	  bool buzzer_enable;
#endif
    protected:
      static CircularQueue<tone_t, TONE_QUEUE_LENGTH> buffer;

      /**
       * @brief Inverts the sate of a digital PIN
       * @details This will invert the current state of an digital IO pin.
       */
      FORCE_INLINE static void invert() { TOGGLE(BEEPER_PIN); }

      /**
       * @brief Turn off a digital PIN
       * @details Alias of digitalWrite(PIN, LOW) using FastIO
       */
      //FORCE_INLINE static void off() { WRITE(BEEPER_PIN, LOW); }
      FORCE_INLINE static void off() { analogWrite(BEEPER_PIN, 0); }

      /**
       * @brief Turn on a digital PIN
       * @details Alias of digitalWrite(PIN, HIGH) using FastIO
       */
      FORCE_INLINE static void on() { WRITE(BEEPER_PIN, HIGH); }

      /**
       * @brief Resets the state of the class
       * @details Brings the class state to a known one.
       */
      static inline void reset() {
        off();
        state.endtime = 0;
      }

    public:
      /**
       * @brief Class constructor
       */
      Buzzer() {
        SET_OUTPUT(BEEPER_PIN);
        reset();
      }

#if ENABLED(TARGET_LPC1768)
      void set_buzzer_switch(bool status);
      bool get_buzzer_switch(void);
#endif
#if ENABLED(USE_DWIN_LCD)
      void buzzer_icon_init(void);
#endif

      /**
       * @brief Add a tone to the queue
       * @details Adds a tone_t structure to the ring buffer, will block IO if the
       *          queue is full waiting for one slot to get available.
       *
       * @param duration Duration of the tone in milliseconds
       * @param frequency Frequency of the tone in hertz
       */
      static void tone(const uint16_t duration, const uint16_t frequency=0);

      /**
       * @brief Tick function
       * @details This function should be called at loop, it will take care of
       *          playing the tones in the queue.
       */
      static void tick();

      /**
       * @brief clear the tone in the queue and stop buzzer imediately
       *
       */
       static void clear();
  };

  // Provide a buzzer instance
  extern Buzzer buzzer;
  #define BUZZ(d,f) buzzer.tone(d, f)

#else // No buzz capability

  #define BUZZ(d,f) NOOP

#endif
