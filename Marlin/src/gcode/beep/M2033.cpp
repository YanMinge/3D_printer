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
#if ENABLED(TARGET_LPC1768)
#if PIN_EXISTS(BEEPER)

#include "../gcode.h"
#include "../../libs/buzzer.h"

/**
 * M2030: Set/Query the buzzer enable status.
 *      OR, with 'L<type>' set the buzzer status.
 *      OR, with 'NULL parameters' get the current buzzer status.
 */
void GcodeSuite::M2033()
{
  if (parser.seen('S'))
  {
    buzzer.set_buzzer_switch(parser.value_bool());
  }
  else
  {
    SERIAL_ECHOPGM("buzzer: ");
    if(buzzer.get_buzzer_switch() == true)
    {
       SERIAL_ECHOLNPGM("enable");
    }
    else
    {
       SERIAL_ECHOLNPGM("disable");
    }
  }
}
#endif //BEEPER
#endif //PIN_EXISTS(BEEPER)