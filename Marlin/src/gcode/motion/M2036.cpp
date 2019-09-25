/**
 * \par Copyright (C), 2018-2019, MakeBlock
 * @file    M2036.cpp
 * @author  Mark Yan
 * @version V1.0.0
 * @date    2019/09/23
 * @brief   source code for M2036.
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
 * Z axis current height recording function.
 * \par History:
 * <pre>
 * `<Author>`         `<Time>`        `<Version>`        `<Descr>`
 * Mark Yan         2019/09/23     1.0.0            Initial function design.
 * </pre>
 *
 */

#include "../../inc/MarlinConfig.h"


#if ENABLED(TARGET_LPC1768)
#include "../gcode.h"

#if ENABLED(FACTORY_MACHINE_INFO)
#include "machine_info.h"

/**
 * M2036: Z axis current height recording function;
 */
void GcodeSuite::M2036()
{
  if (parser.seen('S'))
  {
    MachineInfo.set_z_axis_height(parser.value_float());
  }
  else
  {
    SERIAL_PRINTF("Z axis height is %f mm\r\n", MachineInfo.get_z_axis_height());
  }
}
#endif //FACTORY_MACHINE_INFO
#endif // TARGET_LPC1768