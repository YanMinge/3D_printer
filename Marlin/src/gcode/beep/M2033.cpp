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

uint8_t beep_status = 1;
/**
 * M2027: Get SD Card status
 *      OR, with 'S<seconds>' set the SD status auto-report interval. (Requires AUTO_REPORT_SD_STATUS)
 *      OR, with 'C' get the current filename.
 */
void GcodeSuite::M2033()
{
  if (parser.seen('S'))
  {
    SERIAL_ECHOPGM("M2033:\r\n");
    beep_status = parser.value_byte();
  }
}

#endif //PIN_EXISTS(BEEPER)