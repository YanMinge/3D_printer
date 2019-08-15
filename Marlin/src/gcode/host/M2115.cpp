/**
 * \par Copyright (C), 2018-2019, MakeBlock
 * @file    M2115.cpp
 * @author  Mark Yan
 * @version V1.0.0
 * @date    2019/08/15
 * @brief   source code for M2115.
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
 * Query firmware information.
 * \par History:
 * <pre>
 * `<Author>`         `<Time>`        `<Version>`        `<Descr>`
 * Mark Yan         2019/08/15     1.0.0            Initial function design.
 * </pre>
 *
 */

#include "../gcode.h"
#include "../../inc/MarlinConfig.h"

#if ENABLED(FACTORY_MACHINE_INFO)
#include "machine_info.h"

/**
 * M2115: Query firmware information;
 */
void GcodeSuite::M2115()
{
  SERIAL_ECHOPGM(MSG_M2115_REPORT);
  SERIAL_EOL();

  SERIAL_ECHOPGM("PROTOCOL_VERSION:" PROTOCOL_VERSION);
  SERIAL_EOL();

  SERIAL_ECHOPGM("MACHINE_TYPE:" MACHINE_NAME);
  SERIAL_EOL();

  SERIAL_ECHOPGM("UUID:");
  MachineInfo.print_uuid_info();
}

#endif // SPINDLE_LASER_ENABLE