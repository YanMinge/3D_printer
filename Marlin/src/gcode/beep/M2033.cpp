/**
 * \par Copyright (C), 2018-2019, MakeBlock
 * @file    M2033.cpp
 * @author  Akingpan
 * @version V1.0.0
 * @date    2019/05/24
 * @brief   mcode file that used to control beep.
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
 * Command to contorl beep.
 * \par History:
 * <pre>
 * `<Author>`         `<Time>`        `<Version>`        `<Descr>`
 * Akingpan         2019/05/24     1.0.0            Initial function design.
 * </pre>
 *
 */

#include "../../inc/MarlinConfig.h"

#if PIN_EXISTS(BEEPER)

#include "../gcode.h"

uint32_t beep_times_count;
/**
 * M2027: Get SD Card status
 *      OR, with 'S<seconds>' set the SD status auto-report interval. (Requires AUTO_REPORT_SD_STATUS)
 *      OR, with 'C' get the current filename.
 */
void GcodeSuite::M2033()
{
  bool s;
  uint32_t beep_frequency;
  if (parser.seen('S'))
  {
    SERIAL_ECHOPGM("M2033:\r\n");
    s = parser.value_bool();
    if(s)
    {
      analogWrite(BEEPER_PIN,100);
    }
    else
    {
      analogWrite(BEEPER_PIN,0);
    }
  }
  if (parser.seen('T'))
  {
    SERIAL_ECHOPGM("M2033:\r\n");
    beep_times_count = parser.value_int();
    beep_times_count *= 1000;
  }
  if (parser.seen('F'))
  {
    SERIAL_ECHOPGM("M2033:\r\n");
    beep_frequency = parser.value_ulong();
    pwm_set_frequency(BEEPER_PIN,beep_frequency);
  }
}

#endif //PIN_EXISTS(BEEPER)