/**   
 * \par Copyright (C), 2018-2019, MakeBlock
 * @file    M2023.cpp
 * @author  Mark Yan
 * @version V1.0.0
 * @date    2019/05/24
 * @brief   source code for M2023.
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
 * Open a file in usb disk.
 * \par History:
 * <pre>
 * `<Author>`         `<Time>`        `<Version>`        `<Descr>`
 * Mark Yan         2019/05/24     1.0.0            Initial function design.
 * </pre>
 *
 */

#include "../../inc/MarlinConfig.h"

#if ENABLED(USB_DISK_SUPPORT)

#include "../gcode.h"
#include "udisk_reader.h"

#if ENABLED(USE_DWIN_LCD)
#include "lcd_process.h"
#include "lcd_parser.h"
#endif

#if ENABLED(POWER_LOSS_RECOVERY)
#include "../../feature/power_loss_recovery.h"
#endif

/**
 * M2023: Open a file
 */
void GcodeSuite::M2023()
{
  udisk.open_file(parser.string_arg, true);

  if (udisk.is_usb_detected() && udisk.is_file_open())
  {
    if(udisk.check_gm_file(udisk.get_file_name()))
    {
      uint32_t initial_time = udisk.get_print_time(udisk.get_file_name());
      dwin_parser.file_initial_time = initial_time;
	  uint32_t index = 0;
#if ENABLED(POWER_LOSS_RECOVERY)
      if(udisk.job_recover_file_exists())
      {
        //lcd image
        udisk.recovery_print_time_dynamic(recovery.info.print_job_elapsed);
		initial_time -= recovery.info.print_job_elapsed;
		index = recovery.info.udisk_pos;
        udisk.remove_job_recovery_file();
      }
	  else
      {
        index = udisk.get_gcode_offset(udisk.get_file_name());
      }
#else
      index = udisk.get_gcode_offset(udisk.get_file_name());
#endif
      udisk.set_index(index);
#if ENABLED(USE_DWIN_LCD)
      dwin_process.send_print_time(initial_time);
#endif
    }
    else
    {
      uint32_t index = 0;
#if ENABLED(POWER_LOSS_RECOVERY)
      if(udisk.job_recover_file_exists())
      {
        index = recovery.info.udisk_pos;
        udisk.remove_job_recovery_file();
      }
#endif
      udisk.set_index(index);
#if ENABLED(USE_DWIN_LCD)
      dwin_process.send_print_time(0);
#endif
    }
  }
}

#endif // USB_DISK_SUPPORT
