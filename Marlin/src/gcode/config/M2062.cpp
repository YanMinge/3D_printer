/**   
 * \par Copyright (C), 2018-2019, MakeBlock
 * @file    M2062.cpp
 * @author  Mark Yan
 * @version V1.0.0
 * @date    2019/06/20
 * @brief   source code for M2062.
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
 * Query the head type.
 * \par History:
 * <pre>
 * `<Author>`         `<Time>`        `<Version>`        `<Descr>`
 * Mark Yan         2019/06/20     1.0.0            Initial function design.
 * </pre>
 *
 */

#include "../../inc/MarlinConfig.h"

#if ENABLED(FACTORY_MACHINE_INFO)

#include "../gcode.h"
#include "machine_info.h"

/**
 * M2062: Query the head type.
 */
void GcodeSuite::M2062()
{ 
  SERIAL_ECHOPGM("head type: ");
  head_t head_type = MachineInfo.get_head_type();
  if(head_type == HEAD_NULL)
  {
    SERIAL_ECHOLNPGM("no head");
  }
  else if(head_type == HEAD_LASER)
  {
    SERIAL_ECHOLNPGM("laser head");
  }
  else
  {
    SERIAL_ECHOLNPGM("print head");
  }
}

#endif // FACTORY_MACHINE_INFO