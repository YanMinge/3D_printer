/**   
 * \par Copyright (C), 2018-2019, MakeBlock
 * @file    M2060.cpp
 * @author  Mark Yan
 * @version V1.0.0
 * @date    2019/05/24
 * @brief   source code for M2060.
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
 * Set/Query the product uuid.
 * \par History:
 * <pre>
 * `<Author>`         `<Time>`        `<Version>`        `<Descr>`
 * Mark Yan         2019/06/14     1.0.0            Initial function design.
 * </pre>
 *
 */

#include "../../inc/MarlinConfig.h"

#if ENABLED(FACTORY_MACHINE_UUID)

#include "../gcode.h"
#include "machine_uuid.h"

/**
 * M2060: Set/Query the product uuid.
 *      OR, with 'String data' set product uuid.
 *      OR, with 'NULL parameters' get the current product uuid.
 */
void GcodeSuite::M2060()
{
  for (char *fn = parser.string_arg; *fn; ++fn) if (*fn == ' ') *fn = '\0';
  uint8_t str_len = strlen(parser.string_arg);
  if(str_len <= 2)
  {
    SERIAL_ECHOPGM("uuid: ");
  	MachineUuid.print_info();
  }
  else
  {
    bool ret = MachineUuid.set_uuid_from_str(parser.string_arg);
	if(ret == false)
    {
      SERIAL_ECHOLNPGM("wrong parameter input");
    }
  }
}

#endif // FACTORY_MACHINE_UUID
