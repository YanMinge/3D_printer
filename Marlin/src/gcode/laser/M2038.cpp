/**
 * \par Copyright (C), 2018-2019, MakeBlock
 * @file    M2038.cpp
 * @author  Mark Yan
 * @version V1.0.0
 * @date    2019/06/11
 * @brief   source code for M2038.
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
 * Laser height adjustment.
 * \par History:
 * <pre>
 * `<Author>`         `<Time>`        `<Version>`        `<Descr>`
 * Mark Yan         2019/08/15     1.0.0            Initial function design.
 * </pre>
 *
 */

#include "../../inc/MarlinConfig.h"


#if ENABLED(TARGET_LPC1768)
#if ENABLED(SPINDLE_LASER_ENABLE)
#include "laser.h"
#include "../gcode.h"

/**
 * M2038: Laser height adjustment;
 */
void GcodeSuite::M2038()
{
  //
}

#endif // SPINDLE_LASER_ENABLE
#endif // TARGET_LPC1768