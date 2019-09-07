/**
 * \par Copyright (C), 2018-2019, MakeBlock
 * \class   lcd_file
 * \brief   lcd_file_list.
 * @file    lcd_file.h
 * @author  Akingpan
 * @version V1.0.0
 * @date    2019/05/15
 * @brief   Header file for lcd_file list.
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
 * This file is the fundamental function of lcd_file list.
 *
 * \par Method List:
 *
 *    1.  void          lcd_file::file_list_init(void);
 *
 * \par History:
 * <pre>
 * `<Author>`         `<Time>`        `<Version>`        `<Descr>`
 * Akingpan           2019/05/15       1.0.0              Initial function design.
 * </pre>
 *
 */

#ifdef TARGET_LPC1768
#include "../Marlin.h"
#include "../../module/planner.h"

#if ENABLED(USE_DWIN_LCD)
#include "dwin.h"
#include "lcd_file.h"
#include "lcd_process.h"
#include "lcd_parser.h"
#include "filament_ui.h"

#if ENABLED(USB_DISK_SUPPORT)
#include "udisk_reader.h"
#include "user_execution.h"

#if ENABLED(POWER_LOSS_RECOVERY)
#include "../../feature/power_loss_recovery.h"
#endif

filament_ui_show filament_show;

filament_ui_show::filament_ui_show()
{
  heating_status = HEAT_NULL_STATUS;
  print_after_heat_status = false;
}

void filament_ui_show::show_file_print_page(void)
{
  if(get_print_after_heat_status() && (HEAT_PRINT_STATUS == filament_show.get_heating_status_type()))
  {
    pfile_list_t temp = NULL;
    temp = LcdFile.file_list_index(dwin_parser.get_current_page_index());
    dwin_process.show_stop_print_file_page(temp);
    LcdFile.set_current_status(on_printing);
    set_print_after_heat_status(false);
    //set_heating_status_type(HEAT_NULL_STATUS);
  }
}

#endif // USB_DISK_SUPPORT
#endif // USE_DWIN_LCD
#endif // TARGET_LPC1768
