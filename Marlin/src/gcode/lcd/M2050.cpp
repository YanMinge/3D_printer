/**   
 * \par Copyright (C), 2018-2019, MakeBlock
 * @file    M2050.cpp
 * @author  Mark Yan
 * @version V1.0.0
 * @date    2019/05/24
 * @brief   source code for M2050.
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
 * Set/Query the language type of the LCD UI.
 * \par History:
 * <pre>
 * `<Author>`         `<Time>`        `<Version>`        `<Descr>`
 * Mark Yan         2019/06/14     1.0.0            Initial function design.
 * </pre>
 *
 */

#include "../../inc/MarlinConfig.h"

#if ENABLED(USE_DWIN_LCD)

#include "../gcode.h"
#include "lcd_process.h"

/**
 * M2050: Set/Query the language type of the LCD UI.
 *      OR, with 'L<type>' set the language type.
 *      OR, with 'C' get the current language type.
 */
void GcodeSuite::M2050()
{
  if (parser.seen('C'))
  {
    SERIAL_ECHOPGM("Current language: ");
    if(dwin_process.get_language_type() == LAN_CHINESE)
    {
       SERIAL_ECHOLNPGM("chinese");
    }
    else if(dwin_process.get_language_type() == LAN_ENGLISH)
    {
       SERIAL_ECHOLNPGM("english");
    }
    else
    {
       SERIAL_ECHOLNPGM("not yet set ");
    }
  }
  else if (parser.seen('L'))
  {
    dwin_process.set_language_type((language_type)parser.value_byte());
  }
}

#endif // USE_DWIN_LCD
